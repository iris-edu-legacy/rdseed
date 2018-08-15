/*===========================================================================*/
/* SEED reader     |             find_type31               |   abbrev dctnry */
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

void find_type31 (fp,code)
FILE *fp;
int code;
{
	struct type31 *type31;	/* looping vbl */

	for (type31 = type31_head; type31 != NULL; type31 = type31->next)
	{
		if (type31->code == code)
		{
			fprintf (fp,"\t%s ",
				type31->comment ? type31->comment : "(null)");

			if (type31->units != 0)
			{
				find_type34(fp, type31->units);
			}
			else 
				fprintf(fp, "\n");

			break;
		}
	}
	if (type31 == NULL)
		fprintf (fp,"!!! ERROR - Comment Code NOT FOUND !!!\n");
}
