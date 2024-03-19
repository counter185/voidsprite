/*
 *  tgawrite.c
 *
 *  Copyright (C) 2001-2002  Matthias Brueckner  <matbrc@gmx.de>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tga.h"
#include "tga_private.h"

#define LSB_SH(SHORT) ((SHORT) & 0xff)
#define MSB_SH(SHORT) ((SHORT) >> 8)

size_t
TGAWrite(TGA 	     *tga, 
	 const tbyte *buf, 
	 size_t       size, 
	 size_t       n)
{
	size_t wrote = fwrite(buf, size, n, tga->fd);
	if (wrote != n) {
		TGA_ERROR(tga, TGA_WRITE_FAIL);
	}
	tga->off = ftell(tga->fd);
	return wrote;
}


int TGAWriteImage(TGA 	  *tga, 
		  TGAData *data)
{
	if (!tga) return TGA_ERROR;

	if (data->flags & TGA_IMAGE_ID) {
		TGAWriteImageId(tga, data);
		if (!__TGA_SUCCEEDED(tga)) {
			return __TGA_LASTERR(tga);
		}
	}

	if (data->flags & TGA_IMAGE_DATA) {
		TGAWriteColorMap(tga, data);
		if (!__TGA_SUCCEEDED(tga)) {
			return __TGA_LASTERR(tga);
		}

		TGAWriteScanlines(tga, data);
		if (!__TGA_SUCCEEDED(tga)) {
			return __TGA_LASTERR(tga);
		}
	}

	TGAWriteHeader(tga);
	if (!__TGA_SUCCEEDED(tga)) {
		return __TGA_LASTERR(tga);
	}

	return tga->last;
}


int
TGAWriteHeader(TGA *tga)
{
	if (!tga) return 0;

	__TGASeek(tga, 0, SEEK_SET);
	if (!__TGA_SUCCEEDED(tga)) {
		return __TGA_LASTERR(tga);
	}

	tbyte tmp[TGA_HEADER_SIZE];

	if (tga->hdr.map_t != 0) {
		tmp[1] = 1;
		tmp[3] = LSB_SH(tga->hdr.map_first);
		tmp[4] = MSB_SH(tga->hdr.map_first);
		tmp[5] = LSB_SH(tga->hdr.map_len);
		tmp[6] = MSB_SH(tga->hdr.map_len);
		tmp[7] = tga->hdr.map_entry;
	} else {
		memset(tmp, 0, 8);
	}

	tmp[0] = tga->hdr.id_len;
	tmp[2] = tga->hdr.img_t;

	tmp[8] = LSB_SH(tga->hdr.x);
	tmp[9] = MSB_SH(tga->hdr.x);
	tmp[10] = LSB_SH(tga->hdr.y);
	tmp[11] = MSB_SH(tga->hdr.y);
	tmp[12] = LSB_SH(tga->hdr.width);
	tmp[13] = MSB_SH(tga->hdr.width);
	tmp[14] = LSB_SH(tga->hdr.height);
	tmp[15] = MSB_SH(tga->hdr.height);
	tmp[16] = tga->hdr.depth;
	tmp[17] = tga->hdr.alpha;
	tmp[17] |= (tga->hdr.horz << 4);
	tmp[17] |= (tga->hdr.vert << 5);

	TGAWrite(tga, tmp, TGA_HEADER_SIZE, 1);
	if (!__TGA_SUCCEEDED(tga)) {
		return __TGA_LASTERR(tga);
	}

	return TGA_OK;
}


int
TGAWriteImageId(TGA	*tga,
		TGAData	*data)
{
	if (!tga) return TGA_ERROR;

	if (tga->hdr.id_len == 0) return TGA_OK;

	if (!data) {
		TGA_ERROR(tga, TGA_ERROR);
		return __TGA_LASTERR(tga);
	}

	if (!data->img_id) {
		data->flags &= ~TGA_IMAGE_ID;
		TGA_ERROR(tga, TGA_ERROR);
		return __TGA_LASTERR(tga);
	}

	__TGASeek(tga, TGA_HEADER_SIZE, SEEK_SET);
	if (!__TGA_SUCCEEDED(tga)) {
		return __TGA_LASTERR(tga);
	}

	if (!TGAWrite(tga, data->img_id, tga->hdr.id_len, 1)) {
		TGA_ERROR(tga, TGA_WRITE_FAIL);
		return __TGA_LASTERR(tga);
	}

	return TGA_OK;
}


int
TGAWriteColorMap(TGA     *tga,
		TGAData  *data)
{
	if (!tga) return TGA_ERROR;
	if (!data) {
		TGA_ERROR(tga, TGA_ERROR);
		return __TGA_LASTERR(tga);
	}

	tlong n = TGA_CMAP_SIZE(tga);
	if (n == 0) {
		data->flags &= ~TGA_COLOR_MAP;
		return TGA_OK;
	}

	data->flags |= TGA_COLOR_MAP;

	if (TGA_CAN_SWAP(tga->hdr.map_entry) && (data->flags & TGA_RGB)) {
		__TGAbgr2rgb(data->cmap, n, tga->hdr.map_entry / 8);
		data->flags &= ~TGA_RGB;
	}

	tlong off = TGA_CMAP_OFF(tga);
	__TGASeek(tga, off, SEEK_SET);
	if (!__TGA_SUCCEEDED(tga)) {
		return __TGA_LASTERR(tga);
	}

	TGAWrite(tga, data->cmap, n, 1);
	if (!__TGA_SUCCEEDED(tga)) {
		return __TGA_LASTERR(tga);
	}

	return TGA_OK;
}


int
TGAWriteRLE(TGA   *tga, 
	    tbyte *buf)
{
	if (!tga) return TGA_ERROR;
	if (!buf) {
		TGA_ERROR(tga, TGA_ERROR);
		return __TGA_LASTERR(tga);
	}

	const tbyte sample_bytes = tga->hdr.depth / 8;
	tuint8 repetition = 0;
	tuint8 raw = 0;
	FILE *fd = tga->fd;

	tuint8 *sample_start = buf;

	for (tshort x = 1; x < tga->hdr.width; ++x) {
		if (memcmp(buf, buf + sample_bytes, sample_bytes)) {
			if (repetition) {
				tbyte packet_head = repetition | 0x80;
				TGAWrite(tga, &packet_head, 1, 1);
				TGAWrite(tga, sample_start, sample_bytes, 1);
				sample_start = buf + sample_bytes;
				repetition = 0;
				raw = 0;
			} else {
				raw += 1;
			}
		} else {
			if (raw) {
				tbyte packet_head = (raw - 1) | 0x00;
				TGAWrite(tga, &packet_head, 1, 1);
				TGAWrite(tga, sample_start, sample_bytes, raw);
				sample_start = buf;
				raw = 0;
				repetition = 1;
			} else {
				repetition += 1;
			}
		}
		if (repetition == 0x80) {
			putc(255, fd);
			fwrite(sample_start, sample_bytes, 1, fd);
			sample_start = buf + sample_bytes;
			raw = 0;
			repetition = 0;
		} else if (raw == 128) {
			putc(127, fd);
			fwrite(sample_start, sample_bytes, raw, fd);
			sample_start = buf + sample_bytes;
			raw = 0;
			repetition = 0;
		}
		buf += sample_bytes;
	}

	if (repetition > 0) {
		tbyte packet_head = repetition | 0x80;
		TGAWrite(tga, &packet_head, 1, 1);
		TGAWrite(tga, sample_start, sample_bytes, 1);
	} else {
		tbyte packet_head = raw | 0x00;
		TGAWrite(tga, &packet_head, 1, 1);
		TGAWrite(tga, sample_start, sample_bytes, raw + 1);
	}

	return __TGA_LASTERR(tga);
}


size_t
TGAWriteScanlines(TGA	  *tga, 
		  TGAData *data)
{
	if (!tga) return TGA_ERROR;

	if (!TGA_IMGTYPE_AVAILABLE(tga)) {
		return TGA_OK;
	}

	if (!data || !data->img_data) {
		TGA_ERROR(tga, TGA_ERROR);
		return __TGA_LASTERR(tga);
	}

	size_t sln_start = 0;
	size_t sln_stop = tga->hdr.height;
	size_t sln_size = TGA_SCANLINE_SIZE(tga);
	tlong off = TGA_IMG_DATA_OFF(tga) + (sln_start * sln_size);

	
	if (tga->off != off) {
		__TGASeek(tga, off, SEEK_SET);
		if (!__TGA_SUCCEEDED(tga)) {
			return __TGA_LASTERR(tga);
		}
	}

	if (TGA_CAN_SWAP(tga->hdr.depth) && (data->flags & TGA_RGB)) {
		__TGAbgr2rgb(data->img_data + (sln_start * sln_size),
			sln_size * (sln_stop - sln_start),
			tga->hdr.depth / 8);
	}

	if (data->flags & TGA_RLE_ENCODE) {
		for(size_t sln_i = sln_start; sln_i < sln_stop; ++sln_i) {
			TGAWriteRLE(tga, data->img_data + (sln_i * sln_size));
			if (!__TGA_SUCCEEDED(tga)) {
				data->flags &= ~TGA_IMAGE_DATA;
				return __TGA_LASTERR(tga);
			}
		}
		tga->hdr.img_t |= 0x8; //FIXME: do not change tga
	} else {
		TGAWrite(tga, data->img_data + (sln_start * sln_size),
			sln_size, sln_stop - sln_start);
		if (!__TGA_SUCCEEDED(tga)) {
			return __TGA_LASTERR(tga);
		}
	}

	return TGA_OK;
}
