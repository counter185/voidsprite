#ifndef TGA_PRIVATE_H
#define TGA_PRIVATE_H

#include "tga.h"

tlong __TGASeek(TGA *tga, tlong off, int whence);

void __TGAbgr2rgb(tbyte *data, size_t size, size_t stride);

#define TGA_HEADER_SIZE         18
#define TGA_CMAP_SIZE(tga)      ((tga)->hdr.map_len * (tga)->hdr.map_entry / 8)
#define TGA_CMAP_OFF(tga) 	(TGA_HEADER_SIZE + (tga)->hdr.id_len)
#define TGA_IMG_DATA_OFF(tga) 	(TGA_HEADER_SIZE + (tga)->hdr.id_len + TGA_CMAP_SIZE(tga))
#define TGA_IMG_DATA_SIZE(tga)	((tga)->hdr.width * (tga)->hdr.height * (tga)->hdr.depth / 8)
#define TGA_SCANLINE_SIZE(tga)	((tga)->hdr.width * (tga)->hdr.depth / 8)
#define TGA_CAN_SWAP(depth)     (depth == 24 || depth == 32)

#define TGA_IS_BW(tga)          ((((tga)->hdr.img_t & 0x3)==0x3) ? 1 : 0)

const char *__TGAStrError(tuint8 code);

tuint8 TGA_handle_set_error(TGA *tga, tuint8 code);

#define __TGA_SUCCEEDED(TGA) ((TGA)->last == TGA_OK)

#define __TGA_LASTERR(TGA) ((TGA)->last)

#ifdef TGA_DEBUG
#define TGA_DBG_PRINTF(...) fprintf(stderr, __VA_ARGS__)
#include <errno.h>
#include <string.h>
#define TGA_PRINT_ERRNO() do { \
		if (errno) { \
			TGA_DBG_PRINTF("errno=%d, %s\n", errno, strerror(errno)); \
		} \
	} while (0)
#else
#define TGA_DBG_PRINTF(...)
#define TGA_PRINT_ERRNO()
#endif


#define TGA_ERROR(tga, code) \
	do { \
		TGA_DBG_PRINTF("%s:%u %s\n", __FILE__, __LINE__, TGAStrErrorCode(code)); \
		TGA_PRINT_ERRNO(); \
		TGA_handle_set_error((tga), (code)); \
	} while (0)

#endif /* TGA_PRIVATE_H */
