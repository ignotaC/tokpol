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

#ifndef USE_FILE_H
#define USE_FILE_H

#define FPERM ( S_IRUSR S_IWUSR )

#include <stddef.h>
#include <stdio.h>

FILE *open_rfile( const char *const filename );
FILE *open_wfile( const char *const filename );

int read_line( FILE *const rfile,
	       char *const buff,
	       const size_t buff_size );
void fmemfile( char **memfile );
char **file_to_mem( const char *const fname,
	            char *const buff,
	            const size_t buff_size );
void memfile_free( char **memfile );
 
#endif
