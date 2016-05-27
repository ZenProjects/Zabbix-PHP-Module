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

#include "GetPathDir.h"

// Retrieves the pathpath from a pathname.
//
// Returns: SUCCESS if the basepath is present and successfully copied to the p_base_path buffer
//          FAILURE_NULL_ARGUMENT if any arguments are NULL
//          FAILURE_INVALID_ARGUMENTS if either buffer size is less than 1
//          FAILURE_BUFFER_TOO_SMALL if the p_basepath buffer is too small
//          FAILURE_INVALID_PATH if the p_pathname doesn't have a path (e.g. C:, calc.exe, ?qwa)
//          FAILURE_API_CALL if there is an error from the underlying API calls
int get_base_path_from_pathname( const char*  const p_pathname,
                                 size_t             pathname_size,
                                 char* const        p_basepath,
                                 size_t             basepath_size )
{
  char*  p_end_of_path;
  size_t path_length;
  int    return_code;

  // Parameter Validation
  if( p_pathname == NULL || p_basepath == NULL ) { return FAILURE_NULL_ARGUMENT; }
  if( pathname_size < 1 || basepath_size < 1 ) { return FAILURE_INVALID_ARGUMENTS; }

  // Returns a pointer to the last occurrence of \ in p_pathname or NULL if it is not found
  p_end_of_path = strrchr( p_pathname, '\\' );
  if( p_end_of_path == NULL )
  {
    // There is no path part
    return FAILURE_INVALID_PATH;
  } 
  else 
  {
    path_length = (size_t)( p_end_of_path - p_pathname + 1 );

    // Do some sanity checks on the length
    if( path_length < 1 ) { return FAILURE_INVALID_PATH; }
    if( ( path_length + 1 ) > basepath_size ) { return FAILURE_BUFFER_TOO_SMALL; }

    // Copy the base path into the out variable
    if( strncpy( p_basepath, p_pathname, path_length ) != 0 ) { return FAILURE_API_CALL; }
    p_basepath[path_length] = '\0';
  }

  return SUCCESS;
}
