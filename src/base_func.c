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



#include "base_func.h"

#include "const.h"

#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int set_nonblock( const int fd )  {

  for(;;)  {
    
    int flag = fcntl( fd, F_GETFL );
    if( flag == -1 )  {

      if( errno == EINTR )  continue;
      return -1;

    }

    flag |= O_NONBLOCK;

    if( fcntl( fd, F_SETFL, flag ) == -1 )  {

      if( errno == EINTR )  continue;
      return -1;

    }

    return 0;

  }
  
}

int readfd( const int fd,
	    void *const bytes,
	    size_t bytes_size,
	    ssize_t *const read_sum )  {


  ssize_t readret;
  bytes_size -= *read_sum;
  while( bytes_size )  {

    if( ( readret = read( fd, ( char *) bytes + *read_sum ,
		          bytes_size - ( size_t ) *read_sum  ) ) 
        == -1 )
      return -1;
    bytes_size -= ( size_t ) readret;
    *read_sum += readret;
    if( readret == 0 )  {

      errno = 0;
      return -1;

    }

  }

  return 0;

}

int read_msgpart( const int sockfd,
	          char *const buff,
	          const size_t buff_size )  {

  int counter = 0;
  ssize_t readret = 1, read_sum = 0;
  while( ( size_t )read_sum < buff_size )  {
	 
    if( ( readret = readfd( sockfd, buff, buff_size,
			    &read_sum ) ) == -1 )  {

      if( RDWR_ERR )  {

        if( counter <= COUNTER_MAX )  {

          if( nanosleep( SLEEPTIME, NULL ) == -1 )
	    if( errno != EINTR )  return -1;
	  counter++;
          continue;

	}

	return -1;

      }

    return -1;

    }

  }

  if( buff_size != ( size_t )read_sum )  {
	 
    errno = VALUES_ERROR; // lame way but will do the job
    return -1; // this should not happen

  }

  return 0;
  
}

int read_msg( const int sockfd,
	      char *const buff,
	      const size_t buff_size )  {

  uint32_t msg_size = 0;
  if( buff_size < sizeof( msg_size ) )  {

    errno = VALUES_ERROR;
    return -1;

  }
  if( read_msgpart( sockfd, ( char* )&msg_size, sizeof( msg_size ) ) == -1 )
    return -1;

  if( buff_size < msg_size )  {

    errno = VALUES_ERROR;
    return -1;

  }

  if( read_msgpart( sockfd, buff, msg_size ) == -1 )
    return -1;

  return 0;

}

int read_proto( const int sockfd,
	        char *const buff,
	        const size_t buff_size )  {
 
  uint32_t protocol;
  if( buff_size < sizeof( protocol ) )  {

    errno = VALUES_ERROR;
    return -1;

  }
  if( read_msgpart( sockfd, ( char* )&protocol, sizeof( protocol ) ) == -1 )
    return -1;

  memcpy( buff, &protocol, sizeof( protocol ) ); 
  return 0;

}

int writefd( const int fd,
	     void *const bytes,
             size_t bytes_size,
	     ssize_t *const write_sum )  {

  ssize_t writeret;
  bytes_size -= *write_sum;
  while( bytes_size )  {

    if( ( writeret = write( fd, ( char *)bytes + *write_sum,
			     bytes_size - ( size_t ) *write_sum ) ) == -1 )
      return -1;
    
    bytes_size -= ( size_t ) writeret;
    *write_sum += writeret;

  }

  return 0;

}

int mkmsg( const int protocol,
	   char *const inmsg,
	   const size_t inmsg_size,
	   char *const msg,
	   const size_t msg_size )  {

  uint32_t keep_proto = protocol;
  if( msg_size < sizeof( keep_proto ) + inmsg_size )  {

	  errno = 0;
	  return -1;

  }

  memcpy( msg, &keep_proto, sizeof( keep_proto ) );
  if( inmsg != NULL )
    memcpy( msg + sizeof( keep_proto ), inmsg, msg_size );

  return -1;

}

 
