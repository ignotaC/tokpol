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



#include "serv_func.h"
#include "serv_globals.h"
#include "base_func.h"

#include "const.h"

#include <netinet/in.h>

#include <sys/socket.h>

#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int set_cloexec( const int fd )  {

  for(;;)  {
    
    int flag = fcntl( fd, F_GETFD );
    if( flag == -1 )  {

      if( errno == EINTR )  continue;
      return -1;

    }

    flag |= FD_CLOEXEC;

    if( fcntl( fd, F_SETFD, flag ) == -1 )  {

      if( errno == EINTR )  continue;
      return -1;

    }

    return 0;

  }
  
}

void restart( char **envp )  {

  char *name = "new program";
  char *envp_pass[] = { name, NULL };

  perror( "Restarting" );
  errno = 0;
  execve( program_path, envp_pass, envp );
  exit( EXIT_FAILURE );
  
}

void init_msg_buff( void )  {

  memset( mb[0].msg, 0, MSG_SIZE );
  mb[0].next = &mb[ MB_SIZE - 1 ]; 

  for( size_t i = 1; i < MB_SIZE; i++ )  {

    memset( mb[i].msg, 0, MSG_SIZE );
    mb[i].next = &mb[ i - 1 ];

  }

}


int load_saved_msg( void )  {

  FILE *chatfile = fopen( saved_msg, "r" );
  if( chatfile == NULL )  {

    if( errno == ENOENT )  {

      perror( "Saved chat file does not exist." );
      return 0;

    }

    return -1;

  }

  while( fgets( mb_cur->msg, MSG_SIZE - 1, chatfile ) != NULL )
    mb_cur = mb_cur->next;

  if( ferror( chatfile ) )  return -1;
  fclose( chatfile );
  return 0;

}


// MUTEX MUST BE LOCKED ON THIS!!!!!
int update_msg( const int sockfd,
		char *buff,
	       	const size_t buff_size )  {

  struct msg_buff *mbp = mb_cur;
  do {  
	  
    mbp = mbp->next;
    if( mbp->msg[0] != '\0' )  {

      size_t msg_size = strnlen( mbp->msg, MSG_SIZE );
      memcpy( buff, mbp->msg, msg_size );
      if( write_proto( sockfd, PRT_MSG ) == -1 )
        return -1;
      if( write_msg( sockfd, buff, buff_size,
	             msg_size ) == -1 )  return -1;

    }


  } while( mbp != mb_cur );

  return 0;

}



void save_msg( char **envp )  {

  FILE *filechat = NULL;

  for(;;)  {

    filechat = fopen( saved_msg, "a" );
    if( filechat == NULL )  {

      if( errno == EINTR )  continue;
      restart( envp ); 

    }

    break;

  }

  if( fprintf( filechat, "%s",  mb_cur->msg ) < 0 )
    perror( "Fail on saving line from file buff" );

  fclose( filechat );

}
             

int parse_ptr( int *const cli_stat_ptr,
	       char *const buff,
	       const size_t buff_size )  {

  uint8_t protocol = 0;
  if( buff_size < sizeof( protocol ) )  {

    errno = 0;
    perror( "No protocol number at all" );
    return -1;

  }

  memcpy( &protocol, buff, sizeof( protocol ) );

  if( protocol == PRT_BYE )  {
    
    puts( "Client saying bye bye" );
    *cli_stat_ptr = CLI_BYE;
    return 0;

  }

  switch( *cli_stat_ptr )  {

    case CLI_NEW:
      if( protocol != PRT_HELLO )  {

	errno = 0;
        perror( "Wrong protocol on new connection" );
	return -1;

      }
      *cli_stat_ptr = CLI_HEL;
      return 0;

    case CLI_HEL:
      switch( protocol )  {

        case PRT_LOGIN:
          *cli_stat_ptr = CLI_LOG;
	  return 0;
	default:
	  errno = 0;
	  perror( "Wrong protocol on hello client" );
	  return -1;

      }

    default:
      errno = 0;
      perror( "Unknown protocol" );
      return -1;

  }

}

int mklistenfd( int port )  {

  struct sockaddr_in sin;
  memset( &sin, 0, sizeof( sin ) );
  sin.sin_port = htons( ( unsigned short ) port );
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;

  int listenfd = socket( AF_INET, SOCK_STREAM, 0 );
  if( listenfd < 0 )  return -1;

  int enable = 1;
  if( setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR,
      &enable, ( socklen_t )sizeof( enable ) ) < 0 )
    return -1;

  if( bind( listenfd, ( struct sockaddr* ) &sin, sizeof( sin ) ) < 0 )
    return -1;
  
  if( listen( listenfd, 10 ) < 0 ) return -1;

  return listenfd;

}

int turn_daemon( void )  {

  // fork and exit with parent
  pid_t pid = 0;
  if( ( pid = fork() ) == -1 ) return -1;
  if( pid )  _exit( EXIT_SUCCESS );

  // session lider
  if( setsid() < 0 )  return -1;

  //get rid of signal that would kill after closing term
  struct sigaction sa;
  memset( &sa, 0, sizeof( sa ) );
  sa.sa_flags = 0;
  //  FIX HERE SIGMASK
  sa.sa_handler = SIG_IGN;
  if( sigaction( SIGHUP, &sa, NULL ) < 0 )  return -1;

  // refork and exit with parent so demon can't get tern
  if( ( pid = fork() ) < 0 ) return -1;
  if( pid )  _exit( EXIT_SUCCESS );

  // lock up all in/out files
  fclose( stdin );
  fclose( stdout );
  fclose( stderr );

  // noise will go to null
  open( "/dev/null", O_RDONLY );
  open( "/dev/null", O_WRONLY );
  open( "log", O_RDWR | O_APPEND | O_CREAT );

  return 0;
  
}

int load_sd( char *const*const servconf,
             struct server_data *sd )  {

  const size_t temp_buff_size = 8192;
  char temp_buff[ temp_buff_size ];
  void *tmp_mem;

  // init sd
  sd->logpass = NULL;
  sd->history_file = NULL;
  sd->port = DEFAULT_PORT;
  sd->logpass_size = 0;
  
  char *const* namepos = ( char *const* ) servconf;
  int status = -1;
  for( ;*namepos != NULL; namepos++ )  {
 
    if( status == -1 )  {

      if( sscanf( *namepos, "%s", temp_buff ) != 1 )
        continue;

      if( ! strcmp( temp_buff, USER_HINT ) )  {

        status = ULOG_DATA;
	continue;

      }

      if( ! strcmp( temp_buff, HIST_HINT ) )  {

        status = HIST_DATA;
	continue;

      }


      if( ! strcmp( temp_buff, PORT_HINT ) )  {

        status = PORT_DATA;
	continue;

      }

      fprintf( stderr,
               "Wrong server variable, ignoring:\n" );
      fprintf( stderr, "%s\n", temp_buff );

    }
    
    if( sscanf( *namepos, "%s", \
        temp_buff ) != 1 )  {

      perror( *namepos );
      return -1;

    }


    switch( status )  {

      case PORT_DATA:
       errno = 0;
       sd->port = ( unsigned short )strtol( \
          temp_buff, NULL, 10 );
       if( errno != 0 )  return -1;
       status = -1;
       break;

      case HIST_DATA:
        ;
	size_t hist_name_size = \
	  strnlen( temp_buff, temp_buff_size );
        tmp_mem = malloc( hist_name_size + 1 );
	if( tmp_mem == NULL )  return -1;
	strncpy( tmp_mem, temp_buff, hist_name_size );
	( ( char* )tmp_mem )[ hist_name_size ] = '\0';
	sd->history_file = tmp_mem;
	status = -1;
	break;

       case ULOG_DATA:
        sd->logpass_size++;
	tmp_mem = realloc( sd->logpass, \
	  sd->logpass_size * sizeof *( sd->logpass ) );
	if( tmp_mem == NULL )  return -1;
	sd->logpass = tmp_mem;
	( sd->logpass )[ sd->logpass_size- 1 ][0] = \
	  NULL;
	( sd->logpass )[ sd->logpass_size - 1 ][1] = \
	  NULL;

	size_t user_name_size = \
	  strnlen( temp_buff, temp_buff_size );
	tmp_mem = malloc( user_name_size + 1 );
	if( tmp_mem == NULL )  return -1;
	strncpy( tmp_mem, temp_buff, user_name_size );
	( ( char* )tmp_mem )[ user_name_size ] = '\0';
	sd->logpass[ sd->logpass_size - 1 ][0] = \
	   tmp_mem;
	status = UPAS_DATA;
	break;

       case UPAS_DATA:
	;  // must be expression
	size_t pass_name_size = \
	  strnlen( temp_buff, temp_buff_size );
	tmp_mem = malloc( pass_name_size + 1 );
	if( tmp_mem == NULL )  return -1;
	strncpy( tmp_mem, temp_buff, pass_name_size );
	( ( char* )tmp_mem )[ pass_name_size ] = '\0';
	sd->logpass[ sd->logpass_size - 1 ][1] = \
	  tmp_mem;
	status = -1;
	break;

       default:
	return -1;

    }

  }

  return 0;

}
