/*===========================================================================*/
/* SEED reader     |             print_type73              |     time header */
/*===========================================================================*/
/*
	Name:		print_type73
	Purpose:	print the time span data start index table to the standard
				output
	Usage:		void print_type73 ();
				print_type73 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type73_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_timeh
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type73_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				08/26/89  Dennis O'Neill  Moved sequence_number to column 1
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type73 ()
{
	struct type73 *type73;						/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B073";						/* blockette id string */

	if (type73_head != NULL)
	{
		/* print column headings for time index to available waveforms */
		fprintf(outputfile,"%sRecord  ",com_strt);
		fprintf(outputfile,"Station  ");
		fprintf(outputfile,"Channel  ");
		fprintf(outputfile,"Start date and time     ");
		fprintf(outputfile,"Loc  ");
		fprintf(outputfile,"Subseq\n");

		for (type73 = type73_head; type73 != NULL; type73 = type73->next)
		{
			for (i = 0; i < type73->number_datapieces; i++)
			{
				if (strlen (type73->datapiece[i].station) != 0)
				{
					fprintf(outputfile,"%s%s%2.2d-%2.2d  %-6ld  %-6s   %-6s  %-22s %6s %2d\n",
					blkt_id,fld_pref,4,9,type73->datapiece[i].sequence_number,
					type73->datapiece[i].station ? type73->datapiece[i].station : "(null)",
					type73->datapiece[i].channel ? type73->datapiece[i].channel : "(null)",
					type73->datapiece[i].time ? type73->datapiece[i].time : "(null)",
					type73->datapiece[i].location ? type73->datapiece[i].location : "(null)",
					type73->datapiece[i].subsequence);
				}
			}
		}
	}
	fprintf(outputfile,"%s\n",com_strt);
}
