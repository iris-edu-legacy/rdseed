/*===========================================================================*/
/* SEED reader     |              memncpy                  |         utility */
/*===========================================================================*/
/*
	Name:		memncpy
	Purpose:	copy a given number of bytes from one string to another, 
				then null-terminate the destination string.
	Usage:		int memncpy ();
				char *input;
				char *output;
				int number_of_bytes;
				int number_copied;
				number_copied = memncpy (output, input, number_of_bytes);
	Input:		input = pointer to a section of memory (a string), input
				output = pointer to a section of memory (a string), output
				number_of_bytes = number of bytes to copy
	Output:		number_copied = number of bytes actually copied
				the specified number of bytes from input will be in output if
					available, otherwise all available bytes will be copied
	Warnings:	none
	Errors:		tried to copy too many bytes
	Called by:	anything
	Calls to:	none
	Algorithm:	copy the specified number of bytes; null-terminate.
	Notes:		to replace Sun strncpy, which didn't seem to do the job
					the output string must be >= number_of_bytes+1 in length
	Problems:	doesn't check to see that number of bytes wanted was
					actually copied.
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
			05/01/92  Allen Nance   fixed ouput message
				fprintf (stderr, "\tTry changing TEMP_CHAR_LENGTH in structures.h"); to
				fprintf (stderr, "\tTry changing TEMP_CHAR_LENGTH in rdseed.h");
*/

#include "rdseed.h"

int memncpy (output, input, number_of_bytes)
char *output;
char *input;
int number_of_bytes;
{
	int i;										/* counter */
	int ix;

	if (number_of_bytes > TEMP_CHAR_LENGTH)
	{
		fprintf (stderr, "ERROR (memncpy):  ");
		fprintf (stderr, "tried to copy %d bytes; maximum is %d.\n",
			number_of_bytes, TEMP_CHAR_LENGTH);
		fprintf (stderr, "\tTry changing TEMP_CHAR_LENGTH in rdseed.h");
		fprintf (stderr, " and recompiling everything.\n");
		fprintf (stderr, "\tExecution terminating.\n");
		exit (-1);
	}

	/* copy input string to output string; null-terminate the output */
	for (i = 0; i < number_of_bytes; i++) 
	{
		output[i] = input[i];
	}

	output[i] = 0;

	/* return the length of the output string to the caller */
	i = strlen (output);

	return (i);

/*	return (number_of_bytes); */
}
