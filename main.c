/* 
 * File:   main.c
 * Author: desmond
 *
 * Created on January 29, 2013, 8:13 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "error.h"
#include "tree.h"
#include "debug.h"
#include "path.h"

#define TEST_STRING "the quick brown fox jumps over the lazy dog."
// debug
char scratch[128];
typedef struct pos_struct pos;
// describes a character-position in the tree
struct pos_struct
{
    node *v;
    int loc;
};
// required globals that could be instance vars in an object
// the actual string we are building a tree of
char *str = TEST_STRING;
// length of str
int slen;
// end of current leaves
int e = 0;
// the root of the tree
static node *root=NULL;
// the first leaf with the longest suffix
static node *f=NULL;
// the last created internal node
static node *current=NULL;
// the last position of str[j..i-1] used in the extension algorithm
static pos last;
// value of j for next extension
static int r = 0;
/**
 * Find a child of an internal node starting with a character
 * @param v the internal node
 * @param c the char to look for
 * @return the child node
 */
node *find_child( node *v, char c )
{
    v = node_children(v);
    while ( v != NULL && str[node_start(v)] != c )
       v = node_next(v);
    return v;
}
#ifdef DEBUG
/**
 * Print out the path leading to this node from root
 * @param v the node to print the leading path of
 * @return the allocated path (caller to dispose)
 */
char *ascend( node *v )
{
    // measure string
    node *temp = v;
    int len = 0;
    while ( temp != root )
    {
        len += node_len(temp);
        temp = node_parent(temp);
    }
    char *path = calloc( len+1, 1 );
    temp = v;
    int loc = len-node_len(v);
    while ( temp != root )
    {
        memcpy( &path[loc], &str[node_start(temp)], node_len(temp) );
        temp = node_parent( temp );
        loc -= node_len( temp );
    }
    return path;
}
/**
 * Verify that all the suffix links all point to the next suffix
 * @param v the node whose suffix link is to be tested
 */
static void verify_link( node *v )
{
    if ( node_children(v) != NULL )
    {
        if ( node_parent(v) != NULL )
        {
            if ( node_link(v) == NULL )
                fprintf(stderr,"main: internal node without suffix link\n");
            else
            {
                // we are an internal node
                char *xa = ascend( v );
                char *a = ascend( node_link(v) );
                if ( strcmp(&xa[1],a)!=0 )
                    fprintf(stderr,"link from %s to %s is invalid\n",
                        xa,(a[0]==0)?(char*)"[R]":a);
                else
                    fprintf(stderr,"link: %s -> %s\n",
                        xa,(a[0]==0)?(char*)"[R]":a);
                free( xa );
                free( a );
            }
        }
    }
}
static char *substr( int start, int len )
{
    scratch[0] = 0;
    strncat( scratch, &str[start], len );
    return scratch;
}
// debug
static int match_downwards( int j, int i )
{
    node *v = root;
    int res = 1;
    int k,loc = 0;
    for ( k=j;k<=i;k++ )
    {
        if ( loc > node_end(v,e) )
        {
            v = find_child( v, str[k] );
            if ( v == NULL )
            {
                res = 0;
                break;
            }
            loc = node_start(v);
        }
        if ( str[k] != str[loc] )
        {
            res = 0;
            break;
        }
        else
            loc++;
    }
    return res;
}
static int match_backwards( pos *p, int j, int i )
{
    int k,m;
    int res = 1;
    node *v = p->v;
    for ( k=p->loc,m=i;m>=j;m-- )
    {
        if ( k >= node_start(v) )
        {
            if ( str[k] != str[m] )
            {
                res = 0;
                break;
            }
        }
        if ( k > node_start(v) )
            k--;
        else
        {
            v = node_parent(v);
            k = node_end(v,e);
        }
    }
    return res;
}
#endif
/**
 * Create a position safely
 * @return the finished pos or fail
 */
static pos *pos_create()
{
    pos *p = calloc( 1, sizeof(pos) );
    if ( p == NULL )
        fail( "couldn't create new pos\n" );
    return p;
}
/**
 * Walk down the tree from the given node following the given path
 * @param v the node to start from its children
 * @param p the path to walk down and then free
 * @return a position corresponding to end
 */
pos *walk_down( node *v, path *p )
{
    pos *q=NULL;
    int start = path_start( p );
    int len = path_len( p );
    v = find_child( v, str[start] );
    while ( len > 0 )
    {
        if ( len <= node_len(v) )
        {
            q = pos_create();
            q->loc = node_start(v)+len-1;
            q->v = v;
            break;
        }
        else
        {
            start += node_len(v);
            len -= node_len(v);
            v = find_child( v, str[start] );
        }
    }
    path_dispose( p );
    return q;
}
/**
 * Find a location of the suffix in the tree.
 * @param j the extension number counting from 0
 * @param i the current phase - 1
 * @return the position (combined node and edge-offset)
 */ 
static pos *find_beta( int j, int i )
{
    pos *p;
    if ( j>i )  // empty string
    {
        p = pos_create();
        p->loc = 0;
        p->v = root;
    }
    else if ( j==0 )    // entire string
    {
        p = pos_create();
        p->loc = i;
        p->v = f;
    }
    else // walk across tree
    {
        node *v = last.v;
        int len = last.loc-node_start(last.v)+1;
        path *q = path_create( node_start(v), len );
        v = node_parent( v );
        while ( v != root && node_link(v)==NULL )
        {
            path *r = path_create( node_start(v), node_len(v) );
            q = path_prepend( q, r );
            v = node_parent( v );
        }
        if ( v != root )
        {
            v = node_link( v );
            p = walk_down( v, q );
        }
        else
        {
            path_dispose( q );
            p = walk_down( root, path_create(j,i-j+1) );
        }
    }
    last = *p;
    return p;
}
/**
 * Does the position continue with the given character?
 * @param p a position in the tree. 
 * @param c the character to test for in the next position
 * @return 1 if it does else 0
 */
static int continues( pos *p, char c )
{
    if ( node_end(p->v,e) > p->loc )
        return str[p->loc+1] == c;
    else
        return find_child(p->v,c) != NULL;
}
/**
 * If current is set set its link to point to the next node, then clear it
 * @param v the link to point current to
 */
void update_current_link( node *v )
{
    if ( current != NULL )
    {
        node_set_link( current, v );
        verify_link( current );
        current = NULL;
    }
}
/**
 * Are we at the end of this edge?
 * @param p the position to test
 * @return 1 if it is, else 0
 */
int pos_at_edge_end( pos *p )
{
    return p->loc==node_end(p->v,e);
}
/**
 * Extend the implicit suffix tree by adding one suffix of the current prefix
 * @param j the offset into str of the suffix's start
 * @param i the offset into str at the end of the current prefix
 * @return 1 if the phase continues else 0
 */
static int extension( int j, int i )
{
    int res = 1;
    pos *p = find_beta( j, i-1 );
    // rule 1 (once a leaf always a leaf)
    if ( node_is_leaf(p->v) && pos_at_edge_end(p) )
        res = 1;
    // rule 2
    else if ( !continues(p,str[i]) )
    {
        node *leaf = node_create_leaf( i );
        if ( p->v==root || pos_at_edge_end(p) )
        {
            node_add_child( p->v, leaf );
            update_current_link( p->v );
        }
        else
        {
            node *u = node_split( p->v, p->loc );
            update_current_link( u );
            if ( i-j==1 )
            {
                node_set_link( u, root );
                verify_link(u);
            }
            else 
                current = u;
            node_add_child( u, leaf );
        }
    }
    // rule 3
    else
    {
        update_current_link( p->v );
        res = 0;
    }
    free( p );
    return res;
}
/**
 * Process the prefix of str ending in the given offset
 * @param i the inclusive end-offset of the prefix
 */
static void phase( int i )
{
    int j;
    current = NULL;
    for ( j=0;j<=i;j++ )            
        if ( !extension(j,i) )
            break;
    // update all leaf ends
    e++;
    //print_tree( root );
}
#ifdef MAIN
/**
 * Test program for implementing Ukkonen's suffix tree algorithm
 */
int main(int argc, char** argv) 
{    
    // set input string
    if ( argc==2 )
        str = argv[1];
    // create I_0 manually
    slen = strlen( str );
    root = node_create( 0, 0 );
    if ( root != NULL )
    {
        f = node_create_leaf( 0 );
        if ( f != NULL )
        {
            int i;
            node_add_child( root, f );
            for ( i=1; i<=slen; i++ )
                phase(i);
            print_tree(root);
        }
        node_dispose( root );
    }
}
#endif