/*===========================================================================*/
/* SEED reader     |         read_logical_record           |         utility */
/*===========================================================================*/
/*
	Name:		read_logical_record
	Purpose:	read a logical record from the input file
	Usage:		int read_logical_record ();
				int result;
				FILE *inputfile;
				result = read_logical_record (inputfile);
	Input:		inputfile = a pointer to an open input file
	Output:		result = FALSE if there is no more data to be read, TRUE
					otherwise
	Externals:	input - structure for an input record; defined in structures.h,
					allocated in globals.h
				FALSE and TRUE are defined in macros.h via rdseed.h
	Warnings:	none
	Errors:		none
	Called by:	main, read_blockette
	Calls to:	memncpy - does what strncpy is alleged to do
	Algorithm:	read an input record, recover its sequence number, type, and
				continuation flag, and data.  If there are more data to be
				recovered, return TRUE, otherwise return FALSE.
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

void read_logical_record (input_ptr)
char *input_ptr;
{
	memset((char *)&input, 0, sizeof(input));

	/* get the record number from the record */
	memncpy (temp_char, input_ptr, 6);
	input.recordnumber = atol (temp_char);

	/* get the record type */
	input.type = *(input_ptr + 6);

	/* determine whether the record is a continuation of the previous one */
	if (*(input_ptr + 7) == '*') input.continuation = TRUE;
	else input.continuation = FALSE;

	/* recover the data from the record */
// 	if (input.type != 'D' || input.type != 'R' || input.type != 'Q') 
//	{
		/*memncpy (input.data, (input_ptr + 8), LRECL-8);*/

		memcpy (input.data, (input_ptr + 8), (size_t) LRECL-8);

//	}
}
