/*===========================================================================*/
/* SEED reader     |              parse_int                |         utility */
/*===========================================================================*/
/*
	Name:		parse_int
	Purpose:	parse an integer field from a blockette
	Usage:		int parse_int ();
				char *blockette_ptr;
				int number_of_bytes;
				int result;
				result = parse_int (&blockette_ptr, number_of_bytes);
	Input:		blockette_ptr = ptr to the blockette_ptr
				number_of_bytes = number of bytes from blockette to process
	Output:		blockette_ptr = points to beginning of next string
				result = the value of the integer field
	Externals:	temp_char - allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	any parsing procedure
	Calls to:	memncpy - does what strncpy is alleged to do
	Algorithm:	get number_of_bytes bytes from the blockette; increment the
				blockette pointer; return the integer value of the string 
				to the caller
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

int parse_int (blockette_ptr, number_of_bytes)
char **blockette_ptr;
int number_of_bytes;
{
	memncpy (temp_char, *blockette_ptr, number_of_bytes);
	*blockette_ptr += number_of_bytes;
	return (atoi (temp_char));
}
