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

#define	DATE_FMT	"%b %e %H:%M"		/* text format	*/

static bool aFlag = false;
static bool kFlag = false;
static char *root;

static int disk_usage(char[]);
static void showInfo( char *, struct stat *, int );
static bool setOption(char*);

/* main(int ac, char *av[])
 * 
 */
// TODO: test arguments more (think they work though)
int main(int ac, char *av[]) {
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
    else {                                   // only two arguments
        if ( setOption( av[1] ) ) {          // if valid option given                   
            disk_usage( root = "." );
            ac = 1;                          // advance counter (no files given)
        }
    }	    
    while ( --ac ) {                         // if filename(s) given
        disk_usage( root = *++av );
    } 
}

/* setOption(char*)
 *
 */
static bool setOption(char* option)
{
    if ( '-' == option[0] && strlen(option) > 1 ) 
    {        
        for (size_t i = 1; i < strlen(option); i++)
        {
            if ( option[i] != 'a' && option[i] != 'k')
            {
                printf("dulite: invalid option -- '%c'\n", option[i]);
                exit(1);
            }

            else if ( aFlag == false && option[i] == 'a')
                aFlag = true;            

            else if ( kFlag == false && option[i] == 'k')
                kFlag = true;                        
        }        
        
        if (aFlag || kFlag)
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
	DIR	*dir_ptr;		                                        // the directory 
	struct dirent *direntp;		                                   // each entry	 
    struct stat info;
    int sumBlocks = 0;
    if ( lstat( pathname, &info) == -1 )	                      // cannot stat	 
		perror( pathname );			                                  // say why	         
    //if ( S_ISDIR ( info.st_mode ) ) {                            // if directory  
        // TODO: proper error message
        if ( ( dir_ptr = opendir( pathname ) ) == NULL )
            perror("dulite: cannot access '%s': ");
        else {
            sumBlocks += info.st_blocks;               // get directory's blocks
            while ( ( direntp = readdir( dir_ptr ) ) != NULL )   
                if ( direntp->d_name[0] != '.' ) {   // skip '.' beginning paths
                    char *path;
                    path = malloc( ( strlen(pathname) + strlen(direntp->d_name)
                                        + 2 ) * sizeof( char ) );
                    strcat( strcpy( path, pathname ), "/" ); // concat base path
                    strcat( path, direntp->d_name );  // add file/directory name           
                    
                    long loc = telldir(dir_ptr);
                    closedir(dir_ptr);
                    
                    sumBlocks += disk_usage(path);  // sum sub files/dirs blocks                  
                    
                    dir_ptr = opendir(pathname);
                    seekdir(dir_ptr, loc);
                    
                    free( path );
                }            
            closedir(dir_ptr);            
        }
    //}
    //else                                                          // else a file
        sumBlocks = info.st_blocks;
    //showInfo( pathname, &info, sumBlocks );               // print file/dir path
    return sumBlocks;
}

/* showInfo( char *pathname, int sumBlocks )
 * args: 
 * rets:
 */
// TODO: test flag options working (think they work though)
static void showInfo( char *pathname, struct stat *info, int sumBlocks ) {
	if (aFlag == true) {    // print all files and directories including nesteds
        
        if (kFlag == true)                        // change to 1024-byte blocks?
            sumBlocks = sumBlocks / 2;
       
        printf( "%-7d ", sumBlocks );
	    printf( "%s\n", pathname);
    }

    // TODO: du prints a file's info if it was passed in (not a directory)
    //       and if a directory is passed in, it doesn't print files (for no a
    //       flag) <-- create a "command line arg was a directory/file" flag?

    else                         // else print just directories or just the root
        if ( S_ISDIR( info->st_mode ) ) {
            
            if (kFlag == true)                    // change to 1024-byte blocks?
                sumBlocks = sumBlocks / 2;
            
            printf( "%-7d ", sumBlocks );
	        printf( "%s\n", pathname);
        }
        else if ( strcmp( pathname, root ) == 0 ) {   // only print file if root
            if (kFlag == true)                    // change to 1024-byte blocks?
                sumBlocks = sumBlocks / 2;
            
            printf( "%-7d ", sumBlocks );
	        printf( "%s\n", pathname);
        }        
}