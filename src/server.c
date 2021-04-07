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

#include "debug.h"
#include "serv_func.h"
#include "serv_globals.h"
#include "thread.h"

#include "const.h"
#include "base_func.h"
#include "use_file.h"

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 8192

// Made for God glory.
/////////////////////

const char *program_path = NULL;

/// MUTEX
pthread_mutex_t main_mutex;
struct msg_buff mb[ MB_SIZE ];
// ^ we acces it via mb_cur
struct msg_buff *mb_cur = mb;
struct server_data sd;
/// MUTEX END

char *saved_msg = NULL;

int main( int argc, char **argv, char **envp  )  {

  char buff[ BUFF_SIZE ];

  if( argc != 2 )
    fail( "Missing server config file." );
  char **server_conf = file_to_mem( argv[1], buff, BUFF_SIZE );
  if( server_conf == NULL )
    fail( "Couold not load server_conf file." );
  puts( "Configuration file loaded." );
  if( load_sd( server_conf, &sd ) == -1 )
    fail( "Failed on loading server data." );
  puts( "Server variables loaded." );
  // get rid of loaded to memory file.
  memfile_free( server_conf );
  saved_msg = sd.history_file;

  int estat = pthread_mutex_init( &main_mutex, NULL );
  if( estat )  fail( strerror( estat ) );
  puts( "Mutex ready." );

  init_msg_buff();
  puts( "Message buff set up." );
  if( load_saved_msg() < 0 )
    fail( "Chat loading fail." );
  puts( "Saved chat loaded." );

  struct sigaction sa;
  memset( &sa, 0, sizeof( sa ) );
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  if( sigemptyset( &sa.sa_mask ) == -1 )
    fail( "Could not set empty mask" );
  if( sigaction( SIGPIPE, &sa, NULL ) == -1 )
    fail( "Could not ignore sigpipe." );
  puts( "Signal handlers set" );

  printf( "Setting up on %d port\n", sd.port );

  int listenfd = mklistenfd( sd.port );
  if( listenfd < 0 )  fail( "Failed on creating listen socket" );
  puts( "Listen socket created and ready to accept connections." );

  #ifndef DPROG

  puts( "Turning to daemon." ); 
   if( turn_daemon() < 0 )
     fail( "Could not turn to daemon" );

  #endif // DPROG

  struct thread_data *td;
  struct sockaddr_in cliaddr;
  socklen_t clilen = sizeof( cliaddr );
  pthread_t pt;
  int sockfd;
  for( int save_tic = 0;; save_tic++)  {

    // One thead per 1 sec on comnection is enought.
    // This is not busy server.
    // Protects from jokers to some point.
    sleep( 1 ); 
    if( ( sockfd = accept( listenfd,
			   ( struct sockaddr* ) &cliaddr,
			   &clilen ) ) < 0 )  {

      switch( errno )  {

      case EINTR:
      case ECONNABORTED:
	continue;
      default:
	restart( envp );
	
      }
      
    }

    if( set_cloexec( sockfd ) < 0 )
      restart( envp );

    int hash = 0;
    memcpy( &hash, &( cliaddr.sin_addr ),
	    sizeof( hash ) < sizeof( cliaddr.sin_addr ) ?
	    sizeof( hash ) : sizeof( cliaddr.sin_addr ) );

    hash ^= 734473;
    hash ^= 17237766;

    
    if( ( td = malloc( sizeof( td ) ) ) == NULL )
      restart( envp );
    td->hash = hash;
    td->sockfd = sockfd;
    td->envp = envp;

    estat = pthread_create( &pt, NULL, client, td );
    if( estat )  restart( envp );

    
  }
  
}


