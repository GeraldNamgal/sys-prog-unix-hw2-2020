/*  dulite.c
 *	purpose:  
 *	action:
 *	note:     
 */
#include	<stdio.h>
#include	<dirent.h>
#include	<sys/stat.h>
#include	<string.h>

#define	DATE_FMT	"%b %e %H:%M"		/* text format	*/

static struct stat info;

int disk_usage(char[]);

int main(int ac, char *av[])
{
	if ( ac == 1 ) {        
		disk_usage( "." );
    }
	else
		while ( --ac ){           
			printf("%s:\n", *++av );
			disk_usage( *av );
		}
}

/* disk_usage( char dirname[] )
 *	
 */
int disk_usage( char dirname[] )
{
	DIR		*dir_ptr;		                /* the directory */
	struct dirent	*direntp;		        /* each entry	 */
    int sumBlocks = 0;
    if ( lstat( dirname, &info) == -1 )		/* cannot stat	 */
		perror( dirname );			        /* say why	     */    
    if ( S_ISDIR ( info.st_mode ) ) {       /* if directory  */
        if ( ( dir_ptr = opendir( dirname ) ) == NULL )
            fprintf(stderr,"dulite.c error: cannot open %s\n", dirname);
        else {
            sumBlocks += info.st_blocks;               // add directory's blocks
            while ( ( direntp = readdir( dir_ptr ) ) != NULL )   
                if ( direntp->d_name[0] != '.' ) {        // skip '.' file names
                    char path[1024];
                    strcat( strcpy( path, dirname ), "/" );  // concat base path
                    strcat( path, direntp->d_name );  // add file/directory name           
                    sumBlocks += disk_usage(path);  // add sub files/dirs blocks                  
                }            
            closedir(dir_ptr);            
        }
    }
    else                                    /* if file */
        sumBlocks = info.st_blocks;
    printf( "%d ", sumBlocks );
	printf( "%s\n"  , dirname );
    return sumBlocks;
}