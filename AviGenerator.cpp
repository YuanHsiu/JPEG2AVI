//AviGenerator.cpp
//---------------------------------------------------------------------------

#pragma hdrstop

#include "AviGenerator.h"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>
#ifdef _WIN32
    #include <io.h>
    #include <errno.h>
#else
    #include <sys/param.h>
    #include <unistd.h>
#endif
//---------------------------------------------------------------------------

#pragma package(smart_init)

AviGenerator::AviGenerator(void)
    :fps (15) ,frames (1), jpg_sz (1), width (320), height (240)
{
    usec_per_frame = 1000000 / fps;
}

AviGenerator::~AviGenerator(void)
{
    for (vector<Jpeg_Data*>::iterator iter = this->jpeg_list.begin (); iter != jpeg_list.end (); ++iter) {
        Jpeg_Data* p = *iter;
        delete p;
    }
    this->jpeg_list.clear ();
}

void AviGenerator::set_avi_file (string const& file)
{
    this->avi_name_ = file;
}

void AviGenerator::set_fps (int fps)
{
    this->fps_ = fps;
    this->usec_per_frame = 1000000 / fps;
}

void AviGenerator::set_avi_size (int w, int h)
{
    this->width = w;
    this->height = h;
}

int AviGenerator::file_size (string const& file)
{
    int ret;
#ifdef _WIN32
    struct stati64 result;
    if (-1 == _stati64 (file.c_str (), &result))
        return -1;
    ret = result.st_size;
#else
    struct stat s;
    if (-1 == stat(file.c_str (), &s))
        return -1;
    ret = s.st_size;
#endif
#ifdef DEBUG_VERSION
    cerr << file.c_str () << " size is " << ret << endl;
#endif
    return ret;
}

int AviGenerator::files_size ()
{
    int ret = 0;
    int tmp, it;
    for (vector<Jpeg_Data*>::iterator iter = jpeg_list.begin (); iter != jpeg_list.end (); ++iter) {
        it = (*iter)->size;
        tmp = it != 0?it:file_size ((*iter)->name.c_str ());
        ret += tmp;
        ret += (4 - (tmp % 4)) % 4;
    }
    return ret;
}

int AviGenerator::add_frame (string const& file)
{
    int size = file_size (file);
    if (size <= 0)
        return -1;
    struct Jpeg_Data* jpeg= new Jpeg_Data ();
    jpeg->name = file;
    jpeg->size = size;
    jpeg->offset = 0;
    this->jpeg_list.insert (jpeg_list.begin (), jpeg);
    return 0;
}

void AviGenerator::print_quartet (FILE* file, unsigned int i)
{
    for (int j = 0; j < 4; ++j) {
        fputc (i % 0x0100, file);
        i /= 0x100;
    }
}

int AviGenerator::initalize_header ()
{
    frames = this->jpeg_list.size ();
    if (frames <= 0)
        return -1;
    /* getting image, and hence, riff sizes */
    jpg_sz_64 = this->files_size ();
    if (-1 == jpg_sz_64) {
        cerr << "couldn't determine size of images" << endl;
        return -2;
    }
    riff_sz_64 = sizeof (struct AVI_list_hdrl) + 4 + 4 + jpg_sz_64 + 8 * frames + 8 + 8 + 16 * frames;

    if (riff_sz_64 >= MAX_RIFF_SZ) {
        cerr << "RIFF would exceed 2 Gb limit" << endl;
        return -3;
    }
    jpg_sz = (long) jpg_sz_64;
    riff_sz = (unsigned int) riff_sz_64;

    //update the struct AVI_list_hdrl
    /*
    this->list_hdrl.avih.usec_per_frame = LILEND4(usec_per_frame);
    this->list_hdrl.avih.max_bytes_per_sec = LILEND4((int)1000000 * (jpg_sz / frames) / usec_per_frame);
    this->list_hdrl.avih.flags = LILEND4(AVIF_HASINDEX);
    this->list_hdrl.avih.tot_frames = LILEND4(frames);
    this->list_hdrl.avih.width = LILEND4(width);
    this->list_hdrl.avih.height = LILEND4(height);
    this->list_hdrl.strh.scale = LILEND4(usec_per_frame);
    this->list_hdrl.strh.rate = LILEND4(1000000);
    this->list_hdrl.strh.length = LILEND4(frames);
    this->list_hdrl.strf.width = LILEND4(width);
    this->list_hdrl.strf.height = LILEND4(height);
    this->list_hdrl.strf.image_sz = LILEND4(width * height * 3);
    this->list_hdrl.list_odml.frames = LILEND4(frames);
    */
    this->list_hdrl.avih.usec_per_frame = usec_per_frame;
    this->list_hdrl.avih.max_bytes_per_sec = (int)1000000 * (jpg_sz / frames) / usec_per_frame;
    this->list_hdrl.avih.flags = AVIF_HASINDEX;
    this->list_hdrl.avih.tot_frames = frames;
    this->list_hdrl.avih.width = width;
    this->list_hdrl.avih.height = height;
    this->list_hdrl.strh.scale = usec_per_frame;
    this->list_hdrl.strh.rate = 1000000;
    this->list_hdrl.strh.length = frames;
    this->list_hdrl.strf.width = width;
    this->list_hdrl.strf.height = height;
    this->list_hdrl.strf.image_sz = width * height * 3;
    this->list_hdrl.list_odml.frames = frames;

    return 0;
}

int AviGenerator::generate_avi ()
{
    if (this->initalize_header () != 0)
        return -1;

    //open the file
    FILE* fdest = fopen (this->avi_name_.c_str (), "wb");
    if (NULL == fdest) {
        cerr << "Can't create a new file to write (" << this->avi_name_ << ")!" << endl;
        return -2;
    }
    //fwrite (&list_hdrl.avih.max_bytes_per_sec, 4, 1, fdest);
    long nbr;
    long nbw;
    long tnbw = 0;
    long mfsz;
    long remnant;
    char buff[512];

    //write file header
    fwrite ("RIFF", 4, 1, fdest);
    print_quartet (fdest, riff_sz);
    fwrite ("AVI ", 4, 1, fdest);
    fwrite (&list_hdrl, sizeof (struct AVI_list_hdrl), 1, fdest);

    //sort the list by file name
    sort (this->jpeg_list.begin (), this->jpeg_list.end (), AviGenerator::Jpeg_Data::lestthan);
    // list movi
    size_t offset = 4;
    fwrite ("LIST", 4, 1, fdest);
    print_quartet (fdest, jpg_sz + 8 * frames + 4);
    fwrite ("movi", 4, 1, fdest);
    //write video data
    for (vector <Jpeg_Data*>::iterator iter = this->jpeg_list.begin (); iter != jpeg_list.end (); ++iter) {
#ifdef DEBUG_VERSION
        cout << "dealing with " << (*iter)->name << endl;
#endif
        fwrite ("00db", 4, 1, fdest);
        mfsz = (*iter)->size;
        remnant = (4 - (mfsz % 4)) % 4;
        print_quartet (fdest, mfsz + remnant);
        (*iter)->size += remnant;
        (*iter)->offset = offset;
        offset += (*iter)->size + 8;

        int fd;
#ifdef _WIN32
        fd = open ((*iter)->name.c_str (), O_RDONLY | O_BINARY);
#else
        fd = open ((*iter)->name.c_str (), O_RDONLY);
#endif
        if (fd < 0) {
            cerr << "couldn't open file (" << (*iter)->name << ")!" << endl;
            fclose (fdest);
            return -3;
        }
        nbw = 0;

        if ((nbr = read (fd, buff, 6)) != 6) {
            cerr << "reading error" << endl;
            fclose (fdest);
            close (fd);
            return -4;
        }
        fwrite (buff, nbr, 1, fdest);
        read (fd, buff, 4);
        fwrite ("AVI1", 4, 1, fdest);
        nbw = 10;
        while ((nbr = read (fd, buff, 512)) > 0){
#ifdef DEBUG_VERSION
            //cout << "read " << nbr << " bytes from " << (*iter)->name << endl;
#endif
            fwrite (buff, nbr, 1, fdest);
            nbw += nbr;
        }

        if (remnant > 0) {
            fwrite (buff, remnant, 1, fdest);
            nbw += remnant;
        }
        tnbw += nbw;
        close (fd);
    }
    if (tnbw != jpg_sz) {
        cerr << "error writing images (wrote " << tnbw << " bytes, expected " << jpg_sz << " bytes)" << endl;
        fclose (fdest);
        return -5;
    }

    /* indices */
    fwrite ("idx1", 4, 1, fdest);
    print_quartet (fdest, 16 * frames);
    for (vector <Jpeg_Data*>::iterator iter = this->jpeg_list.begin (); iter != jpeg_list.end (); ++iter) {
        fwrite ("00db", 4, 1, fdest);
        print_quartet (fdest, 18);
        print_quartet (fdest, (*iter)->offset);
        print_quartet (fdest, (*iter)->size);
    }
    //this->jpeg_list.clear ();
    fclose (fdest);
    return 0;
}