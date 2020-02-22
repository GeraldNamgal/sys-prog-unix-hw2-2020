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
static int count = 0;

void disk_usage(char[]);
void dostat(char *);
void show_file_info( char *);

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

// referenced https://stackoverflow.com/questions/1036625/differentiate-between-a-unix-directory-and-file-in-c-and-c
void disk_usage( char dirname[] )
/*
 *	list files in directory called dirname
 */
{
	DIR		*dir_ptr;		/* the directory */
	struct dirent	*direntp;		/* each entry	 */

	if ( ( dir_ptr = opendir( dirname ) ) == NULL )
		fprintf(stderr,"dulite.c error: cannot open %s\n", dirname);
	else
	{
		while ( ( direntp = readdir( dir_ptr ) ) != NULL )
			if ( direntp->d_name[0] != '.' ) {
                char path[1024];
                strcat( strcpy( path, dirname ), "/" );
				dostat( strcat( path, direntp->d_name ) );
                if (direntp->d_type == DT_DIR) {         // if directory
                    disk_usage(path);
                }
            }
		closedir(dir_ptr);
	}
}

void dostat( char *filename )
{
	if ( stat(filename, &info) == -1 )		/* cannot stat	 */
		perror( filename );			/* say why	 */
	else					/* else show info	 */
		show_file_info( filename );
}

void show_file_info( char *filename )
/*
 * display the info about 'filename'.  The info is stored in struct info
 */
{
    printf("%ld ", info.st_blocks);
	printf( "%s\n"  , filename );
}