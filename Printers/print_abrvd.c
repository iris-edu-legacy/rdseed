/*===========================================================================*/
/* SEED reader     |              print_abrvd              |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		print_abrvd
	Purpose:	print all abbreviation dictionaries, including all type 03x 
				blockettes, to the standard output
	Usage:		void print_abrvd ();
				print_abrvd ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	main
	Calls to:	print_type30 - print data format dictionary
				print_type31 - print comment description dictionary
				print_type32 - print cited source dictionary
				print_type33 - print generic abbreviation dictionary
				print_type34 - print units abbreviation dictionary
				print_type35 - print beam configuration dictionary
	Algorithm:	call procedures to print the various dictionaries to the
				standard output
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				10/13/95  TJM   prefix strings added to make consistent with RESP files
				03/25/99  Stephane Zuzlewski	added support for blockette 42
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_abrvd ()
{
	fprintf(outputfile,"%s+---------------------------------------",com_strt);
	fprintf(outputfile,"--------------------------------------+\n");
	fprintf(outputfile,"%s| Abbreviations    |  Codes and descript",com_strt);
	fprintf(outputfile,"ions used in SEED  |        SEED tape |\n");
	fprintf(outputfile,"%s+---------------------------------------",com_strt);
	fprintf(outputfile,"--------------------------------------+\n");
	fprintf(outputfile,"%s\n",com_strt);

	print_type30 ();
	print_type31 ();
	print_type32 ();
	print_type33 ();
	print_type34 ();
	print_type35 ();

	print_type41 ();
	print_type42 ();
	print_type43 ();
	print_type44 ();
	print_type45 ();
	print_type46 ();
	print_type47 ();
	print_type48 ();
}
