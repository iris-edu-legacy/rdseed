/*===========================================================================*/
/* SEED reader     |               print_stnh              |  station header */
/*===========================================================================*/
/*
	Name:		print_stnh
	Purpose:	print a station header, including all type 05x blockettes, to
				the standard output
	Usage:		void print_stnh ();
				print_stnh ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	current_station
	Warnings:	none
	Errors:		none
	Called by:	main
	Calls to:	print_type50 - print the station ident entry
				print_type51 - print the station comment entries
				print_channel - print channel information
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

void print_stnh ()
{
	print_type50 ();
	if (current_station->type51_head != NULL) print_type51 (0);
	if (current_station->type52_head != NULL) print_channel ();
}
