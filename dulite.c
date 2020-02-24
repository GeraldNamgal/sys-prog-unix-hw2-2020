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

static bool aFlag = false;
static bool kFlag = false;

static int disk_usage(char[]);
static void showInfo( char *, int );
static void setOption(char*);

/* main(int ac, char *av[])
 * 
 */
// TODO: test arguments more (think they work though)
int main(int ac, char *av[]) {
	if ( ac == 1 )                          // if only one argument
		disk_usage( "." );   
    else if ( ac >= 3 ) {                   // if three or more arguments
        if ( strcmp( av[1], "-a" ) == 0 || strcmp( av[1], "-k" ) == 0 ) {
            if ( strcmp( av[2], "-a" ) == 0 || strcmp( av[2], "-k" ) == 0 ) {              
                  setOption( av[1] ); setOption( av[2] ); 
                  if ( ac == 3 ) {          // if only three args total
                      disk_usage( "." );        
                      ac = 1;               // advance counter (no files given)
                  }
                  else
                      {ac -= 2; av += 2;}   // advance pointer past option args                
            }
            else {                          // if only first arg is option
                setOption( av[1] );
                ac -= 1; av += 1;           // advance pointer past option arg
            }
        }
    }
    else {                                  // if there are only two arguments
        if ( strcmp( av[1], "-a" ) == 0 || strcmp( av[1], "-k" ) == 0 ) {
            setOption( av[1] );             
            disk_usage( "." );
            ac = 1;                         // advance counter (no files given)
        }
    }	    
    while ( --ac )                          // if filename(s) given
        disk_usage( *++av ); 
}

/* setOption(char*)
 *
 */
// TODO 
static void setOption(char* option)
{
    if ( strcmp( "-a", option ) == 0 ) {
        aFlag = true;
    }

    if ( strcmp( "-k", option ) == 0 ) {
        kFlag = true;
    }
}

/* disk_usage( char pathname[] )
 * args: 
 * rets:
 * note: referenced
 *       https://stackoverflow.com/questions/276827/string-padding-in-c	
 */
static int disk_usage( char pathname[] ) {
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
// TODO
static void showInfo( char *pathname, int sumBlocks )
{
	printf( "%-7d ", sumBlocks );
	printf( "%s\n", pathname);
}