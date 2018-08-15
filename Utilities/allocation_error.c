/*===========================================================================*/
/* SEED reader     |           allocation_error            |         utility */
/*===========================================================================*/
/*
	Name:		allocation_error
	Purpose:	print an error message after a memory allocation failure, quit
	Usage:		void allocation_error ();
				char *message_string;
				allocation_error (message_string);
	Input:		message_string = a message to be printed; this will usually be 
				the name of the calling procedure (where memory allocation 
				failed.
	Output:		none
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	anything
	Calls to:	none
	Algorithm:	print a message, terminate execution
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include <stdio.h>
#include <stdlib.h>

void allocation_error (string)
char *string;
{
	fprintf (stderr,"ERROR (%s):  ", string);
	fprintf (stderr, "unable to allocate sufficient memory.\n");
	fprintf (stderr, "\tExecution terminating.\n");
	exit (-1);
}
