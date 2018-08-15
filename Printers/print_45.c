/*===========================================================================*/
/* SEED reader     |            print_type45               |  station header */
/*===========================================================================*/
/*
	Name:		print_type45
	Purpose:	print the channel list response table to standard output
	Usage:		void print_type45 ();
				print_type45 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type45_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type45_head is the first member
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

void print_type45 ()
{
	struct type45 *type45;							/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B045";						/* blockette id string */

	if (type45_head != NULL)
	{
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s+------------------|       Response List Dictionary",
				com_strt);
		fprintf(outputfile,"        |------------------+\n");
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s\n",com_strt);
	
	
		for (type45 = type45_head; type45 != NULL; type45 = type45->next)
		{
			fprintf(outputfile,"%s%s%2.2d     Response Lookup Code:                  %d\n",
					blkt_id,fld_pref,3,type45->response_code);
			fprintf(outputfile,"%s%s%2.2d     Response name:                         %s\n",
					blkt_id,fld_pref,4,type45->name ? type45->name : "(null)");
			fprintf(outputfile,"%s%s%2.2d     Response in units lookup:  %4d        ",
					blkt_id,fld_pref,5,type45->input_units_code);
			find_type34(outputfile, type45->input_units_code);
			fprintf(outputfile,"%s%s%2.2d     Response out units lookup: %4d        ",
					blkt_id,fld_pref,6,type45->output_units_code);
			find_type34(outputfile, type45->output_units_code);
			fprintf(outputfile,"%s%s%2.2d     Number of responses:                   %d\n",
					blkt_id,fld_pref,7,type45->number_responses);
			fprintf(outputfile,"%sResponses:\n",com_strt);
			fprintf(outputfile,"%s  i, amplitude,    amplitude error, phase,        phase error\n",
					com_strt);
			for (i = 0; i < type45->number_responses; i++)
				fprintf(outputfile,"%s%s%2.2d-%2.2d  %3d % E % E    % E % E\n",
						blkt_id,fld_pref,8,12,i,
						type45->response[i].frequency, 
						type45->response[i].amplitude,
						type45->response[i].amplitude_error,
						type45->response[i].phase,
						type45->response[i].phase_error);
			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
