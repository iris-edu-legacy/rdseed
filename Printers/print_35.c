/* UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED */
/*      Data of this type were unavailable when this process was written     */

/*===========================================================================*/
/* SEED reader     |              print_type35                |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		print_type35
	Purpose:	print the contents of the beam configuration table to standard 
				output
	Usage:		void print_type35 ();
				print_type35 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type35_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_abbrevdic
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type35_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				09/17/89  Dennis O'Neill  added subchannel line
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type35 ()
{
	struct type35 *type35;							/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B035";						/* blockette id string */

	if (type35_head != NULL )
	{
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s+------------------|     Beam configurat",com_strt);
		fprintf(outputfile,"ion dictionary     |------------------+\n");
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s\n",com_strt);

		for (type35 = type35_head; type35 != NULL; type35 = type35->next)
		{
			fprintf(outputfile,"%s%s%2.2d     Beam code:                             %d\n",
					blkt_id,fld_pref,3,type35->code);
			for (i = 0; i < type35->number_beams; i++)
			{
				fprintf(outputfile,"%sComponent #:                           %d\n",
						com_strt,i);
				fprintf(outputfile,"%s%s%2.2d     Station ID:                            %s\n",
						blkt_id,fld_pref,5,type35->beam[i].station ? type35->beam[i].station : "(null)");
				fprintf(outputfile,"%s%s%2.2d     Location ID:                           %s\n",
						blkt_id,fld_pref,6,type35->beam[i].location ? type35->beam[i].location : "(null)");
				fprintf(outputfile,"%s%s%2.2d     Channel ID:                            %s\n",
						blkt_id,fld_pref,7,type35->beam[i].channel ? type35->beam[i].channel : "(null)");
				fprintf(outputfile,"%s%s%2.2d     Subchannel:                            %d\n",
						blkt_id,fld_pref,8,type35->beam[i].subchannel);
				fprintf(outputfile,"%s%s%2.2d     Channel weight:                        %f\n",
						blkt_id,fld_pref,9,type35->beam[i].weight);
			}
			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
