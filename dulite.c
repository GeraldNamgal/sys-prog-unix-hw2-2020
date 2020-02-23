/*  dulite.c
 *	purpose:  
 *	action:
 *	note:     
 */
#include	<stdio.h>
#include    <stdlib.h>
#include	<dirent.h>
#include	<sys/stat.h>
#include	<string.h>

#define	DATE_FMT	"%b %e %H:%M"		/* text format	*/

int disk_usage(char[]);
void showInfo( char *, int );

int main(int ac, char *av[])
{
	if ( ac == 1 )  
		disk_usage( "." );
	else
		while ( --ac ) {
            // TODO: check that [1] or [1] and [2] are -a and/or -k           
			disk_usage( *++av );
        }
}

/* disk_usage( char pathname[] )
 * args: 
 * rets:
 * note: referenced
 *       https://stackoverflow.com/questions/276827/string-padding-in-c	
 */
int disk_usage( char pathname[] ) {
	DIR	*dir_ptr;		                    /* the directory */
	struct dirent *direntp;		            /* each entry	 */
    struct stat info;
    int sumBlocks = 0;
    if ( lstat( pathname, &info) == -1 )	/* cannot stat	 */
		perror( pathname );			        /* say why	     */    
    if ( S_ISDIR ( info.st_mode ) ) {       /* if directory  */
        if ( ( dir_ptr = opendir( pathname ) ) == NULL )
            fprintf(stderr,"dulite: cannot access '%s'\n", pathname);
        else {
            sumBlocks += info.st_blocks;               // add directory's blocks
            while ( ( direntp = readdir( dir_ptr ) ) != NULL )   
                if ( direntp->d_name[0] != '.' ) {   // skip '.' beginning paths
                    char *path;
                    path = malloc( ( strlen(pathname) + strlen(direntp->d_name)
                                        + 2 ) * sizeof( char ) );
                    strcat( strcpy( path, pathname ), "/" ); // concat base path
                    strcat( path, direntp->d_name );  // add file/directory name           
                    sumBlocks += disk_usage(path);  // add sub files/dirs blocks                  
                    free( path );
                }            
            closedir(dir_ptr);            
        }
    }
    else                                    /* else a file */
        sumBlocks = info.st_blocks;
    showInfo( pathname, sumBlocks );
    return sumBlocks;
}

/* showInfo( char *pathname, int sumBlocks )
 * args: 
 * rets:
 */
void showInfo( char *pathname, int sumBlocks )
{
	printf( "%-7d ", sumBlocks );
	printf( "%s\n", pathname);
}