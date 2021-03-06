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
#include "thread.h"

#include "serv_func.h"
#include "serv_globals.h"

#include "const.h"
#include "base_func.h"

#include <sys/socket.h>

#include <unistd.h>

#include <stdio.h>
#include <stdint.h>
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

  const int hash = ( ( struct thread_data* )td )->hash;
  const int sockfd = ( ( struct thread_data* )td )->sockfd;
  char **const envp = ( ( struct thread_data* )td )->envp;
  free( td );

  if( set_nonblock( sockfd ) == -1 )  {

    close( sockfd );
    thread_fail( "Could not set socket to nonblocking" );

  }

  int status;
  if( ( status = pthread_detach( pthread_self() ) ) != 0 )  {

    perror( strerror( status ) );
    errno = 0;
    close( sockfd );
    return NULL;

  }

  char buff[ COMMON_BUFF_SIZE ];
  const size_t buff_size = COMMON_BUFF_SIZE;
  size_t data_size = 0;
  char cliname[ MAX_LEN_NAME + 1 ];
  memset( cliname, 0, MAX_LEN_NAME + 1 );

  int cli_stat = CLI_NEW;
  int ping_time = 0;
  uint8_t msgstat_prt;
  struct msg_buff *thread_pos = NULL;

  for(;;)  {
  
    if( cli_stat == CLI_MSG )  {
 
      DPUTS(  "Start of cli stat msg" );

      for( int i = 0; lock_mutex( 1, envp ) != 0;
          i++ )  {

        if( i > 10 )  restart( envp );

      }
      DPUTS( "mutex locked" );
      
      if( thread_pos != mb_cur )  {

	DPUTS( "thread pos check up" );
        do {

          thread_pos = thread_pos->next;
          if( write_proto( sockfd, PRT_MSG ) == -1 ) {

	    close( sockfd );
            unlock_mutex( envp );
	    thread_fail( "Fail on writing to socket" );

	  }
	  size_t msg_len = strnlen( thread_pos->msg,
              MSG_SIZE );
	  if( write_msg( sockfd, thread_pos->msg,
	      MSG_SIZE, msg_len ) == -1 )  {

	    close( sockfd );
            unlock_mutex( envp );
	    thread_fail( "Fail on writing to socket" );

	  }

	  DPUTS( "updating" );

	}  while( thread_pos != mb_cur );

      }

      unlock_mutex( envp );
      DPUTS( "mutex unlocked" );

      if( ping_time > PING_MAX )  {

	DPUTS( "ping time too long - terminating" );
        close( sockfd );
	thread_fail( "Client timed out" );

      }
      if( ping_time > PING_CLOCK )  {
        
	DPUTS( "ping demand" );
        if( write_proto( sockfd, PRT_PING ) == -1 )  {

          close( sockfd );
          thread_fail( "Could not write to socket" );

        }

        sleep( 1 );

      }
      
      ping_time++;
      if( nanosleep( SLEEPTIME, NULL ) == -1 )  {

        close( sockfd );
	thread_fail( "Failed on nanosleep" );

      }

      DPUTS( "sleeping and incrementing ping" );

      if( read( sockfd, &msgstat_prt,
          sizeof msgstat_prt ) == -1 )  {

        if( RDWR_ERR )  continue;
	close( sockfd );
	thread_fail( "Failed on probing socket" );

      }

      DPUTS( "Message protocol" );
      
      // no matter what is happening - client is alive
      ping_time = 0;
      if( msgstat_prt == PRT_PING )  {

	DPUTS( "ping" );
        continue;

      }

      if( msgstat_prt == PRT_MSG )  {


        DPUTS( "Msg proto" );
        if( read_msg( sockfd, buff,
            buff_size, &data_size ) == -1 )  {

          close( sockfd );
          thread_fail( "Failed on reading msg" );

        }

        for( int i = 0;	lock_mutex( 1, envp ) != 0;
           i++ )  {

          if( i > 10 )  restart( envp );

        }

	mb_cur = mb_cur->next;
	time_t cur_time = time( NULL );
        sprintf( mb_cur->msg, "%s %d %s", cliname, hash,
		 ctime( &cur_time ) );
	save_msg( envp );

	mb_cur = mb_cur->next;
	sprintf( mb_cur->msg, buff, data_size );
	mb_cur->msg[ data_size ] = '\0';
	save_msg( envp );

	unlock_mutex( envp );

	DPUTS( "finished" );

      }

      if( msgstat_prt == PRT_BYE )  {

	DPUTS( "bye" );
        shutdown( sockfd, SHUT_RDWR );
	sleep( 1 );
	close( sockfd );
	return NULL;

      }

      DPUTS( "endofloop" );
      continue;

    }
  
    if( read_proto( sockfd, buff, buff_size ) == -1 )  {

      int rst_type = 0;
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


    if( parse_ptr( &cli_stat, buff, buff_size ) == -2 )  {

      close( sockfd );
      thread_fail( "Fail with protocol, client sending garbage" );

    }
    

    switch( cli_stat )  {

      case CLI_HEL:
       if( write_proto( sockfd, PRT_HELLO ) == -1 )  {

         close( sockfd );
         thread_fail( "Could not write to socket" );

       }
       break;
      case CLI_LOG:
       if( read_msg( sockfd, buff,
         buff_size, &data_size ) == -1 )  {

         close( sockfd );
         thread_fail( "Failed on reading log msg" );

       }

       // nick can not be longer than max len chars
       if( data_size > MAX_LEN_NAME )  {

         close( sockfd );
         thread_fail( "Client name length too long" );

       }

       // we check size safty in msg_size
       buff[ data_size ] = 0;
       strncpy( cliname, buff, MAX_LEN_NAME + 1 );
	
       if( read_msg( sockfd, buff,
           buff_size, &data_size ) == -1 )  {

         // send proto bye wait few sec
         close( sockfd );
         thread_fail( "Failed on reading log msg" );

       }

       // Pass can not be longer max len chars
       if( data_size > MAX_LEN_PASS )  {

         close( sockfd );
         thread_fail( "Client pass too long" );

       }
       buff[ data_size ] = 0;

       for( int i = 0;	lock_mutex( 1, envp ) != 0;
           i++ )  {

         if( i > 10 )  restart( envp );

       }

       int cliloged = 0;
       for( size_t i = 0; i < sd.logpass_size; i++ ) {

         if( strncmp( cliname, sd.logpass[i][0],
	     MAX_LEN_NAME ) )  continue;
	 if( strncmp( buff, sd.logpass[i][1],
             MAX_LEN_PASS ) )  continue;
	 cliloged = 1;
	 break;

       } 

       if( cliloged )  {

         if( write_proto( sockfd, PRT_OK ) == -1 )  {

	   unlock_mutex( envp );
           close( sockfd );
           thread_fail( "Could not write to socket" );

         }
         if( update_msg( sockfd, buff,
			 buff_size ) == -1 )  {

	   unlock_mutex( envp );
	   close( sockfd );
	   thread_fail( "Failed on sending msglog" );

	 }

	 thread_pos = mb_cur;
         unlock_mutex( envp );
	 cli_stat = CLI_MSG;
         break;

       }  // else send bye

       unlock_mutex( envp );
       if( write_proto( sockfd, PRT_BYE ) == -1 )  {

         close( sockfd );
         thread_fail( "Could not write to socket" );

       }
      case CLI_BYE:
       DPUTS( "Shuting down" );
       shutdown( sockfd,  SHUT_RDWR );
       goto thread_end;
      default:
       close( sockfd );
       thread_fail( "Wrong client stat, should not happen" );
        
    }

  }

  thread_end:
   close( sockfd );
   return NULL;

}



