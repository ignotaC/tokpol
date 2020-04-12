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

#include "serv_conf.h"

void* ( *cmd_type( const char *const cmd ) ) ( void* )  {

  char **name = serv_conf_names;
  for( size_t i = 0, name[i] != NULL, i++ )  {

    if( ! strcmp( cmd, name[i] ) )
      return conf_function[i]

  }

  return -1;

}

void* port_set( void *data )  {

  char *port_str = data;
  char *endpoint;
  long int *port = malloc( sizeof *a );
  if( port == NULL )  return NULL;

  errno = 0;
  *port = strtol( port_str, endpoint, 10 );
  if( port_str[0] == '\0' )  goto fail;
  if( endpoint != '\0' )  goto fail;
  if( errno )  goto fail;

  return port;

 fail:
  free( port );
  return NULL;

}


