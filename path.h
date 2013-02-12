/*
 * Copyright [2013] [Desmond Schmidt]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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

