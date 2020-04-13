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

#ifndef SERV_FUNC_H
#define SERV_FUNC_H

#include "serv_globals.h"

#include <stddef.h>

#define DEFAULT_PORT 15151

#define PORT_DATA 0
#define PORT_HINT "PORT"

#define HIST_DATA 1
#define HIST_HINT "HISTORY"

#define ULOG_DATA 2
#define UPAS_DATA 3
#define USER_HINT "USER"

extern struct msg_buff mb[ MB_SIZE ];
extern struct msg_buff *mb_cur;

extern const char *program_path;
extern char *saved_msg;

int set_cloexec( const int fd );
void restart( char **envp );

void init_msg_buff( void );
int load_saved_msg( void );
int save_msg( char **envp );

int parse_ptr( int *const cli_stat_ptr,
	       char *const buff,
	       const size_t buff_size );

int mklistenfd( int port );

int turn_daemon( void );

struct server_data  {

  unsigned short port;
  char *history_file;
  char *( *logpass )[2];
  size_t logpass_size;

};

int load_sd( char *const*const servconf,
             struct server_data *sd );


#endif
