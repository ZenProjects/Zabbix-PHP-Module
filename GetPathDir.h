/*
* Get Base Path of a file
* Copyright (C) 2007-2016 Mathieu CARBONNEAUX
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef GETPATHDIR_H 
#define GETPATHDIR_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define FAILURE_NULL_ARGUMENT 1
#define FAILURE_INVALID_ARGUMENTS 2
#define FAILURE_INVALID_PATH 3
#define FAILURE_BUFFER_TOO_SMALL 4
#define FAILURE_API_CALL 5
#define SUCCESS 0
extern int get_base_path_from_pathname( const char*  const p_pathname,
					size_t             pathname_size,
					char* const        p_basepath,
					size_t             basepath_size );


#endif

