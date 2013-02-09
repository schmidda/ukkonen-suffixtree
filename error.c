#include<stdio.h>
#include <stdlib.h>
#include "error.h"
/**
 * Exit the program on an error
 * @param message display this message first
 */
void fail( const char *message )
{
    fprintf(stderr,"%s",message);
    exit( 0 );
}

