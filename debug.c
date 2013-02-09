#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "tree.h"
// define masks
#define BAR_VALUE 61708863
#define BAR_SPACE 536870912
/**
 * For print_tree only
 */
typedef struct link_struct link;
struct link_struct
{
    link *next;
    int *ptr;
};
// for print_tree
static link *links = NULL;
extern char *str;
extern int slen;
extern int e;
/**
 * Record a bar array for later disposal
 * @param ptr the bars array to save for later
 */
static void record_bars( int *ptr )
{
    link *l = calloc( 1, sizeof(link) );
    if ( l == NULL )
        fail("debug: failed to allocate link\n");
    if ( links == NULL )
        links = l;
    else
    {
        link *temp = links;
        while ( temp->next != NULL )
            temp = temp->next;
    }
    l->ptr = ptr;
}
/**
 * Add a new bar to the bars array
 * @param bars a NULL-terminated array of bar positions
 * @param bar a new bar position to add
 * @return the newly allocated array of bar-positions
 */
static int *add_bar( int *bars, int bar )
{
    int nbars = 0;
    // count bars
    if ( bars != NULL )
    {
        int *temp = bars;
        while ( temp[nbars] != 0 )
            nbars++;
    }
    int *new_bars = calloc( nbars+1, sizeof(int) );
    if ( new_bars != NULL )
    {
        int i;
        // save for later disposal
        record_bars( new_bars );
        for ( i=0;i<nbars;i++ )
            new_bars[i] = bars[i];
        new_bars[i] = bar;
    }
    else
        fail("debug: allocation of bars array failed\n");
    return new_bars;
}
/**
 * Clean up bars allocated during printing
 */
static void dispose_bars()
{
    /* dispose of bars */
    if ( links != NULL )
    {
        link *l = links;
        while ( l != NULL )
        {
            link *next = l->next;
            free(l->ptr);
            free( l );
            l = next;
        }
        links = NULL;
    }
}
/**
 * Print a series of bars
 * @param bars the bars themselves
 * @param skip_last print a space instead of a vertical bar for the last bar
 */
static void print_bars( int *bars, int skip_last )
{
    if ( bars != NULL )
    {
        int j,i = 0;
        while ( bars[i] != 0 )
        {
            int bar_value = bars[i] & BAR_VALUE;
            for ( j=0;j<bar_value;j++ )
                printf(" ");
            if ( !skip_last || bars[i+1]!=0 )
            {
                if ( bars[i] & BAR_SPACE )
                    printf(" ");
                else
                    printf("|");
            }
            i++;
        }
    }
}
/**
 * Print a series of bars and then a CR
 */
static void print_bar_line( int *bars )
{
    print_bars( bars, 0 );
    printf("\n");
}
/**
 * Print the label of the node
 * @param v the node to print
 * @return number of characters written
 */
static int print_label( node *v )
{
    int i,start,end;
    end = node_end(v,e);
    start = node_start(v);
    for ( i=node_start(v);i<=end;i++ )
    {
        if ( str[i]==0 )
            printf("$");
        else
            printf("%c",str[i]);
    }
    // print terminal star for unfinished leaves
    if ( node_children(v)==NULL && e < slen )
        printf("*");
    return end-start+1;
}
/**
 * Set the mode of the final bar in a series
 * @param bars the bar array to modify
 * @param mode the new mode forthe last bar
 */
static void set_last_bar( int *bars, int mode )
{
    int  i;
    if ( bars != NULL )
    {
        i = 0;
        while ( bars[i]!= 0 )
            i++;
    }
    if ( i > 0 )
        bars[i-1] |= mode;
}
/**
 * Print a tree out left to right by preorder traversal
 * @param v the node to start printing from
 * @param bars array of vertical bars to draw on each line
 */
static void print_node( node *v, int *bars )
{
    int depth;
    node *u = v;
    while ( u != NULL )
    {
        if ( u != v )
        {
            print_bar_line( bars );
            print_bars( bars, 1 );
        }
        if ( node_next(u)==NULL )
            set_last_bar(bars,BAR_SPACE);
        printf("-");
        if ( node_len(u) == 0 )
            depth = printf("[R]") + 1;
        else
            depth = print_label(u);
        if ( node_is_leaf(u) )
            printf("\n");
        else
            print_node( node_children(u), add_bar(bars,depth) );
        u = node_next( u );
    }
}
/**
 * Print the entire tree recursively
 * @param v the node to start from (root)
 */
void print_tree( node *v )
{
    print_node( v, NULL );
    dispose_bars();
}
#ifdef DEBUG_TREE
//           012345678901234567890123456789012345678901234567
char *str = "bananaapplepearorangeguavapeachapricotlemongrape";
int slen = 0;
int main( int argc, char **argv )
{
    slen = strlen(str);
    node *root = node_create( 0, 0 );
    // b == "banana"
    node *b = node_create(0,6);
    // children of root: banana, apple, pear
    node_add_child( root, b );
    node_add_child( root, node_create(6,5) );
    node *p = node_create(11,4);
    node_add_child( root, p );
    // lemon and grape are children of pear
    node_add_child( p, node_create(38,5) );
    node_add_child( p, node_create(43,5) );
    // orange, guava are children of banana
    node_add_child( b, node_create(15,6) );
    node *c = node_create(21,5);
    node_add_child( b, c );
    // peach and apricot are children of guava
    node_add_child( c, node_create(26,5) );
    node_add_child( c, node_create(31,7) );
    print_tree( root );
}
#endif