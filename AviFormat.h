//AviFormat.h
//---------------------------------------------------------------------------

#ifndef _AVI_FORMAT_H_
#define _AVI_FORMAT_H_

#include <iostream>
//---------------------------------------------------------------------------

using namespace std;

/* 4 bytes */
//typedef int WORD;
//typedef unsigned int DWORD;

/* for use in AVI_avih.flags */
const unsigned int AVIF_HASINDEX = 0x00000010; /* index at end of file */
const unsigned int AVIF_MUSTUSEINDEX = 0x00000020;
const unsigned int AVIF_ISINTERLEAVED = 0x00000100;
const unsigned int AVIF_TRUSTCKTYPE = 0x00000800;
const unsigned int AVIF_WASCAPTUREFILE = 0x00010000;
const unsigned int AVIF_COPYRIGHTED = 0x00020000;

struct AVI_avih 
{
    unsigned int usec_per_frame;   //* frame display rate (or 0L) */
    unsigned int max_bytes_per_sec; //* max. transfer rate */
    unsigned int padding;    //* pad to multiples of this size; */
    /* normally 2K */
    unsigned int flags;
    unsigned int tot_frames;   //* # frames in file */
    unsigned int init_frames;
    unsigned int streams;
    unsigned int buff_sz;
    unsigned int width;
    unsigned int height;
    unsigned int reserved[4];

    AVI_avih () {}
    AVI_avih (unsigned int usec, unsigned int max, unsigned int pad, unsigned int flags, unsigned int tot, unsigned int init, unsigned int str, unsigned int buff, unsigned int w, unsigned int h, unsigned int* re = NULL)
        :usec_per_frame (usec), max_bytes_per_sec (max), padding (pad), tot_frames (tot)
        ,init_frames (init), streams (str), buff_sz (buff), width (w), height (h) {
        if (re != NULL)
            for (int i = 0; i < 4; ++i)
                reserved[i] = re[i];
        else
            for (int i = 0; i < 4; ++i)
                reserved[i] = 0;
    }
    AVI_avih (AVI_avih const& avih) {
        usec_per_frame = avih.usec_per_frame;
        max_bytes_per_sec = avih.max_bytes_per_sec;
        padding = avih.padding;

        flags = avih.flags;
        tot_frames = avih.tot_frames;
        init_frames = avih.init_frames;
        streams = avih.streams;
        buff_sz = avih.buff_sz;
        width = avih.width;
        height = avih.height;
        for (int i; i < 4; ++i)
            reserved[i] = avih.reserved[i];
    }
    ~AVI_avih (){}

    friend ostream& operator << (ostream& out, AVI_avih const& avih) {
        out << avih.usec_per_frame << avih.max_bytes_per_sec << avih.padding << avih.flags << avih.tot_frames << avih.init_frames;
        out << avih.streams << avih.buff_sz << avih.width << avih.height;
        out << avih.reserved[0] << avih.reserved[1] << avih.reserved[2] << avih.reserved[3];
        return out;
    }
};

struct AVI_strh
{
    unsigned char type[4];      ///* stream type */
    unsigned char handler[4];
    unsigned int flags;
    unsigned int priority;
    unsigned int init_frames;       ///* initial frames (???) */
    unsigned int scale;
    unsigned int rate;
    unsigned int start;
    unsigned int length;
    unsigned int buff_sz;           ///* suggested buffer size */
    unsigned int quality;
    unsigned int sample_sz;

    AVI_strh () {}
    AVI_strh (char const* t, char const* h, unsigned int f, unsigned int p, unsigned int in, unsigned int sc, unsigned int r, unsigned int st, unsigned int l, unsigned int bs, unsigned int q, unsigned int ss)
        : flags (f), priority (p), init_frames (in), scale (sc)
        , rate (r), start (st), length (l), buff_sz (bs)
        , quality (q) ,sample_sz (ss) {
        for (int i = 0; i < 4; ++i) {
            type[i] = t[i];
            handler[i] = h[i];
        }
    }
    AVI_strh (AVI_strh const& strh) {
        flags = strh.flags;
        priority = strh.priority;
        init_frames = strh.init_frames;
        scale = strh.scale;
        rate = strh.rate;
        start = strh.start;
        length = strh.length;
        buff_sz = strh.buff_sz;
        quality = strh.quality;
        sample_sz = strh.sample_sz;
        for (int i = 0; i < 4; ++i) {
            type[i] = strh.type[i];
            handler[i] = strh.handler[i];
        }
    }
    ~AVI_strh () {}

    friend ostream& operator << (ostream& out, AVI_strh const& strh) {
        out << strh.type[0] << strh.type[1] << strh.type[2] << strh.type[3];
        out << strh.handler[0] << strh.handler[1] << strh.handler[2] << strh.handler[3];
        out << strh.flags << strh.priority << strh.init_frames << strh.scale << strh.rate;
        out << strh.start << strh.length << strh.buff_sz << strh.quality << strh.sample_sz;
        return out;
    }
};

struct AVI_strf
{       
    unsigned int sz;
    unsigned int width;
    unsigned int height;
    unsigned int planes_bit_cnt;
    unsigned char compression[4];
    unsigned int image_sz;
    unsigned int xpels_meter;
    unsigned int ypels_meter;
    unsigned int num_colors;        // used colors
    unsigned int imp_colors;        // important colors
    /*
    may be more for some codecs
    */
    AVI_strf () {}
    AVI_strf (unsigned int s, unsigned int w, unsigned int h, unsigned int p, char const* c, unsigned int i, unsigned int x, unsigned int y, unsigned int n, unsigned int imp)
        :sz (s), width (w), height (h), planes_bit_cnt (p), image_sz (i)
        ,xpels_meter (x), ypels_meter (y), num_colors (n), imp_colors (imp) {
        for (int i = 0; i < 4; ++i)
            compression[i] = c[i];
    }
    AVI_strf (AVI_strf const& strf) {
        sz = strf.sz;
        width = strf.width;
        height = strf.height;
        planes_bit_cnt = strf.planes_bit_cnt;
        image_sz = strf.image_sz;
        xpels_meter = strf.xpels_meter;
        ypels_meter = strf.ypels_meter;
        num_colors = strf.num_colors;
        imp_colors = strf.imp_colors;
        for (int i = 0; i < 4; ++i)
            compression[i] = strf.compression[i];
    }
    ~AVI_strf () {}

    friend ostream& operator << (ostream& out, AVI_strf const& strf) {
        out << strf.sz << out << strf.width << strf.height << strf.planes_bit_cnt;
        out << strf.compression[0] << strf.compression[1] << strf.compression[2] << strf.compression[3];
        out << strf.image_sz << strf.xpels_meter << strf.ypels_meter << strf.num_colors << strf.imp_colors;
        return out;
    }
};

/*
  AVI_list_hdr
  spc: a very ubiquitous AVI struct, used in list structures
       to specify type and length
*/
struct AVI_list_hdr
{
    unsigned char id[4];   /* "LIST" */
    unsigned int sz;              /* size of owning struct minus 8 */
    unsigned char type[4]; /* type of list */

    AVI_list_hdr () {}
    AVI_list_hdr (char const* list, unsigned int d, char const* value) {
        sz = d;
        for (int i = 0; i < 4; ++i) {
            id[i] = list[i];
            type[i] = value[i];
        }
    }
    AVI_list_hdr (AVI_list_hdr const& hdr) {
        for (int i = 0; i < 4; ++i) {
            id[i] = hdr.id[i];
            type[i] = hdr.type[i];
        }
        sz = hdr.sz;
    }
    ~AVI_list_hdr () {}

    friend ostream& operator<< (ostream& out, AVI_list_hdr const& hdr) {
        out << hdr.id[0] << hdr.id[1] << hdr.id[3] << hdr.id[4] << hdr.sz << hdr.type[0] << hdr.type[1] << hdr.type[2] << hdr.type[3];
        return out;
    }
};

struct AVI_list_odml
{
    struct AVI_list_hdr list_hdr;

    unsigned char id[4];
    unsigned int sz;
    unsigned int frames;

    AVI_list_odml () {}
    AVI_list_odml (char const* l1, unsigned int d1, char const* v1, char const* l2, unsigned int d2, unsigned int f)
        :list_hdr (l1, d1, v1), sz (d2), frames (f) {
        for (int i = 0; i < 4; ++i)
            id[i] = l2[i];
    }
    AVI_list_odml (AVI_list_hdr const hdr, char const* v, unsigned int d2, unsigned int f)
        :list_hdr (hdr), sz (d2), frames (f) {
        for (int i = 0; i < 4; ++i)
            id[i] = v[i];
    }
    AVI_list_odml (AVI_list_odml const& odml)
        :list_hdr (odml.list_hdr), sz (odml.sz), frames (odml.frames) {
        for (int i = 0; i < 4; ++i)
            id[i] = odml.id[i];
    }
    ~AVI_list_odml () {}

    friend ostream& operator << (ostream& out, AVI_list_odml const& odml){
        out << odml.list_hdr << odml.id[0] << odml.id[1] << odml.id[2] << odml.id[3];
        out << odml.sz << odml.frames;
        return out;
    }
};

struct AVI_list_strl 
{
    struct AVI_list_hdr list_hdr;
    /* chunk strh */
    unsigned char strh_id[4];
    unsigned int strh_sz;
    struct AVI_strh strh;
    /* chunk strf */
    unsigned char strf_id[4];
    unsigned int strf_sz;
    struct AVI_strf strf;
    /* list odml */
    struct AVI_list_odml list_odml;
};

struct AVI_list_hdrl
{
    struct AVI_list_hdr list_hdr;
    /* chunk avih */
    unsigned char avih_id[4];
    unsigned int avih_sz;
    struct AVI_avih avih;
    /* list strl */
    struct AVI_list_hdr strl_hdr;
    /* chunk strh */
    unsigned char strh_id[4];
    unsigned int strh_sz;
    struct AVI_strh strh;
    /* chunk strf */
    unsigned char strf_id[4];
    unsigned int strf_sz;
    struct AVI_strf strf;
    /* list odml */
    struct AVI_list_odml list_odml;

    AVI_list_hdrl (unsigned int width = 0, unsigned int height = 0, unsigned int jpg_sz = 1, unsigned int per_usec = 1, unsigned int frames = 1)
        :list_hdr ("LIST", sizeof (struct AVI_list_hdrl) - 8, "hdrl")
        //* chunk avih */
        ,avih_sz (sizeof (struct AVI_avih))
        ,avih (per_usec, 1000000 * (jpg_sz/frames) / per_usec, (0)
        ,AVIF_HASINDEX, frames, 0, 1, 0, width, height)
        // list strl
        ,strl_hdr ("LIST", sizeof (struct AVI_list_strl) - 8, "strl")
        // chunk strh
        ,strh_sz (sizeof (struct AVI_strh))
        ,strh ("vids", "MJPG", 0, 0, 0, per_usec, 1000000
        , 0, frames, 0, 0, 0)
        // chunk strf
        ,strf_sz (sizeof (AVI_strf))
        ,strf (sizeof (struct AVI_strf), width, height, 1 + 24*256*256
         ,"MJPG", width * height * 3, 0, 0, 0, 0)
        // list odml
        ,list_odml ("LIST", 16, "odml", "dmlh", 4, frames) {
            avih_id[0] = 'a'; avih_id[1] = 'v'; avih_id[2] = 'i'; avih_id[3] = 'h';
            strh_id[0] = 's'; strh_id[1] = 't'; strh_id[2] = 'r'; strh_id[3] = 'h';
            strf_id[0] = 's'; strf_id[1] = 't'; strf_id[2] = 'r'; strf_id[3] = 'f';
        }
    ~AVI_list_hdrl (){}

    friend ostream& operator << (ostream& out, AVI_list_hdrl const& hdrl) {
        out << hdrl.list_hdr << hdrl.avih_id[0] << hdrl.avih_id[1] << hdrl.avih_id[2] << hdrl.avih_id[3];
        out << hdrl.avih_sz << hdrl.strl_hdr;
        out << hdrl.strh_id[0] << hdrl.strh_id[1] << hdrl.strh_id[2] << hdrl.strh_id[3];
        out << hdrl.strh_sz << hdrl.strh;
        out << hdrl.strf_id[0] << hdrl.strf_id[1] << hdrl.strf_id[2] << hdrl.strf_id[3];
        out << hdrl.strf_sz << hdrl.strf << hdrl.list_odml;
        return out;
    }
};
//---------------------------------------------------------------------------
#endif //_AVI_FORMAT_H_
