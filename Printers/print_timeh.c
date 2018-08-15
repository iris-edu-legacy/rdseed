/*===========================================================================*/
/* SEED reader     |             print_timeh               |     time header */
/*===========================================================================*/
/*
	Name:		print_timeh
	Purpose:	print a time span control header, including all type 07x 
				blockettes, to the standard output
	Usage:		void print_timeh ();
				print_timeh ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	main
	Calls to:	print_type70 - print the time span id table
				print_type71 - print the hypocenter information table
				print_type72 - print the event phases table
				print_type73 - print the time span data start index table
				print_type74 - print the time series index table
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				08/26/89  Dennis O'Neill  Moved Record# to column 1
*/

#include "rdseed.h"

void print_timeh ()
{
	print_type70 ();
	print_type71 ();

/*	if (type72_head != NULL) print_type72 (type72_head); */

	print_type73 ();
	print_type74 ();
}
