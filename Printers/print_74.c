/*===========================================================================*/
/* SEED reader     |             print_type74              |     time header */
/*===========================================================================*/
/*
	Name:		print_type74
	Purpose:	print the time series index table to the standard output
	Usage:		void print_type74 ();
				print_type74 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type74_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_timeh
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type74_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	08/29/89  Dennis O'Neill  Initial version
				04/26/93  Allen Nance     added field 16 for v2.3
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type74 ()
{
	struct type74 *type74;						/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B074";						/* blockette id string */

	if (type74_head != NULL)
	{

		/* print column headings for time index to available waveforms */
		fprintf(outputfile,"%sSta   ",com_strt);
		fprintf(outputfile,"Loc ");
		fprintf(outputfile,"Chn ");
		if (type10.version >= 2.3)
			fprintf(outputfile,"Net ");
		fprintf(outputfile,"Start date and time    ");
		fprintf(outputfile,"Record ");
		fprintf(outputfile,"Sub ");
		fprintf(outputfile,"End date and time      ");
		fprintf(outputfile,"Record ");
		fprintf(outputfile,"Sub ");
		fprintf(outputfile,"Accel\n");

		for (type74 = type74_head; type74 != NULL; type74 = type74->next)
		{
			fprintf(outputfile,"%s%s%2.2d-%2.2d      %-5.5s ",
					blkt_id,fld_pref,3,16,
					type74->station ? type74->station : "     ");

			fprintf(outputfile,"%-2.2s  ", 
					type74->location ? type74->location : "  ");

			fprintf(outputfile,"%-3.3s ",  
					type74->channel ? type74->channel : "   ");

			fprintf(outputfile,"%-2.2s  ",  
					type74->network_code ? type74->network_code : "  ");

			fprintf(outputfile,"%-22.22s ", 
					type74->starttime ? type74->starttime : "(null)");

			fprintf(outputfile,"%-6ld ", type74->start_index);

			fprintf(outputfile,"%-2d  ", type74->start_subindex);

			fprintf(outputfile,"%-22.22s ", 
					type74->endtime ? type74->endtime : "(null)");

			fprintf(outputfile,"%-6ld ", type74->end_index);
			fprintf(outputfile,"%-2d  ", type74->end_subindex);
			fprintf(outputfile,"%-2d\n" , type74->number_accelerators);

/*
			for (i = 0; i < type74->number_accelerators; i++)
			{
				
				if (strlen (type74->accelerator[i].index) != 0)
				{
					fprintf(outputfile,"%6ld  %-22s  %2d\n",
					type74->accelerator[i].index,
					type74->accelerator[i].time,
					type74->accelerator[i].subindex);
				}
			}
*/
		}
	}
	fprintf(outputfile,"%s\n",com_strt);
}
