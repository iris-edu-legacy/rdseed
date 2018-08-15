/*===========================================================================*/
/* SEED reader     |              print_type70               |     time header */
/*===========================================================================*/
/*
	Name:		print_type70
	Purpose:	print the time span id table to the standard output
	Usage:		void print_type70 ();
				print_type70 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type70_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_timeh
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type70_head is the first member
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

void print_type70 ()
{
	struct type70 *type70;							/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B070";						/* blockette id string */

	if (type70_head != NULL)
	{
		fprintf(outputfile,"%sTime spans in this data set:\n",com_strt);

		i = 1;
		fprintf(outputfile,"%s\t               Time, start of span     ",
				com_strt);
		fprintf(outputfile,"Time, end of span       Flag\n");

		for (type70 = type70_head; type70 != NULL; type70 = type70->next)
		{
			fprintf(outputfile,"%s%s%2.2d-%2.2d  \tTime span %2d:  %-22s  %-22s  %s\n",
				blkt_id,fld_pref,3,5,i++, 
				type70->start_of_data ? type70->start_of_data : "(null)", 
				type70->end_of_data ? type70->end_of_data : "(null)", 
				type70->flag ? type70->flag : "(null)");
		}

		fprintf(outputfile,"%s\n",com_strt);
	}
}
