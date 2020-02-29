// Gerald Arocena
// CSCI E-28, Spring 2020
// 2-29-2020
// project 2

/* dulite.c
 * Implements some of the functions of the Unix "du" command. Reports the number
 * of 512-byte blocks used by one or more files and/or directories. If no file
 * or directory name command line arguments are given, then the working
 * directory's name/path is passed in. If a directory name is passed in, then
 * the amount of blocks for that directory and all of its sub directories and
 * files are reported. If a file is passed in, then just that file's blocks are
 * reported. dulite accepts an option, "-a", that prints all of the sub files'
 * and directories' blocks for a given input (no "-a" option excludes files from
 * being reported unless it's the input given). dulite accepts another option,
 * "-k", that prints the number of 1024-byte blocks used (no "-k" option prints
 * the number of 512-byte blocks)
 * 
 * Usage: ./dulite [-ak] [FILE or DIRECTORY]... 
 */
#include	<stdio.h>
#include    <stdlib.h>
#include    <stdbool.h>
#include	<dirent.h>
#include	<sys/stat.h>
#include	<string.h>
#include    <errno.h>

static bool aFlag = false;                   // for if "-a" flag input
static bool kFlag = false;                   // for if "-k" flag input
static char *root = ".";                     // input file/directory 

static int disk_usage(char[]);
static void traverseDir( char **, DIR **, int * ); 
static void showInfo( char *, struct stat *, int );
static void saveLocation( char **, DIR **, long * );    
static void backToSaved( char **, DIR **, long * );                  

/* main(int ac, char *av[])
 * purpose: handles command line arguments (will call setOption() if a flag was
 *          given) and then calls disk_usage() to report the block numbers
 * args: the number of command line arguments and the arguments themselves
 * rets: exit status code
 */
int main(int ac, char *av[])
{
    bool setOption(char*);                   // fxn declaration
	
    if ( ac == 1 )                           // no argument given
        disk_usage( root );  
    else if ( ac >= 3 ) {                    // two or more arguments
        if ( setOption( av[1] ) && setOption( av[2] ) ) {  // two valid options?       
            if ( ac == 3 ) {                 // only two args total
                disk_usage( root );        
                ac = 1;                      // advance counter (no files given)
            }
            else {                           // more than two args given
                ac -= 2;                     // advance counter
                av += 2;                     // advance pointer past option args
            }
        }
        else if ( setOption( av[1] ) ) {     // only first arg is valid option
            ac -= 1;                         // advance counter
            av += 1;                         // advance pointer past option arg        
        }
    }
    else                                     // only one argument
        if ( setOption( av[1] ) ) {          // if arg is valid option                    
            disk_usage( root );
            ac = 1;                          // advance counter (no files given)
        }    	    
    while ( --ac )                           // if filename(s) given
        disk_usage( root = *++av ); 
}

/* setOption(char *option)
 * purpose: checks a given option and sets it if valid
 * args: the option to check and possibly set
 * rets: true if the option was set and false otherwise
 */
bool setOption(char *option)
{
    if ( '-' == option[0] && strlen(option) > 1 ) 
    {        
        bool foundInvalidOpt = false;
        
        for (size_t i = 1; i < strlen(option); i++) {
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

        if (foundInvalidOpt == true) {
            fprintf(stderr, "usage: ./dulite [-ak] [FILE OR DIRECTORY]...\n");
            exit(1);        
        }

        if (aFlag || kFlag)
            return true;
    }
    return false;
}

/* disk_usage( char pathname[] )
 * purpose: takes a directory or file pathname and reports the block space used
 *          by it. If the pathname is a directory then it opens up the directory
 *          and adds up the block space used by each item in it. Any item in the
 *          directory is also a directory, then it is also processed. Items and
 *          their block space are printed to stdout with showInfo()
 * args: the pathname of the file/directory to reports block spaces for
 * rets: the block space for the file or directory
 */
static int disk_usage( char pathname[] ) {
	DIR	*dir_ptr;		                        // the directory 	 
    struct stat info;
    int sumBlocks = 0;   

    if ( lstat( pathname, &info) == -1 ) {	       // cannot lstat	 
		fprintf(stderr, "dulite: cannot access '%s': %s\n"            
                , pathname, strerror(errno));      // say why
        return sumBlocks;
    }
    
    if ( S_ISDIR ( info.st_mode ) ) {              // if directory  
    
        if ( ( dir_ptr = opendir( pathname ) ) == NULL ) {  // cannot opendir
            fprintf(stderr, "dulite: cannot read directory '%s': %s\n"        
                    , pathname, strerror(errno));  // say why
        }   
        
        else                       
            traverseDir( &pathname, &dir_ptr, &sumBlocks );
        
        sumBlocks += info.st_blocks;               // blocks of directory itself 
    }
    
    else                                           // else a file
        sumBlocks = info.st_blocks;

    showInfo( pathname, &info, sumBlocks );        // print file/dir pathname
    
    return sumBlocks;
}

/* traverseDir( char **pathname, DIR **dir_ptr , int *sumBlocks )
 * purpose: utility function for disk_usage(). Iterates over an open directory's
 *          items to gather block space information and then closes the
 *          directory when finished
 * args: the directory's pathname, a DIR pointer to its open stream, and the sum
 *       of 
 * rets: none
 * note: referenced --
 *       http://man7.org/linux/man-pages/man3/telldir.3.html
 */
void traverseDir( char **pathname, DIR **dir_ptr , int *sumBlocks ) {
    struct dirent *direntp;		                     // each entry   
    errno = 0;                       // distinguishes error from no more entries
    while ( ( direntp = readdir( *dir_ptr ) ) != NULL )  // traverse dir        
        if ( strcmp( direntp->d_name, "." ) != 0
              && strcmp( direntp->d_name, ".." ) != 0) {  // skip "." and ".."            
            char *subpath;
            subpath = malloc( strlen(*pathname) + strlen(direntp->d_name) + 2 );  
            if( subpath == NULL ) {                  // if malloc failed
                fprintf(stderr, "dulite: malloc error: %s\n", strerror(errno));
                exit(1);
            }            
            strcat( strcpy( subpath, *pathname ), "/" );  // concat base path
            strcat( subpath, direntp->d_name );      // add subpath's name           
            struct stat buff;                        // for lstat on subpath
            long loc = -1;                  // to save location (before recurse)
            if ( lstat( subpath, &buff ) == -1 )     // if can't lstat subpath          
                saveLocation( pathname, dir_ptr, &loc );
            else if ( S_ISDIR( buff.st_mode ) )      // if subpath is a dir
                saveLocation( pathname, dir_ptr, &loc );
            *sumBlocks += disk_usage(subpath);       // sum sub blocks (recurse)           
            if ( loc != -1 )                         // if a location was saved
                backToSaved( pathname, dir_ptr, &loc );   
            free( subpath );
        } 
    if ( closedir(*dir_ptr) == -1 ) {
        fprintf(stderr, "dulite: can't close '%s': %s\n"
                , *pathname, strerror(errno));
    }
}

/* saveLocation( char **pathname, DIR **dir_ptr, long *loc )
 * purpose: utility function for traverseDir(). Saves the location in a
 *          directory
 * args: the directory's pathname, a DIR pointer to its open stream, and the
 *       variable to save the location in
 * rets: none
 * note: referenced --
 *       https://pubs.opengroup.org/onlinepubs/007908799/xsh/telldir.html
 */
static void saveLocation( char **pathname, DIR **dir_ptr, long *loc )
{
    if ( ( *loc = telldir( *dir_ptr ) ) == -1 )          // save location in dir
    {
        fprintf(stderr, "dulite: could not save location in '%s': %s\n"        
                , *pathname, strerror(errno));
    }

    else if ( closedir(*dir_ptr) == -1 )                            // close dir
    {
        fprintf(stderr, "dulite: cannot close '%s': %s\n"
                , *pathname, strerror(errno));
    }                     
}

/* backToSaved( char **pathname, DIR **dir_ptr, long *loc )
 * purpose: moves the read position of an open directory to the location given
 * args: the pathname of the directory, a DIR pointer to its open stream, and
 *       the location to move to
 * rets: none
 * note: seekdir() has no return value defined. Also, referenced --
 *       https://pubs.opengroup.org/onlinepubs/007908775/xsh/seekdir.html
 */
static void backToSaved( char **pathname, DIR **dir_ptr, long *loc )
{
    if ( ( *dir_ptr = opendir( *pathname ) ) == NULL )               // open dir   
        fprintf(stderr, "dulite: cannot access '%s': %s\n"        
                , *pathname, strerror(errno));                              
    
    else
        seekdir(*dir_ptr, *loc);                        // go to location in dir
}

/* showInfo( char *pathname, int sumBlocks )
 * purpose: prints the pathname of a file or directory and the number of blocks
 *          used by it 
 * args: the pathname of the file or directory, a pointer to its struct stat,
 *       and the number blocks it uses 
 * rets: none
 */
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
