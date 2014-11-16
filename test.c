#ifdef TEST
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include "tree.h"
#include "main.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif
static char *folder;
long mem_usage;
typedef struct entry_struct entry;
struct entry_struct
{
    char *file;
    int size;
    long time;
    long space;
    entry *next;
};
entry *entries=NULL;
static void append_entry( entry *e )
{
    if (entries == NULL )
        entries = e;
    else
    {
        entry *f = entries;
        while ( f->next != NULL )
            f = f->next;
        f->next = e;
    }
}
void dispose_entries()
{
    entry *e = entries;
    while ( e != NULL )
    {
        entry *next = e->next;
        free( e->file );
        free( e );
        e = next;
    }
}
/**
 * Get the length of an open file
 * @param fp an open FILE handle
 * @return file length if successful, else 0
 */
static int file_length( FILE *fp )
{
	int length = 0;
    int res = fseek( fp, 0, SEEK_END );
	if ( res == 0 )
	{
		long long_len = ftell( fp );
		if ( long_len > INT_MAX )
        {
			fprintf( stderr,"mvdfile: file too long: %ld", long_len );
            length = res = 0;
        }
		else
        {
            length = (int) long_len;
            if ( length != -1 )
                res = fseek( fp, 0, SEEK_SET );
            else
                res = 1;
        }
	}
	if ( res != 0 )
    {
		fprintf(stderr, "mvdfile: failed to read file. error %s\n",
            strerror(errno) );
        length = 0;
    }
	return length;
}
/**
 * Read a file
 * @param file the path to the file
 * @param flen update with the length of the file
 * @return NULL on failure else the allocated text content
 */
static char *read_file( char *file, int *flen )
{
    char *data = NULL;
    FILE *fp = fopen( file, "r" );
    if ( fp != NULL )
    {
        int len = file_length( fp );
        data = malloc( len+1 );
        if ( data != NULL )
        {
            int read = fread( data, 1, len, fp );
            if ( read != len )
            {
                fprintf(stderr,"failed to read %s\n",file);
                free( data );
                data = NULL;
                *flen = 0;
            }
            else
            {
                data[len] = 0;
                *flen = len;
            }
        }
        else
            fprintf(stderr,"failed to allocate file buffer\n");
        fclose( fp );
    }
    return data;
}
static char *create_path( char *dir, char *file )
{
    int len1 = strlen( dir );
    int len2 = strlen( file );
    char *path = malloc( len1+len2+2 );
    if ( path != NULL )
    {
        strcpy( path, dir );
        strcat( path, "/" );
        strcat( path, file );
    }
    return path;
}
/**
 * Read a directory
 * @return number of files found or 0 on failure
 */
static int read_dir( char *folder )
{
    int n_files = 0;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(folder)) != NULL) 
    {
        while ((ent = readdir(dir)) != NULL) 
        {
            int flen;
            if ( strcmp(ent->d_name,".")!=0&&strcmp(ent->d_name,"..")!=0 )
            {
                char *path = create_path(folder,ent->d_name);
                printf("building tree for %s\n",ent->d_name);
                char *txt = read_file( path, &flen );
                if ( txt == NULL )
                    break;
                else
                {
                    //long mem2,mem1 = get_mem_usage();
                    // precise memory usage
                    mem_usage = 0;
                    int64_t time2,time1 = epoch_time();
                    node *tree = build_tree( txt );
                    //mem2 = get_mem_usage();
                    time2 = epoch_time();
                    entry *e = calloc( 1, sizeof(entry) );
                    if ( e != NULL )
                    {
                        e->file = strdup(ent->d_name);
                        //e->space = mem2-mem1;
                        e->space = mem_usage;
                        e->time = time2-time1;
                        e->size = flen;
                        append_entry( e );
                        n_files++;
                    }
                    else
                    {
                        n_files = 0;
                        dispose_entries();
                        fprintf(stderr,"test: failed to allocate entry\n");
                        break;
                    }
                    node_dispose( tree );
                    free( txt );
                }
                if ( path != NULL )
                    free( path );
            }
        }
        closedir( dir );
    }
    else
        fprintf(stderr,"test: failed to open directory %s\n",folder);
    return n_files;
}
// arguments: folder of text files
int main( int argc, char **argv )
{
    if ( argc == 2 )
    {
        int res = read_dir( argv[1] );
        if ( res > 0 )
        {
            entry *e = entries;
            printf("%s\t\t%s\t%s\t%s\n","file","size","time (μsecs)","space");
            while ( e != NULL )
            {
                printf("%s\t%d\t%ld\t\t%ld\n",e->file,e->size,e->time,e->space);
                e = e->next;
            }
            dispose_entries();
        }
    }
    else
        fprintf(stderr,"usage: ./suffixtree <dir>\n");
}
#endif
