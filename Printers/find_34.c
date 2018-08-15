/*===========================================================================*/
/* SEED reader     |              print_type34                |   abbrev dctnry */
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

void find_type34 (fp,code)
FILE *fp;
int code;
{
	struct type34 *type34;							/* looping vbl */

	for (type34 = type34_head; type34 != NULL; type34 = type34->next)
	{
		if (type34->code == code)
		{
			fprintf (fp,"%s - %s\n",
				type34->name ? type34->name : "(null)", 
				type34->description ? type34->description : "(null)");
			break;
		}
	}
	if (type34 == NULL)
		fprintf (fp,"No Abbreviation Referenced\n");
}
