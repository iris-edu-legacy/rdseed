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
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type34 ()
{
	struct type34 *type34;							/* looping vbl */
	char *blkt_id="B034";						/* blockette id string */

	if (type34_head != NULL)
	{
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s+------------------|     Units abbreviat",com_strt);
		fprintf(outputfile,"ion dictionary     |------------------+\n");
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s\n",com_strt);

		for (type34 = type34_head; type34 != NULL; type34 = type34->next)
		{
			fprintf(outputfile,"%s%s%2.2d     Unit code:                             %d\n",
					blkt_id,fld_pref,3,type34->code);
			fprintf(outputfile,"%s%s%2.2d     Unit name:                             %s\n",
					blkt_id,fld_pref,4,type34->name ? type34->name : "(null)");
			fprintf(outputfile,"%s%s%2.2d     Unit description:                      %s\n",
					blkt_id,fld_pref,5,type34->description ? type34->description : "(null)");
			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
