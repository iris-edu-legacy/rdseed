/*===========================================================================*/
/* SEED reader     |              parse_nchar              |         utility */
/*===========================================================================*/
/*
	Name:		parse_nchar
	Purpose:	parse a fixed-length string from a blockette
	Usage:		char *parse_nchar ();
				char *blockette_ptr;
				int number_of_bytes;
				char *result;
				result = parse_nchar (&blockette_ptr, number_of_bytes);
	Input:		blockette_ptr = ptr to the blockette_ptr
				number_of_bytes = number of bytes from blockette to process
	Output:		blockette_ptr = points to beginning of next string
				result = the value of the string
	Externals:	temp_char - allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	any parsing procedure
	Calls to:	memncpy - does what strncpy is alleged to do
	Algorithm:	get number_of_bytes bytes from the blockette; strip off trailing
				blanks; allocate space for the stripped string; copy the string
				to the resultant; increment the blockette pointer; return the
				string to the caller.
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				02/17/94  CL added better loop terminating condition,
							i >= 0 in for loop
*/

#include "rdseed.h"

char *parse_nchar (blockette_ptr, number_of_bytes)
char **blockette_ptr;
int number_of_bytes;
{
	char *newcharptr;							/* ptr to output string */
	int i;										/* counter */

	/* copy the input string to a temporary area, strip trailing blanks */
	memncpy (temp_char, *blockette_ptr, number_of_bytes);
	for (i = strlen (temp_char) - 1; temp_char[i] == ' '&& i >= 0; temp_char[i--] = '\0');

	/* allocate space for the string in the structure, put the string there */
	if ((newcharptr = (char *) malloc (strlen (temp_char) + 1)) == NULL)
		allocation_error ("parse_nchar from any caller");
	strcpy (newcharptr, temp_char);

	/* increment the blockette pointer and return the string to the caller */
	*blockette_ptr += number_of_bytes;
	return (newcharptr);
}
