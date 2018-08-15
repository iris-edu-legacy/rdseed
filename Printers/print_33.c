/*===========================================================================*/
/* SEED reader     |              print_type33                |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		print_type33
	Purpose:	print the contents of the generic abbreviation dictionary to 
					standard output
	Usage:		void print_type33 ();
				print_type33 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type33_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_abbrevdic
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type33_head is the first member
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

void print_type33 ()
{
	struct type33 *type33;							/* looping vbl */
	char *blkt_id="B033";						/* blockette id string */

	if (type33_head != NULL)
	{
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s+------------------|    Generic abbrevia",com_strt);
		fprintf(outputfile,"tion dictionary    |------------------+\n");
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s\n",com_strt);

		for (type33 = type33_head; type33 != NULL; type33 = type33->next)
		{
			fprintf(outputfile,"%s%s%2.2d     Description key code:                  %d\n",
					blkt_id,fld_pref,3,type33->code);
			fprintf(outputfile,"%s%s%2.2d     Abbreviation description:              %s\n",
					blkt_id,fld_pref,4,type33->abbreviation ? type33->abbreviation : "(null)");
			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
