/*  dulite.c
 *	purpose:  
 *	action:
 *	note:     
 */
#include	<stdio.h>
#include    <stdlib.h>
#include    <stdbool.h>
#include	<dirent.h>
#include	<sys/stat.h>
#include	<string.h>

#define	DATE_FMT	"%b %e %H:%M"		/* text format	*/

static bool aFlag = false;
static bool kFlag = false;

static int disk_usage(char[]);
static void showInfo( char *, int );
static bool setOption(char*);

/* main(int ac, char *av[])
 * 
 */
// TODO: test arguments more (think they work though)
int main(int ac, char *av[]) {
	if ( ac == 1 )                           // only one argument
		disk_usage( "." );   
    else if ( ac >= 3 ) {                    // three or more arguments
        if ( setOption( av[1] ) && setOption( av[2] ) ) {  // two valid options?       
            if ( ac == 3 ) {                 // only three args total
                disk_usage( "." );        
                ac = 1;                      // advance counter (no files given)
            }
            else {                           // more than three args
                ac -= 2;                     // advance counter
                av += 2;                     // advance pointer past option args                                
            }
        }
        else if ( setOption( av[1] ) ) {     // only first arg is valid option
            ac -= 1;                         // advance counter
            av += 1;                         // advance pointer past option arg        
        }
    }
    else {                                   // only two arguments
        if ( setOption( av[1] ) ) {          // if valid option given                   
            disk_usage( "." );
            ac = 1;                          // advance counter (no files given)
        }
    }	    
    while ( --ac )                           // if filename(s) given
        disk_usage( *++av ); 
}

/* setOption(char*)
 *
 */
// TODO 
static bool setOption(char* option) {
    if ( strcmp( "-a", option ) == 0 ) {
        aFlag = true;
        return true;
    }
    if ( strcmp( "-k", option ) == 0 ) {
        kFlag = true;
        return true;
    }
    if ( '-' == option[0] && strlen(option) > 1 ) {
        bool aFlagFound = false, kFlagFound = false;
        for (size_t i = 1; i < strlen(option); i++) {
            if ( option[i] != 'a' && option[i] != 'k') {
                printf("dulite: invalid option -- '%c'\n", option[i]);
                exit(1);
            }
            else if ( aFlagFound == false && option[i] == 'a') {
                aFlag = true;
                aFlagFound = true;
            }
            else if ( kFlagFound == false && option[i] == 'k') {
                kFlag = true;
                kFlagFound = true;
            }           
        }        
        if (aFlagFound || kFlagFound)
            return true;
    }
    return false;
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