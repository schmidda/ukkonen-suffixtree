/* 
 * File:   path.h
 * Author: desmond
 *
 * Created on February 12, 2013, 5:56 AM
 */

#ifndef PATH_H
#define	PATH_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct path_struct path;
path *path_create( int start, int len );
path *path_prepend( path *p, path *q );
void path_dispose( path *p );
int path_start( path *p );
int path_len( path *p );
char path_first( path *p, char *str );
path *path_next( path *p );



#ifdef	__cplusplus
}
#endif

#endif	/* PATH_H */

