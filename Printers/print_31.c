/*===========================================================================*/
/* SEED reader     |              print_type31               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		print_type31
	Purpose:	print the contents of the comment dictionary to standard output
	Usage:		void print_type31 ();
				print_type31 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type31_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_abbrevdic
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type31_head is the first member
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

void print_type31 ()
{
	struct type31 *type31;							/* looping vbl */
	char *blkt_id="B031";						/* blockette id string */

	if (type31_head != NULL)
	{
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s+------------------|    Comment descript",com_strt);
		fprintf(outputfile,"ion dictionary     |------------------+\n");
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s\n",com_strt);

		for (type31 = type31_head; type31 != NULL; type31 = type31->next)
		{
			fprintf(outputfile,"%s%s%2.2d     Comment code id:                       %d\n",
					blkt_id,fld_pref,3,type31->code);
			fprintf(outputfile,"%s%s%2.2d     Comment class code:                    %s\n",
					blkt_id,fld_pref,4,type31->class ? type31->class : "(null)");
			fprintf(outputfile,"%s%s%2.2d     Comment text:                          %s\n",
					blkt_id,fld_pref,5,type31->comment ? type31->comment : "(null)");
			fprintf(outputfile,"%s%s%2.2d     Comment units:   %4d                  ",
					blkt_id,fld_pref,6,type31->units);
			if (type31->units == 0) fprintf(outputfile,"No Comment Units\n");
			else find_type34(outputfile,type31->units);
			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
