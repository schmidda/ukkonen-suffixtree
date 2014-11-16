/*
 * Copyright 2013 Desmond Schmidt
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
 * File:   tree.h
 * Author: desmond
 *
 * Created on February 4, 2013, 10:01 AM
 */

#ifndef TREE_H
#define	TREE_H

#ifdef	__cplusplus
extern "C" {
#endif


typedef struct node_struct node;
typedef struct node_iterator_struct node_iterator;
#define INFINITY INT_MAX
node *node_create( int start, int len );
void node_dispose( node *v );
void node_set_len( node *v, int len );
int node_kind( node *v );
int node_len( node *v );
int node_start( node *v );
void node_add_child( node *parent, node *child );
int node_is_leaf( node *v );
node *node_create_leaf( int i );
int node_add_leaf( node *parent, int i );
node *node_split( node *v, int loc );
void node_set_link( node *v, node *link );
void node_clear_next( node *v );
int node_has_next(node *v );
node *node_link( node *v );
node *node_parent( node *v );
int node_end( node *v, int max );
char node_first_char( node *v );
node *find_child( node *v, char c );
node_iterator *node_children( node *parent );
int node_iterator_has_next( node_iterator *iter );
node *node_iterator_next( node_iterator *iter );
void node_iterator_dispose( node_iterator *iter );
int node_num_children( node *v );
void node_print_children( node *v );


#ifdef	__cplusplus
}
#endif

#endif	/* TREE_H */

