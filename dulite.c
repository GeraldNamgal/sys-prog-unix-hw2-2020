/*  dulite.c
 *	purpose:  
 *	action:
 *	note: referenced "ls2.c" from lecture 3     
 */
#include	<stdio.h>
#include    <stdlib.h>
#include    <stdbool.h>
#include	<dirent.h>
#include	<sys/stat.h>
#include	<string.h>
#include    <errno.h>

#define	DATE_FMT	"%b %e %H:%M"		/* text format	*/

static bool aFlag = false;
static bool kFlag = false;
static char *root;

static int disk_usage(char[]);
static void traverseDir( char **, DIR **, int * ); 
static void showInfo( char *, struct stat *, int );               

/* main(int ac, char *av[])
 * purpose: 
 * args: 
 * rets: 
 */
// TODO: test arguments more (think they work though)
int main(int ac, char *av[])
{
    bool setOption(char*);                   // fxn declaration
	if ( ac == 1 )                           // only one argument
        disk_usage( root = "." );  
    else if ( ac >= 3 ) {                    // three or more arguments
        if ( setOption( av[1] ) && setOption( av[2] ) ) {  // two valid options?       
            if ( ac == 3 ) {                 // only three args total
                disk_usage( root = "." );        
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
    else                                     // only two arguments
        if ( setOption( av[1] ) ) {          // if valid option given                   
            disk_usage( root = "." );
            ac = 1;                          // advance counter (no files given)
        }    	    
    while ( --ac )                           // if filename(s) given
        disk_usage( root = *++av ); 
}

/* setOption(char *option)
 * purpose: 
 * args: 
 * rets: 
 */
bool setOption(char *option)
{
    if ( '-' == option[0] && strlen(option) > 1 ) 
    {        
        bool foundInvalidOpt = false;
        
        for (size_t i = 1; i < strlen(option); i++)
        {
            if ( option[i] != 'a' && option[i] != 'k')
            {  
                fprintf(stderr, "dulite: invalid option -- '%c'\n", option[i]);
                if ( foundInvalidOpt == false )
                    foundInvalidOpt = true;
            }

            else if ( aFlag == false && option[i] == 'a')
                aFlag = true;            

            else if ( kFlag == false && option[i] == 'k')
                kFlag = true;                        
        }

        if (foundInvalidOpt == true)
            exit(1);        
        
        if (aFlag || kFlag)
            return true;
    }

    return false;
}

/* disk_usage( char pathname[] )
 * purpose:
 * args: 
 * rets:
 */
static int disk_usage( char pathname[] ) {
	DIR	*dir_ptr;		                                        // the directory 	 
    struct stat info;
    int sumBlocks = 0;   

    if ( lstat( pathname, &info) == -1 ) {	                     // cannot lstat	 
		fprintf(stderr, "dulite: cannot access '%s': %s\n"            
                , pathname, strerror(errno));                         // say why
        return sumBlocks;
    }
    
    if ( S_ISDIR ( info.st_mode ) ) {                            // if directory  
    
        if ( ( dir_ptr = opendir( pathname ) ) == NULL ) {     // cannot opendir
            fprintf(stderr, "dulite: cannot read directory '%s': %s\n"        
                    , pathname, strerror(errno));                     // say why
        }   
        
        else                       
            traverseDir( &pathname, &dir_ptr, &sumBlocks );
        
        sumBlocks += info.st_blocks;               // blocks of directory itself 
    }
    
    else                                                          // else a file
        sumBlocks = info.st_blocks;

    showInfo( pathname, &info, sumBlocks );           // print file/dir pathname
    
    return sumBlocks;
}

/* traverseDir( char **pathname, DIR **dir_ptr , int *sumBlocks )
 * purpose:
 * args:
 * rets:
 */
void traverseDir( char **pathname, DIR **dir_ptr , int *sumBlocks )
{
    struct dirent *direntp;		                                   // each entry     
    void saveLocation( DIR **, long * );                  // declare utility fxn
    void backToSaved( char **, DIR **, long * );          // declare utility fxn
    while ( ( direntp = readdir( *dir_ptr ) ) != NULL )          // traverse dir        
        if ( strcmp( direntp->d_name, "." ) != 0
             && strcmp( direntp->d_name, ".." ) != 0) {     // skip "." and ".."            
            char *subpath;
            subpath = malloc( strlen(*pathname) + strlen(direntp->d_name) + 2 );  
            if( subpath == NULL ) {                          // if malloc failed
                fprintf(stderr, "dulite: could not malloc: %s\n"
                        , strerror(errno));
                exit(1);
            }            
            strcat( strcpy( subpath, *pathname ), "/" );     // concat base path
            strcat( subpath, direntp->d_name );                // add sub's name           
            struct stat buff;                            // for lstat on subpath
            long loc = (long) NULL;         // to save location (before recurse)
            if ( lstat( subpath, &buff ) == -1 )       // if can't lstat subpath          
                saveLocation( dir_ptr, &loc );
            else if ( S_ISDIR( buff.st_mode ) )           // if subpath is a dir
                saveLocation( dir_ptr, &loc );
            *sumBlocks += disk_usage(subpath);       // sum sub blocks (recurse)           
            if ( loc != (long) NULL)                  // if a location was saved
                backToSaved( pathname, dir_ptr, &loc );   
            free( subpath );
        } 
    closedir(*dir_ptr); 
}

/*
 *
 */
void saveLocation( DIR **dir_ptr, long *loc )
{
    *loc = telldir( *dir_ptr );                          // save location in dir
    closedir( *dir_ptr );                                           // close dir 
}

/*
 *
 */
void backToSaved( char **pathname, DIR **dir_ptr, long *loc )
{
    if ( ( *dir_ptr = opendir( *pathname ) ) == NULL )               // open dir   
        fprintf(stderr, "dulite: cannot access '%s': %s\n"        
                , *pathname, strerror(errno));                              
    
    seekdir(*dir_ptr, *loc);                            // go to location in dir
}

/* showInfo( char *pathname, int sumBlocks )
 * args: 
 * rets:
 */
// TODO: test flag options working (think they work though)
static void showInfo( char *pathname, struct stat *info, int sumBlocks )
{
    if (aFlag == true)                          // print all files/dirs and subs
    {
        if (kFlag == true)                        // change to 1024-byte blocks?
            sumBlocks = sumBlocks / 2;
       
        printf( "%d\t", sumBlocks );
	    printf( "%s\n", pathname);
    }

    else                                    // else print just dirs or just root
    
        if ( S_ISDIR( info->st_mode ) ) 
        {    
            if (kFlag == true)                    // change to 1024-byte blocks?
                sumBlocks = sumBlocks / 2;
            
            printf( "%d\t", sumBlocks );
	        printf( "%s\n", pathname);
        }    
        else if ( strcmp( pathname, root ) == 0 )     // only print file if root
        {
            if (kFlag == true)                    // change to 1024-byte blocks?
                sumBlocks = sumBlocks / 2;
            
            printf( "%d\t", sumBlocks );
	        printf( "%s\n", pathname);
        }        
}