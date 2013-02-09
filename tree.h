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
#define INFINITY INT_MAX
node *node_create( int start, int len );
void node_dispose( node *v );
int node_len( node *v );
node *node_children( node *v );
node *node_next( node *v );
int node_start( node *v );
void node_add_child( node *parent, node *child );
int node_is_leaf( node *v );
node *node_create_leaf( int i );
int node_add_leaf( node *parent, int i );
node *node_split( node *v, int loc );
void node_set_link( node *v, node *link );
node *node_link( node *v );
node *node_parent( node *v );
int node_end( node *v, int max );

#ifdef	__cplusplus
}
#endif

#endif	/* TREE_H */

