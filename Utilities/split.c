/* ------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rdseed.h"


/* ------------------------------------------------------------------- */
char *get_fld();
/* ------------------------------------------------------------------- */

/* split() will parse a string into components, based on the fs parameter
 * The fs param is very specific, space is space, tab is tab, etc
 * Always call fuse() when done
 */

int split(s, parts, fs)
char *s;        	/* thing to split */
char ***parts;   	/* put parts into */
char fs;       		/* field separator */
 
{
 
        int n, i, finished;
        char *wrkstr;
	char *ch_ptr;

	i = 0;

	finished = FALSE;

	wrkstr = malloc(strlen(s) + 1);

 
        /* check for in bounds */
        if (wrkstr == NULL)
        {
                fprintf(stderr, "split(): too big to split - out of memory - string= %s\n", s);
                return 0;
        }
        
        /* determine how many "parts" will be split */
	if (strlen(s) == 0)
		return 0;

	strcpy(wrkstr, s);

	n = 0;	/* always at least one part unless null string */

	n = count_chars(wrkstr, fs) + 1;

        *parts = (char **)malloc(n * sizeof(char *));

        ch_ptr = get_fld(wrkstr, fs);

	do 
        {
                (*parts)[i] = (char *)malloc(strlen(ch_ptr) + 1);

		strcpy((*parts)[i], ch_ptr);

		i++; /* count the field */

		if (i == n)
		{
			finished = TRUE;
			continue;
		}


		if (i > n)
		{
			fprintf(stderr, "split(), got \"splitting\" headache, not tonight dear!\n");
			break;
		}

                ch_ptr = get_fld(NULL, fs);

        } while (!finished);

	/* make check for i < n, not sure why this would happen */
	if (i < n)
	{

		*parts = (char **)realloc(*parts, i * sizeof(char *));
	} 

	free(wrkstr);

        return i;
}

/* ----------------------------------------------------------------------- */

int count_chars(str, ch)
char *str;
char ch;

{
        int count = 0;

        while (*str)
        {
                if (*str == ch)
                        count++;

                str++;
        }

        return count;

}

/* ------------------------------------------------------------------------ */

char *get_fld(s, fs)
char *s, fs;

{
	static char *sav_fld_ptr;
	char *fld_ptr;
	int n;


	if (s != NULL)
	{
		fld_ptr = s;
	}
	else
		fld_ptr = sav_fld_ptr;

	if (*fld_ptr == '\0')
		return fld_ptr;


	n = 0;

	while ((fld_ptr[n] != fs) && (fld_ptr[n] != '\0'))
		n++;	

	if (fld_ptr[n] != '\0')
	{
		/* move past fs */
		sav_fld_ptr = fld_ptr + n + 1;

	}
	else
		sav_fld_ptr = fld_ptr + n;

	fld_ptr[n] = 0;

	return fld_ptr;

}

 
int fuse(parts, count)
char ***parts;
int count;

{
	int i;

	for (i = 0; i < count; i++)
		free((*parts)[i]);

	if (count > 0)
		free(*parts);

	*parts = 0;

	return 0;

}

/* ------------------------------------------------------------------- */

/* ------------------------------------------------------------------- */

