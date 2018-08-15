/*===========================================================================*/
/* SEED reader     |             print_type47              |  station header */
/*===========================================================================*/
/*
	Name:		print_type47
	Purpose:	print the channel generic response table to standard output
	Usage:		void print_type47 ();
				print_type47 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type47_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type47_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill
				05/09/90  Sue Schoch      Initial release 2.0
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type47()
{
	struct type47 *type47;							/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B047";						/* blockette id string */

	if (type47_head != NULL)
	{
    	fprintf(outputfile,"%s+                  +--------------------",com_strt);
    	fprintf(outputfile,"-------------------+                  +\n");
    	fprintf(outputfile,"%s+------------------|         Decimation Dictionary",
				com_strt);
    	fprintf(outputfile,"         |------------------+\n");
    	fprintf(outputfile,"%s+                  +--------------------",com_strt);
    	fprintf(outputfile,"-------------------+                  +\n");
    	fprintf(outputfile,"%s\n",com_strt);
	
	
		for (type47 = type47_head; type47 != NULL; type47 = type47->next)
		{
			fprintf(outputfile,"%s%s%2.2d     Response Lookup Code:                  %d\n",
					blkt_id,fld_pref,3,type47->response_code);
			fprintf(outputfile,"%s%s%2.2d     Response name:                         %s\n",
					blkt_id,fld_pref,4,type47->name ? type47->name : "(null)");
			fprintf(outputfile,"%s%s%2.2d     Response input sample rate:            %E\n",
					blkt_id,fld_pref,5,type47->input_sample_rate );
			fprintf(outputfile,"%s%s%2.2d     Response decimation factor:            %d\n",
					blkt_id,fld_pref,6,type47->decimation_factor );
			fprintf(outputfile,"%s%s%2.2d     Response decimation offset:            %d\n",
					blkt_id,fld_pref,7,type47->decimation_offset );
			fprintf(outputfile,"%s%s%2.2d     Response delay:                        %E\n",
					blkt_id,fld_pref,8,type47->delay );
			fprintf(outputfile,"%s%s%2.2d     Response correction:                   %E\n",
					blkt_id,fld_pref,9,type47->correction );
			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
