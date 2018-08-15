/*===========================================================================*/
/* SEED reader     |              print_type51                |  station header */
/*===========================================================================*/
/*
	Name:		print_type51
	Purpose:	print all station comments for the current station to standard
				output
	Usage:		void print_type51 ();
				print_type51 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	current_station
				type51_head
	Warnings:	none
	Errors:		none
	Called by:	print_stationh
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type51_head is the first member
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

void print_type51(print_stn_header)
int print_stn_header;

{

	struct type51 *type51;		/* looping vbl */
	int i;
	char *blkt_id="B051";		/* blockette id string */



	fprintf (outputfile,"%s+                  +-----------------", com_strt);

        fprintf (outputfile,"-------------------+                  +\n");

        fprintf (outputfile,"%s+                  |   Station Comments,    %5s, %2s   |                  +\n", com_strt, current_station->station, current_station->network_code);

        fprintf (outputfile,"%s+                  +------------------------------------+                  +\n", com_strt);



	fprintf(outputfile,"%s\n", com_strt);

	if (print_stn_header)
	{

        	fprintf(outputfile,"%s%s%2.2d\t\tStation code:\t\t\t\t%s\n",
                        	"B050",
				fld_pref, 3,
				current_station->station ? current_station->station : "(null)");

		fprintf(outputfile,"%s%s%2.2d\t\tNetwork Code:\t\t\t\t%s\n",
                                "B050",
				fld_pref, 16,
                                current_station->network_code ? current_station->network_code : "??");

	}

        fprintf(outputfile,"%s\n", com_strt);

	i = 0;

	for (type51 = current_station->type51_head; type51 != NULL; type51 = type51->next)
	{
	

		fprintf(outputfile,"%sStation comment %d:\n",com_strt,i++);
		
		fprintf(outputfile,
			  "%s%s%2.2d\t\tStarting effective time:\t\t%s\n", 
				blkt_id,
				fld_pref,
				3,
				type51->start ? type51->start : "(null)");

		fprintf(outputfile,
			"%s%s%2.2d\t\tEnding effective time:\t\t\t%s\n", 
				blkt_id,
				fld_pref,
				4,
				type51->end ? type51->end : "(null)");

		fprintf(outputfile,"%s%s%2.2d\t\tIndex code for comment:  %4d\t",
				blkt_id,
				fld_pref,
				5,
				type51->comment_code);


		find_type31(outputfile, type51->comment_code);

		fprintf(outputfile,
			"%s%s%2.2d\t\tIndex code for level:\t\t\t%ld\n",
				blkt_id,
				fld_pref,
				6,
				type51->level_code);

		fprintf(outputfile,"%s\n",com_strt);
	}

}
