/*===========================================================================*/
/* SEED reader     |              print_type59                |  station header */
/*===========================================================================*/
/*
	Name:		print_type59
	Purpose:	print the channel comments table to the standard output
	Usage:		void print_type59 ();
				print_type59 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type59_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_channel
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type59_head is the first member
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

void print_type59 (type59_head, print_stn_header)
struct type59 *type59_head;
int print_stn_header;

{

	struct type59 *type59;							/* looping vbl */
	int i;
	char *blkt_id="B059";			/* blockette id string */

 	fprintf (outputfile,"%s+                  +--------------------", com_strt);
	fprintf (outputfile,"-------------------+                  +\n");
	fprintf (outputfile,"%s+                  |   Channel Comments,    %5s ch %3s",
			 com_strt,current_station->station, current_channel->channel);
	fprintf (outputfile,"   |                  +\n");
	fprintf (outputfile,"%s+                  +--------------------",com_strt);
	fprintf (outputfile,"-------------------+                  +\n");
	fprintf (outputfile,"%s\n",com_strt);

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

		fprintf(outputfile,
				"%s%s%2.2d\t\tLocation:\t\t\t\t%s\n",
               			"B052", 
				fld_pref, 3,
				strcmp(current_channel->location, "") != 0 ? 
						current_channel->location : "??");
        	fprintf(outputfile,
			"%s%s%2.2d\t\tChannel:\t\t\t\t%s\n",
                        	"B052",
				fld_pref, 4,
				current_channel->channel ? current_channel->channel : "(null)");
	}

	i = 0;
	for (type59 = type59_head; type59 != NULL; type59 = type59->next)
	{
		fprintf(outputfile,"%sChannel comment %d\n",com_strt,i++);

		fprintf(outputfile,"%s%s%2.2d\t\tStarting effective time:\t\t%s\n", 
				blkt_id,
				fld_pref, 3,
				type59->start ? type59->start : "(null)");

		fprintf(outputfile,"%s%s%2.2d\t\tEnding effective time:\t\t\t%s\n", 
				blkt_id,
				fld_pref, 4,
				type59->end ? type59->end : "(null)");

		fprintf(outputfile,"%s%s%2.2d\t\tIndex code for comment:%4d\t",
				blkt_id,fld_pref,5,type59->comment_code);

		find_type31(outputfile, type59->comment_code);

		fprintf(outputfile,"%s%s%2.2d\t\tIndex code for level:\t%ld\n",
				blkt_id,fld_pref,6,type59->level_code);

		fprintf(outputfile,"%s\n",com_strt);
	}
}
