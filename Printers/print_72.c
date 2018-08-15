/*==============================================================================*/
/* SEED reader     |               print_type72               |     time header */
/*==============================================================================*/
/*
	Name:		print_type72
	Purpose:	print the event phases table to the standard output
	Usage:		void print_type72 ();
				print_type72 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type72_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_timeh
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type72_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				04/26/93  Allen Nance     added fields 11,12 for v2.3
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type72 (type72_head)
struct type72 *type72_head;
{
	struct type72 *type72;							/* looping vbl */
	char *blkt_id="B072";						/* blockette id string */

	if (type72_head != NULL)
	{
		fprintf(outputfile,"%sEvent phases:\n",com_strt);

		for (type72 = type72_head; type72 != NULL; type72 = type72->next)
		{
			fprintf(outputfile,"%s%s%2.2d     \tStation id:                   %s\n", 
					blkt_id,fld_pref,3,type72->station ? type72->station : "(null)");

			fprintf(outputfile,"%s%s%2.2d     \tLocation id:                  %s\n",  
					blkt_id,fld_pref,4,type72->location ? type72->location : "(null)");
			fprintf(outputfile,"%s%s%2.2d     \tChannel id:                   %s\n",
					blkt_id,fld_pref,5,type72->channel ? type72->channel: "(null)");
			fprintf(outputfile,"%s%s%2.2d     \tPhase arrival time:           %s\n", 
					blkt_id,fld_pref,6,type72->arrival_time ? type72->arrival_time : "(null)");

			fprintf(outputfile,"%s%s%2.2d     \tSignal amplitude:             %E\n",
					blkt_id,fld_pref,7,type72->amplitude);
			fprintf(outputfile,"%s%s%2.2d     \tSignal period (1/frequency):  %E\n",
					blkt_id,fld_pref,8,type72->period);
			fprintf(outputfile,"%s%s%2.2d     \tSignal to noise ratio:        %E\n",
					blkt_id,fld_pref,9,type72->sig2noise);
			fprintf(outputfile,"%s%s%2.2d     \tPhase name:                   %s\n", 
					blkt_id,fld_pref,10,type72->phasename ? type72->phasename : "(null)");
			if (type10.version >= 2.3)
			{
				fprintf(outputfile,"%s%s%2.2d     \tSource lookup:  %4d          ", 
						blkt_id,fld_pref,11,type72->source_code);
				find_type32(outputfile,type72->source_code);
				fprintf(outputfile,"%s%s%2.2d     \tNetwork Code:                 %s\n", 
						blkt_id,fld_pref,12,
						type72->network_code ? type72->network_code : "(null)");
			}

			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
