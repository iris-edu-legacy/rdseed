/*===========================================================================*/
/* SEED reader     |             print_volh                |   volume header */
/*===========================================================================*/
/*
	Name:		print_volh
	Purpose:	print a volume header, including all type 01x blockettes, to
				the standard output
	Usage:		void print_volh ();
				print_stationh ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	main
	Calls to:	print_type10 - print the volume id structure
				print_vhsib - print the volume station header index
				print_type12 - print the volume time span index
	Algorithm:	call routines to print the volume header structures to standard
				output
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

void print_volh ()
{
	pagetop ();

	print_type10 ();
	print_type11 ();
	print_type12 ();
}
