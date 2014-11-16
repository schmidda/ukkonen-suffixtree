/* 
 * File:   hashtable.h
 * Author: desmond
 *
 * Created on May 21, 2013, 3:14 PM
 */

#ifndef HASHTABLE_H
#define	HASHTABLE_H

#ifdef	__cplusplus
extern "C" {
#endif
typedef struct hashtable_struct hashtable;
hashtable *hashtable_create( node *children );
void hashtable_dispose( hashtable *ht );
int hashtable_add( hashtable *ht, node *child );
int hashtable_remove( hashtable *ht, node *v, char first_char );
int hashtable_replace( hashtable *ht, node *v, node *u );
node *hashtable_get( hashtable *ht, char c );
int hashtable_size( hashtable *ht );
void hashtable_to_array( hashtable *ht, node **nodes );

#ifdef	__cplusplus
}
#endif

#endif	/* HASHTABLE_H */

