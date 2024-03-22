/*
 *  tga.c
 *
 *  Copyright (C) 2001-2002  Matthias Brueckner <matbrc@gmx.de>
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

const char *
tga_error_strings[] =
{
	"Success",
	"Error",
	"Out of memory",
	"Failed to open file",
	"Seek failed",
	"Read failed",
	"Write failed",
	"Unknown sub-format",
};


tuint8
TGA_handle_set_error(TGA *tga, tuint8 code)
{
	if (tga) {
		if (tga->error) {
			tga->error(tga, code);
		}
		tga->last = code;
	}
	return code;
}


TGA *
TGAOpen(const char *file, 
	const char *mode)
{
 	TGA *tga = (TGA*) malloc(sizeof(TGA));
	if (!tga) {
		TGA_ERROR(tga, TGA_OOM);
		return NULL;
	}

	FILE* fd = NULL;
	fopen_s(&fd, file, mode);
	if (!fd) {
		TGA_ERROR(tga, TGA_OPEN_FAIL);
		free(tga);
		return NULL;
	}

	tga->fd = fd;
	tga->off = 0;
	memset(&tga->hdr, 0, sizeof(TGAHeader));
	tga->last = TGA_OK;
	tga->error = (TGAErrorProc) 0;
	return tga;
}


TGA *
TGAOpenFd(FILE *fd)
{
	TGA *tga = (TGA*)malloc(sizeof(TGA));
	if (!tga) {
		TGA_ERROR(tga, TGA_OOM);
		return NULL;
	}

	if (!fd) {
		TGA_ERROR(tga, TGA_OPEN_FAIL);
		free(tga);
		return NULL;
	}

	long offset = ftell(fd);
	if (offset == -1) {
		TGA_ERROR(tga, TGA_OPEN_FAIL);
		free(tga);
		return NULL;
	}

	tga->fd = fd;
	tga->off = offset;
	memset(&tga->hdr, 0, sizeof(TGAHeader));
	tga->last = TGA_OK;
	tga->error = (TGAErrorProc) 0;
	return tga;
}


void
TGAClose(TGA *tga)
{
	if (tga) {
		fclose(tga->fd);
		free(tga);
	}
}


void
TGAClearError(TGA *tga)
{
	if (tga) {
		tga->last = TGA_OK;
		clearerr(tga->fd);
	}
}


const char*
TGAStrError(TGA *tga)
{
	return TGAStrErrorCode(tga->last);
}


const char*
TGAStrErrorCode(tuint8 code)
{
	if (code >= TGA_ERRORS_NB) code = TGA_ERROR;
	return tga_error_strings[code];
}


tlong
__TGASeek(TGA  *tga, 
	  tlong off, 
	  int   whence)
{
	fseek(tga->fd, off, whence);
	long offset = ftell(tga->fd);
	if (offset == -1) {
		TGA_ERROR(tga, TGA_SEEK_FAIL);
	}
	tga->off = offset;
	return tga->off;
}


void
__TGAbgr2rgb(tbyte  *data, 
	     size_t  size, 
	     size_t  stride)
{
	for (size_t i = 0; i < size; i += stride) {
		tbyte tmp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = tmp;
	}
}
