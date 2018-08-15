/*===========================================================================*/
/* SEED reader     |              parse_varlstr            |         utility */
/*===========================================================================*/
/*
	Name:		parse_varlstr
	Purpose:	parse a variable-length string from a blockette
	Usage:		char *parse_varlstr ();
				char *blockette_ptr;
				char *terminator_string;
				char *result;
				result = parse_varlstr (&blockette_ptr, terminator_string);
	Input:		blockette_ptr = ptr to the blockette_ptr
				terminator_string = the string which terminates the
					variable-length string
	Output:		blockette_ptr = points to beginning of next string
				result = the value of the string
	Externals:	temp_char - allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	any parsing procedure
	Calls to:	memncpy - does what strncpy is alleged to do
	Algorithm:	get string from blockette; find its length; allocate space for
				it; increment the blockette pointer; return the string to the
				caller.
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				05/12/94  CL			  Made modification to fix bug. 
										  Substituted strstr for strtok.
										  Strtok didn't work right if
										  var str looks like "Hello~~~world~"
*/

#include "rdseed.h"

char *parse_varlstr (blockette_ptr, terminator_string)
char **blockette_ptr;
char *terminator_string;
{
	char *newcharptr;							/* ptr to output string */
	int length_plus_1;							/* length of string + 1 */
	char *s;

	/* copy the input string to a temporary area */
	if ((s = strstr(*blockette_ptr, terminator_string)) == NULL)
	{
		strcpy(temp_char, "");
	}		
	else
	{
		*s = '\0';
		strcpy (temp_char, *blockette_ptr);
	}
		
	length_plus_1 = strlen (temp_char) + 1;

	/* allocate space for the string in the structure, put the string there */
	if ((newcharptr = (char *) malloc (length_plus_1)) == NULL)
		allocation_error ("parse_varlstr from any caller");
	strcpy (newcharptr, temp_char);

	/* increment the blockette pointer and return the string to the caller */
	*blockette_ptr += length_plus_1 + strlen(terminator_string) - 1;

	return (newcharptr);
}
