/*  dulite.c
 *	purpose:  
 *	action:
 *	note:     
 */
#include	<stdio.h>
#include	<sys/types.h>
#include	<dirent.h>
#include	<sys/stat.h>
#include	<string.h>
#include	<time.h>

#define	DATE_FMT	"%b %e %H:%M"		/* text format	*/

static struct stat info;

void disk_usage(char[]);
void dostat(char *);
void show_file_info( char *);
char *mode_to_letters( int , char [] );
char *uid_to_name( uid_t );
char *gid_to_name( gid_t );
char *fmt_time(time_t, char *);

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
                if (direntp->d_type == DT_DIR) {         
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

/*
 * utility functions
 */

#define	MAXDATELEN	100

char *
fmt_time( time_t timeval , char *fmt )
/*
 * formats time for human consumption.
 * Uses localtime to convert the timeval into a struct of elements
 * (see localtime(3)) and uses strftime to format the data
 */
{
	static char	result[MAXDATELEN];

	struct tm *tp = localtime(&timeval);		/* convert time	*/
	strftime(result, MAXDATELEN, fmt, tp);		/* format it	*/
	return result;
}

/*
 * This function takes a mode value and a char array
 * and puts into the char array the file type and the 
 * nine letters that correspond to the bits in mode.
 * NOTE: It does not code setuid, setgid, and sticky
 * codes
 */
char *mode_to_letters( int mode, char str[] )
{
    strcpy( str, "----------" );           /* default=no perms */

    if ( S_ISDIR(mode) )  str[0] = 'd';    /* directory?       */
    if ( S_ISCHR(mode) )  str[0] = 'c';    /* char devices     */
    if ( S_ISBLK(mode) )  str[0] = 'b';    /* block device     */

    if ( mode & S_IRUSR ) str[1] = 'r';    /* 3 bits for user  */
    if ( mode & S_IWUSR ) str[2] = 'w';
    if ( mode & S_IXUSR ) str[3] = 'x';

    if ( mode & S_IRGRP ) str[4] = 'r';    /* 3 bits for group */
    if ( mode & S_IWGRP ) str[5] = 'w';
    if ( mode & S_IXGRP ) str[6] = 'x';

    if ( mode & S_IROTH ) str[7] = 'r';    /* 3 bits for other */
    if ( mode & S_IWOTH ) str[8] = 'w';
    if ( mode & S_IXOTH ) str[9] = 'x';
    return str;
}

#include	<pwd.h>

char *uid_to_name( uid_t uid )
/* 
 *	returns pointer to username associated with uid, uses getpw()
 */	
{
	struct	passwd *pw_ptr;
	static  char numstr[10];

	if ( ( pw_ptr = getpwuid( uid ) ) == NULL ){
		sprintf(numstr,"%d", uid);
		return numstr;
	}
	else
		return pw_ptr->pw_name ;
}

#include	<grp.h>

char *gid_to_name( gid_t gid )
/*
 *	returns pointer to group number gid. used getgrgid(3)
 */
{
	struct group *grp_ptr;
	static  char numstr[10];

	if ( ( grp_ptr = getgrgid(gid) ) == NULL ){
		sprintf(numstr,"%d", gid);
		return numstr;
	}
	else
		return grp_ptr->gr_name;
}
