/* UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED */
/*      Data of this type were unavailable when this process was written     */

/*===========================================================================*/
/* SEED reader     |              print_type32               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		print_type32
	Purpose:	print the contents of the citation dictionary to standard output
	Usage:		void print_type32 ();
				print_type32 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type32_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_abbrevdic
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type32_head is the first member
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

void print_type32 ()
{
	struct type32 *type32;							/* looping vbl */
	char *blkt_id="B032";						/* blockette id string */

	if (type32_head != NULL)
	{
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s+------------------|        Cited source",com_strt);
		fprintf(outputfile," dictionary        |------------------+\n");
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s\n",com_strt);

		for (type32 = type32_head; type32 != NULL; type32 = type32->next)
		{
			fprintf(outputfile,"%s%s%2.2d     Source identification code:            %d\n",
					blkt_id,fld_pref,3,type32->code);
			fprintf(outputfile,"%s%s%2.2d     Author and/or publication name:        %s\n",
					blkt_id,fld_pref,4,type32->author ? type32->author : "(null)");
			fprintf(outputfile,"%s%s%2.2d     Date published, catalog info:          %s\n",
					blkt_id,fld_pref,5,type32->catalog ? type32->catalog : "(null)");
			fprintf(outputfile,"%s%s%2.2d     Publisher's name:                      %s\n",
					blkt_id,fld_pref,6,type32->publisher ? type32->publisher : "(null)");
			fprintf(outputfile,"%s\n",com_strt);
		}
	}	
}
