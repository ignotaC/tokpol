/*

Copyright (c) 2020 Piotr Trzpil  p.trzpil@protonmail.com

Permission to use, copy, modify, and distribute 
this software for any purpose with or without fee
is hereby granted, provided that the above copyright
notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR
DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE
FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
OF THIS SOFTWARE.

*/

#include "use_file.h"

#include <fcntl.h>

#include <string.h>

FILE open_rfile( const char *const filename )  {

  return fopen( filename, "r" );

}


int open_wfile( const char *const filename )  {

  return fopen( filename, "w" );

}

static void rmv_newline( const char *const buff,
		         const size_t buff_size )  {

  for( size_t i = 0; i < buff_size; i++ )  {

    if( buff[i] == '\0' )  return;
    if( buff[i] == '\n' )  {

      buff[i] = '\0';
      return;

    }

  }

}

// if EOF -1 but errno set to 0
int read_line( FILE *const rfile,
	       const char *const buff,
	       const size_t buff_size )  {

  if( fgets( buff, buff_size, rfile ) == NULL )  {

    if( ! ferror( rfile ) )  errno = 0;
    return -1;

  }

  rmv_newline( buff, buff_size );
  return 0;

}

void fmemfile( char **memfile )  {

  for( size_t i = 0;; i++ )  {

    if( memfile[i] == NULL )  {

      free( memfile[i] );
      break;

    }

    free( memfile[i] );

  }

  free( memfile );

}

char **file_to_mem( const char *const fname,
	            const char *const buff,
	            const size_t buff_size )  {

  FILE *usefile = open_rfile( fname );
  if( usefile == NULL )  return NULL;

  char **memfile = malloc( sizeof *memfile );
  if( memfile == NULL ) {
	 
    fclose( usefile );
    return NULL; 

  }
  memfile[0] = NULL;
  size_t ptr_len = 1;
  while( read_line( usefile, buff, buff_size ) 
	 != -1 )  {
	  
    char **temp = realloc( memfile, ptr_len + 1 );
    if( temp == NULL )  break;
    memfile = temp;

    size_t linesize = strlen( buff ) + 1;
    char *lineptr = malloc( linesize );
    if( ptr == NULL ) break;
    memcpy( lineptr, buff, linesize ); 

    // save on old NULL
    memfile[ ptr_len - 1 ] = lineptr;
    memfile[ ptr_len ] = NULL; // new NULL
    ptr_len++;

  }

  if( errno != 0 )  {

    fmemfile( memfile );
    fclose( usefile );
    return NULL;

  }

  fclose( usefile );
  return memfile;

}





