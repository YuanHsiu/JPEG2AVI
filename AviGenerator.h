//AviGenerator.h
//---------------------------------------------------------------------------

#ifndef _AVI_GENERATOR_H_
#define _AVI_GENERATOR_H_
#define DEBUG_VERSION

#include <stdio.h>
#include <string>
#include <vector>
#include "AviFormat.h"
//---------------------------------------------------------------------------

using namespace std;

/* 4 bytes */
//typedef int WORD;
//typedef unsigned int DWORD;
/* 1 byte */
typedef unsigned char BYTE;

class AviGenerator
{
public:
    AviGenerator(void);
    ~AviGenerator(void);

    void set_avi_file (string const& file);
    void set_fps (int fps);
    int add_frame (string const& file);
    void set_avi_size (int w, int h);

    int generate_avi ();
private:
    int file_size (string const& file);
    int files_size ();

    int initalize_header ();

    void print_quartet (FILE* file, unsigned int i);
public:
    static unsigned int const MAX_RIFF_SZ = 2147483648LL; /*Max avi file size, 2 GB limit*/
    static unsigned int const JPEG_DATA_SZ = sizeof(unsigned int) * 2;

    struct Jpeg_Data
    {
        unsigned int size;
        unsigned int offset;
        string name; /* i.e. variable length structure */

        Jpeg_Data () {}
        ~Jpeg_Data () {}

        friend bool operator< (Jpeg_Data const& a, Jpeg_Data const& o) {
            return a.name < o.name;
        }
        static bool lestthan (Jpeg_Data const* a, Jpeg_Data const* o) {
            return a->name < o->name;
        }
    };
protected:
    string avi_name_;
    int fps_;
    int frames_;
    int usec_per_frame;
    //file structure
    AVI_list_hdrl list_hdrl;
    //file list
    vector <Jpeg_Data*> jpeg_list;
    //option
    unsigned int width;
    unsigned int height;
    unsigned int frames;
    unsigned int fps;
    long jpg_sz;
    size_t jpg_sz_64;
    size_t riff_sz_64;
    unsigned int riff_sz;
};
//---------------------------------------------------------------------------
#endif
