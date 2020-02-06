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



#ifndef BASE_FUNC_H
#define BASE_FUNC_H

#include <unistd.h>
#include <stddef.h>

void fail( const char *const estr );

int set_nonblock( const int fd );

int readfd( const int fd,
	    void *const bytes,
	    size_t bytes_size,
	    ssize_t *const read_sum );

int writefd( const int fd,
	     void *const bytes,
             size_t bytes_size,
	     ssize_t *const write_sum );

int read_msgpart( const int sockfd,
	          char *const buff,
	          const size_t buff_size );

int read_msg( const int sockfd,
	      char *const buff,
	      const size_t buff_size );

int read_proto( const int sockfd,
	        char *const buff,
	        const size_t buff_size );

int mkmsg( const int protocol,
	   char *const inmsg,
	   const size_t inmsg_size,
	   char *const msg,
	   const size_t msg_size );

#endif
