/*
 *  tga.h - Libtga header
 *
 *  Copyright (C) 2001-2002, Matthias Brueckner
 *  This file is part of the TGA library (libtga).
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __TGA_H
#define __TGA_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* sections */
#define TGA_IMAGE_ID	0x01
#define TGA_IMAGE_INFO	0x02
#define TGA_IMAGE_DATA	0x04
#define TGA_COLOR_MAP	0x08
/* RLE */
#define TGA_RLE_ENCODE  0x10

/* color format */
#define TGA_RGB		0x20
#define TGA_BGR		0x40

/* orientation */
#define TGA_BOTTOM	0x0
#define TGA_TOP		0x1
#define TGA_LEFT	0x0
#define TGA_RIGHT	0x1

/* image types */
#define TGA_IMGTYPE_NOIMAGE             0
#define TGA_IMGTYPE_UNCOMP_CMAP         1
#define TGA_IMGTYPE_UNCOMP_TRUEC        2
#define TGA_IMGTYPE_UNCOMP_BW           3
#define TGA_IMGTYPE_RLE_CMAP            9
#define TGA_IMGTYPE_RLE_TRUEC          10
#define TGA_IMGTYPE_RLE_BW             11

#define TGA_IMGTYPE_CMAP_FLAG           0x1
#define TGA_IMGTYPE_TRUEC_FLAG          0x2
#define TGA_IMGTYPE_BW_FLAG             0x3

#define TGA_IMGTYPE_AVAILABLE(tga)      (((tga)->hdr.img_t) != 0)
#define TGA_IMGTYPE_IS_MAPPED(tga)      ((((tga)->hdr.img_t) & 0x3) == 0x1)
#define TGA_IMGTYPE_IS_TRUEC(tga)       ((((tga)->hdr.img_t) & 0x3) == 0x2)
#define TGA_IMGTYPE_IS_BW(tga)          ((((tga)->hdr.img_t) & 0x3) == 0x3)
#define TGA_IMGTYPE_IS_ENCODED(tga)     ((((tga)->hdr.img_t) & 0x8) == 0x8)

#define TGA_HAS_ID(tga)         ((tga)->hdr.id_len != 0)
#define TGA_IS_MAPPED(tga)      ((tga)->hdr.map_t == 1)

/* error codes */
enum {
	TGA_OK = 0, 		/* success */
	TGA_ERROR,
	TGA_OOM,		/* out of memory */
	TGA_OPEN_FAIL,
	TGA_SEEK_FAIL,
	TGA_READ_FAIL,
	TGA_WRITE_FAIL,
	TGA_UNKNOWN_SUB_FORMAT, /* invalid bit depth */
	TGA_ERRORS_NB
};

typedef uint32_t	tuint32;
typedef uint16_t	tuint16;
typedef uint8_t	tuint8;

typedef tuint8	tbyte;
typedef tuint16	tshort;
typedef tuint32	tlong;

typedef struct _TGAHeader TGAHeader;
typedef struct _TGAData	  TGAData;
typedef struct _TGA	  TGA;

typedef void (*TGAErrorProc)(TGA*, int);


/* TGA image header */
struct _TGAHeader {
	tbyte	id_len;		/* F1: image id length */
	tbyte	map_t;		/* F2: color map type */
	tbyte	img_t;		/* F3: image type */
	tshort	map_first;	/* F4.1: index of first map entry */
	tshort	map_len;	/* F4.2: number of entries in color map */
	tbyte	map_entry;	/* F4.3: bit-depth of a cmap entry */
	tshort	x;		/* F5.1: x-coordinate */
	tshort	y;		/* F5.2: y-coordinate */
	tshort	width;		/* F5.3: width of image */
	tshort	height;		/* F5.4: height of image */
	tbyte	depth;		/* F5.5: pixel-depth of image */
	tbyte	alpha;		/* F5.6 (bits 3-0): alpha bits */
	tbyte	horz;		/* F5.6 (bit 4): horizontal orientation */
	tbyte	vert;		/* F5.6 (bit 5): vertical orientation */
};

/* TGA image data */
struct _TGAData {
	tbyte	*img_id;	/* F6: image id */
	tbyte	*cmap;		/* F7: color map */
	tbyte	*img_data;	/* F8: image data */
	tuint32	 flags;
};

/* TGA image handle */
struct _TGA {
	FILE*		fd;		/* file stream */
	tlong		off;		/* current offset in file*/
	int		last;		/* last error code */
	TGAHeader	hdr;		/* image header */
	TGAErrorProc 	error;		/* user-defined error proc */
};

TGA* TGAOpen(const char *name, const char *mode);

TGA* TGAOpenFd(FILE *fd);


int TGAReadHeader(TGA *tga);

int TGAReadImageId(TGA *tga, TGAData *data);

int TGAReadColorMap(TGA *tga, TGAData *data);

int TGAReadScanlines(TGA *tga, TGAData *data);

int TGAReadImage(TGA *tga, TGAData *data);

void TGAFreeTGAData(TGAData *data);

int TGAWriteHeader(TGA *tga);

int TGAWriteImageId(TGA *tga, TGAData *data);

int TGAWriteColorMap(TGA *tga, TGAData *data);

size_t TGAWriteScanlines(TGA *tga, TGAData *ata);

int TGAWriteImage(TGA *tga, TGAData *data);

void TGAClose(TGA *tga);

void TGAClearError(TGA *tga);

#define TGA_SUCCEEDED(TGA) (((TGA) != 0) && ((TGA)->last == TGA_OK))

const char *TGAStrErrorCode(tuint8 code);
const char* TGAStrError(TGA *tga);

#ifdef __cplusplus
}
#endif

#endif /* __TGA_H */
