#include <stdlib.h>
#include "path.h"
// for storing gamma (walking across the tree)
struct path_struct
{
    int start;
    int len;
};
/**
 * Create a path
 * @param start the start index into str
 * @param len the length of this path 
 * @return the complete path
 */
path *path_create( int start, int len )
{
    path *p = calloc( 1, sizeof(path) );
    if ( p == NULL )
        fail("failed to create path\n");
    else
    {
        p->start = start;
        p->len = len;
    }
    return p;
}
/**
 * Add one path on the end of another
 * @param p the current path
 * @param pre the desired head
 * @return the new head
 */
path *path_prepend( path *p, path *pre )
{
    p->start -= pre->len;
}
/**
 * Get the first char in the path
 * @param p the path in question
 * @param str the string it refers to
 * @return the first char
 */
char path_first( path *p, char *str )
{
    return str[p->start];
}
/**
 * Dispose of a path cleanly
 * @param p the path to dispose
 */
void path_dispose( path *p )
{
    free( p );
}
/**
 * Access the start field
 * @param p the path in question
 * @return the path start index in str
 */
int path_start( path *p )
{
    return p->start;
}
/**
 * Access the start field
 * @param p the path in question
 * @return the path length
 */
int path_len( path *p )
{
    return p->len;
}

