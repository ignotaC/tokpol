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



#include "thread.h"

#include "serv_func.h"

#include "serv_globals.h"

#include "const.h"
#include "base_func.h"

#include <sys/socket.h>

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void thread_fail( const char *const failstr )  {

  perror( failstr );
  pthread_exit( NULL );

}

int lock_mutex( const int timewait_sec, char **envp )  {

  struct timespec ts;
  ts.tv_sec = timewait_sec;
  ts.tv_nsec = 0;

  int lockret = pthread_mutex_timedlock( &main_mutex, &ts );
  if( lockret == ETIMEDOUT ) return -1;
  if( lockret != 0 )  {
       
    perror( strerror( lockret ) );
    errno = 0;
    restart( envp ); // probably a problem

  }

  return 0;

}

void unlock_mutex( char **envp )  {

  int unlockret = pthread_mutex_unlock( &main_mutex );
  if( unlockret )  {
       
     perror( strerror( unlockret ) );
     errno = 0;
     restart( envp ); // total fuck up here

  }


}

void* client( void* td )  {

  puts( "new thread created" );

  const int hash = ( ( struct thread_data* )td )->hash;
  const int sockfd = ( ( struct thread_data* )td )->sockfd;
  free( td );

  ( void ) hash; // REMOVE IN FUTURE COMPILE RSHUT UP HERE

  int status;
  if( ( status = pthread_detach( pthread_self() ) ) != 0 )  {

    perror( strerror( status ) );
    errno = 0;
    close( sockfd );
    return NULL;

  }

  char buff[ COMMON_BUFF_SIZE ];
  const size_t buff_size = COMMON_BUFF_SIZE;

  int cli_stat = CLI_NEW;

  for(;;)  {

    if( cli_stat == CLI_LOG )  {

      sleep( 2 );
      // check if read
      // also check if we have new message.
      // do job here

    }
    
    puts( "reading protocol" );
    if( read_proto( sockfd, buff, buff_size ) == -1 )  {

      int rst_type = 0;
      //close( sockfd );
      switch( errno )  {

        case 0:
	 rst_type = write( sockfd, buff, 1 );
	 rst_type = write( sockfd, buff, 1 );
	 if( rst_type == 0 )  
	   perror( "Client sent fin" );
	 if( rst_type < 0 ) 
           perror( "RST on connection" );
	 close( sockfd );
         pthread_exit( NULL );
	case VALUES_ERROR:
          errno = 0;
          perror( "Serious error - some vars have values that"
	          "should never happen - should be impossible" );
        default:
	  close( sockfd );
          thread_fail( "fail on thread communication with client" );

      }

    }

    puts( "parsing protocol" );
    if( parse_ptr( &cli_stat, buff, buff_size ) == -2 )  {

      close( sockfd );
      thread_fail( "Fail with protocol, client sending garbage" );

    }
    
    printf( "clistat - %d\n", cli_stat );

    switch( cli_stat )  {

      case CLI_HEL:
        send_proto( PRT_HELLO );
	break;
      case CLI_LOG:
	//check_pass();
	//send_proto( PRT_OK );
	break;
      case CLI_MSG:
	//save_msg();
	break;
      case CLI_BYE:
	puts( "Leaving" );
        shutdown( sockfd,  SHUT_RDWR );
	goto thread_end;
      default:
        close( sockfd );
	thread_fail( "Wrong client stat, should not happen" );
        
    }

    puts( "returning" );

  }

 thread_end:
  close( sockfd );
  return NULL;

}




