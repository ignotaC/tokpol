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



#ifndef SERV_GLOBALS_H
#define SERV_GLOBALS_H

#include <stddef.h>

#define MSG_SIZE 8192
#define MB_SIZE 1024
#define PING_CLOCK 180
#define PING_MAX 300

struct msg_buff {

  struct msg_buff *next;
  char msg[ MSG_SIZE ];

};

enum {

  CLI_NEW,
  CLI_HEL,
  CLI_CHK,
  CLI_LOG,
  CLI_MSG,
  CLI_BYE

};

#endif
