#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define MAX_ALT_FILES 10;

FILE *alt_file_array[] =  {0, 0, 0, 0, 0, 
			   0, 0, 0, 0, 0};

static int now_file_ptr = 0, next_file_ptr = 0;

/* ---------------------------------------------------------------------- */
int alt_file_open(fname)
char *fname;

{

	if ((alt_file_array[next_file_ptr] = fopen(fname, "r")) == NULL)
		return -1;

	next_file_ptr++;
	
	return 1;

}

/* ---------------------------------------------------------------------- */
int close_alt_files()

{
	int i;

	for (i = 0; i < next_file_ptr; i++)
		fclose(alt_file_array[i]);

	now_file_ptr = 0, next_file_ptr = 0;

	memset((char *)alt_file_array, 0, sizeof(alt_file_array));

}

/* ---------------------------------------------------------------------- */
int open_alt_files(fnames)
char *fnames;

{
	int error;

	if (strchr(fnames, ':'))
	{
		char *p;
		
		p = strtok(fnames, ":");
		
		if (alt_file_open(p) == -1)	
		{
			fprintf(stderr, "Warning : open alt files : Unable to open %s\n", p);                               
            perror("open_alt_files");
		}

		while(p = strtok(NULL, ":"))
			if (alt_file_open(p) == -1) 
			{ 
            	fprintf(stderr, "Warning : open alt files : Unable to open %s\n", p);                                      
            	perror("open_alt_files"); 
        	}

	}
	else
		if (alt_file_open(fnames) == -1)
		{
			fprintf(stderr, "Warning : open alt files : Unable to open %s\n", fnames);
			perror("open_alt_files");
		}



}

/* ---------------------------------------------------------------------- */
FILE *get_alt_file()

{
	/* filter out the 0, 1, 2 - stdin, out & err */
	/* if (alt_file_array[now_file_ptr] < 3)
		return (FILE *) NULL;
	*/

	return(alt_file_array[now_file_ptr++]);
}

