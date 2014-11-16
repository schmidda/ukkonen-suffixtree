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
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include "error.h"
#include "tree.h"
#include "hashtable.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif
#define MAX_LIST_CHILDREN 6

#if INT_MAX != 2147483647
#error "please adjust mask for int size != 4"
#endif
// necessary hacks to minimise node_struct
#define LEN_MASK 0x7FFFFFFF
#define KIND_MASK 0x80000000
#define PARENT_HASH(p) (p->len&KIND_MASK)==0x80000000
#define PARENT_LIST(p) (p->len&KIND_MASK)==0
extern long mem_usage;
struct node_iterator_struct
{
    int num_nodes;
    int position;
    node **nodes;
};
union child_list
{
    node *children;
    hashtable *ht;
};
/**
 * Represent the node structure of a tree but don't build it here
 * on 64 bit machine size is 40 bytes per node
 */
struct node_struct
{
    // index into string: edge leading INTO node
    int start;
    // length of match or INFINITY, kind in MSB
    unsigned len;
    node *next;
    union child_list c;
    // suffix link 
    node *link;
    // parent of node : needed to implement splits
    node *parent;
};
extern char *str;
/**
 * Create a node safely
 * @return the finished node or fail
 */
node *node_create( int start, int len )
{
    node *n = calloc( 1, sizeof(node) );
    mem_usage += sizeof(node);
    if ( n == NULL )
        fail( "couldn't create new node\n" );
    else
    {
        n->start = start;
        n->len = len;
    }
    return n;
}
/**
 * Create a leaf starting at a given offset 
 * @param i the offset into the string
 * @return the finished leaf or NULL on failure
 */
node *node_create_leaf( int i )
{
    node *leaf = calloc( 1, sizeof(node) );
    mem_usage += sizeof(node);
    if ( leaf != NULL )
    {
        leaf->start = i;
        leaf->len = INFINITY;
    }
    else
        fail("tree: failed to create leaf\n");
    return leaf;
}
/**
 * Dispose of a node recursively, and thus the entire tree
 * @param v the node to dispose and its children. 
 */
void node_dispose( node *v )
{
    if ( PARENT_LIST(v) )
    {
        node *child = v->c.children;
        while ( child != NULL )
        {
            node *next = child->next;
            node_dispose( child );
            child = next;
        }
    }
    else if ( PARENT_HASH(v) )
    {
        hashtable_dispose( v->c.ht );
    }
    else
        fail( "invalid node kind\n");
    free( v );
}
/**
 * Iterate through a set of nodes
 * @param parent the parent whose children should be iterated through
 * @return an iterator or NULL if it failed
 */
node_iterator *node_children( node *parent )
{
    node_iterator *iter = calloc( 1, sizeof(node_iterator) );
    if ( iter != NULL )
    {
        if ( PARENT_LIST(parent) )
        {
            int size = node_num_children( parent );
            iter->nodes = calloc( size, sizeof(node*) );
            iter->num_nodes = size;
            if ( iter->nodes != NULL )
            {
                int i=0;
                node *v = parent->c.children;
                while ( v != NULL )
                {
                    iter->nodes[i++] = v;
                    v = v->next;
                }
            }
        }
        else
        {
            int size = hashtable_size( parent->c.ht);
            iter->num_nodes = size;
            iter->nodes = calloc( size, sizeof(node*) );
            if ( iter->nodes != NULL )
                hashtable_to_array( parent->c.ht,iter->nodes );
            else
            {
                node_iterator_dispose(iter);
                iter = NULL;
            }
        }
    }
    return iter;
}
/**
 * Find out the number of children we have
 * @param v the node in question
 * @return an integer
 */
int node_num_children( node *v )
{
    int size = 0;
    if ( PARENT_LIST(v) )
    {
        node *temp = v->c.children;
        while ( temp != NULL )
        {
            size++;
            temp = temp->next;
        }
    }
    else
        size = hashtable_size( v->c.ht );
    return size;
}
/* define iterators here because they must see inside node */
/**
 * Throw away the memory occupied by the iterator (nothing else)
 * @param iter the iterator to dispose
 */
void node_iterator_dispose( node_iterator *iter )
{
    if ( iter->nodes != NULL )
        free( iter->nodes );
    free( iter );
}
/**
 * Get the next node pointed to by the iterator
 * @param iter the iterator 
 * @return the next node object
 */
node *node_iterator_next( node_iterator *iter )
{
    if ( iter->position < iter->num_nodes )
        return iter->nodes[iter->position++];
    else
        return NULL;
}
/**
 * Are there any more nodes in this iterator?
 * @param iter the iterator
 * @return 1 if it does else 0
 */
int node_iterator_has_next( node_iterator *iter )
{
    return iter->position < iter->num_nodes;
}
/**
 * Add another child to the sibling list
 * @param parent the parent
 * @param child the new sibling of parent's children
 */
static void node_append_sibling( node *parent, node *child )
{
    node *temp = parent->c.children;
    int size = 1;
    while ( temp->next != NULL )
    {
        size++;
        if ( size >= MAX_LIST_CHILDREN )
        {
            hashtable *ht = hashtable_create( parent );
            if ( ht != NULL )
            {
                parent->c.ht = ht;
                parent->len |= KIND_MASK;
                hashtable_add( parent->c.ht, child );
                return;
            }
        }
        temp = temp->next;
    }
    temp->next = child;
}
/**
 * Add a child node (can't fail)
 * @param parent the node to add the child to
 * @param child the child to add
 */
void node_add_child( node *parent, node *child )
{
    if ( PARENT_LIST(parent) )
    {
        if ( parent->c.children == NULL )
            parent->c.children = child;
        else
            node_append_sibling( parent, child );
    }
    else
    {
        hashtable_add( parent->c.ht, child );
    }
    child->parent = parent;
}
/**
 * Is this node the last one in this branch of the tree?
 * @param v the node to test
 * @return 1 if it is else 0
 */
int node_is_leaf( node *v )
{
    if ( PARENT_LIST(v) )
        return v->c.children == NULL;
    else
        return 0;
}
/**
 * Replace one child with another
 * @param v the node to be replaced
 * @param u its replacement
 */
void replace_child( node *v, node *u )
{
    if ( PARENT_LIST(v->parent) )
    {
        // isolate v and repair the list of children
        node *child = v->parent->c.children;
        node *prev = child;
        while ( child != NULL && child != v )
        {
            prev = child;
            child = child->next;
        }
        if ( child == prev )
            v->parent->c.children = u;
        else
            prev->next = u;
        u->next = child->next;
        v->next = NULL;
        //node_print_children(v->parent);
    }
    else if ( PARENT_HASH(v->parent) )
    {
        int res = hashtable_replace( v->parent->c.ht, v, u );
        if (!res)
            fail( "failed to replace node\n" );
    }
    else
        fail( "unknown node kind \n" );
}
/**
 * Split this node's edge by creating a new node in the middle. Remember 
 * to preserve the "once a leaf always a leaf" property or f will be wrong.
 * @param v the node in question
 * @param loc the place on the edge after which to split it
 * @return the new internal node
 */
node *node_split( node *v, int loc )
{
    // create front edge u leading to internal node v
    int u_len = loc-v->start+1;
    node *u = node_create( v->start, u_len );
    // now shorten the following node v
    if ( !node_is_leaf(v) )
        v->len -= u_len;
    // replace v with u in the children of v->parent
    replace_child( v, u );
    v->start = loc+1;
    // reset parents
    u->parent = v->parent;
    v->parent = u;
    // NB v is the ONLY child of u
    u->c.children = v;
    return u;
}
/**
 * Set the node's suffix link
 * @param v the node in question
 * @param link the node sv
 */
void node_set_link( node *v, node *link )
{
    v->link = link;
}
void node_set_len( node *v, int len )
{
    v->len = (v->len&KIND_MASK)+len;
}
void node_clear_next( node *v )
{
    if ( PARENT_LIST(v->parent) )
        v->next = NULL;
}
int node_has_next(node *v )
{
    return v->next !=NULL;
}
node *node_parent( node *v )
{
    return v->parent;
}
/**
 * Get the suffix link
 * @param v the node to get the link of
 * @return the node sv
 */
node *node_link( node *v )
{
    return v->link;
}
//accessors
int node_len( node *v )
{
    return LEN_MASK&v->len;
}
int node_start( node *v )
{
    return v->start;
}
int node_kind( node *v )
{
    return v->len&KIND_MASK;
}
/**
 * Find a child of an internal node starting with a character
 * @param v the internal node
 * @param c the char to look for
 * @return the child node or NULL
 */
node *find_child( node *v, char c )
{
    if ( PARENT_LIST(v) )
    {
        v = v->c.children;
        while ( v != NULL && str[v->start] != c )
           v = v->next;
        return v;
    }
    else if ( PARENT_HASH(v) )
    {
        node *u = hashtable_get( v->c.ht, c );
        return u;
    }
    else
        return NULL;
}
char node_first_char( node *v )
{
    return str[node_start(v)];
}
int node_end( node *v, int max )
{
    if ( node_len(v) == INFINITY )
        return max;
    else
        return v->start+node_len(v)-1;
}
void node_print_children( node *v )
{
    node_iterator *iter = node_children( v );
    while ( node_iterator_has_next(iter) )
    {
        node *u = node_iterator_next(iter);
        printf("%c ",str[u->start]);
    }
    node_iterator_dispose( iter );
    printf("\n");
}