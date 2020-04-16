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



#ifndef CONST_H
#define CONST_H

#define COMMON_BUFF_SIZE 4096
#define SAFE_BUFF_SIZE ( COMMON_BUFF_SIZE - 1 )
#define SLEEPTIME &( struct timespec ){ 0, 500000000 }
#define COUNTER_MAX 40
#define RDWR_ERR ( errno == EWOULDBLOCK ) || \
	         ( errno == EAGAIN ) || \
		 ( errno == EINTR )
#define VALUES_ERROR 254
#define MAX_LEN_NAME 10
#define MAX_LEN_PASS 30

enum {

  PRT_HELLO,
  PRT_LOGIN,
  PRT_MSG,
  PRT_OK,
  PRT_BYE

};

#endif
