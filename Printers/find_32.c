/*===========================================================================*/
/* SEED reader     |             find_type32               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		print_type34
	Purpose:	print the contents of the units dictionary to standard output
	Usage:		void print_type34 ();
				print_type34 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type34_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_abbrevdic
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type34_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				02/17/93  CL    nullpointer protect fprintfs 

*/

#include "rdseed.h"

void find_type32 (fp,code)
FILE *fp;
int code;
{
	struct type32 *type32;							/* looping vbl */

	for (type32 = type32_head; type32 != NULL; type32 = type32->next)
	{
		if (type32->code == code)
		{
			fprintf (fp,"%s,%s,%s\n",
				type32->author ? type32->author : "(null)",
				type32->catalog ? type32->catalog: "(null)",
				type32->publisher ? type32->publisher: "(null)");
			break;
		}
	}
	if (type32 == NULL)
		fprintf (fp,"!!! ERROR - Cited Source Code NOT FOUND !!!\n");
}
