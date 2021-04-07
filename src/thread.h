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



#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

extern pthread_mutex_t main_mutex;
// mutex start
extern struct msg_buff *mb_cur;
extern struct server_data sd;
// Mutex end


struct thread_data  {

  int sockfd;
  int hash;
  char **envp;

};


void thread_fail( const char *const failstr );
int lock_mutex( const int timewait_sec, char **envp );
void unlock_mutex( char **envp );
void* client( void* td );

#endif
