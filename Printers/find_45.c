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
				02/16/94  CL made fprintfs null-pointer protected
*/

#include "rdseed.h"
#include "resp_defs.h"

int find_type45(fp, code)
FILE *fp;
int code;
{
	struct type45 *type45;					    /* looping vbl */
	int i;										/* counter */
	char *blkt_id="B045";						/* blockette id string */

	for (type45 = type45_head; type45 != NULL; type45 = type45->next)
	{
		if (type45->response_code == code)
		{
		    fprintf (fp,"%s+                     +-----------------",com_strt);
		    fprintf (fp,"----------------+                     +\n");
		    fprintf (fp,"%s+                     |   Response List, %5s ch %3s",com_strt,
				   current_station->station, current_channel->channel);
		    fprintf (fp,"   |                     +\n");
		    fprintf (fp,"%s+                     +-----------------",com_strt);
		    fprintf (fp,"----------------+                     +\n");
		    fprintf (fp,"%s\n",com_strt);

			fprintf (fp,"%s%s%2.2d     Response in units lookup:              ",
				blkt_id,fld_pref,5);

			find_type34(fp,type45->input_units_code);

			fprintf (fp,"%s%s%2.2d     Response out units lookup:             ",
				blkt_id,fld_pref,6);

			find_type34(fp,type45->output_units_code);

			fprintf (fp,"%s%s%2.2d     Number of responses:                   %d\n",
				blkt_id,fld_pref,7,type45->number_responses);

			fprintf (fp,"%sResponses:\n",com_strt);

			fprintf (fp,"%s  i, amplitude,    amplitude error, phase,        phase error\n",
					 com_strt);

			for (i = 0; i < type45->number_responses; i++)
				fprintf (fp,"%s%s%2.2d-%2.2d  %3d % E % E    % E % E\n",
						 blkt_id,fld_pref,8,12,i,
						 type45->response[i].frequency, 
						 type45->response[i].amplitude,
						 type45->response[i].amplitude_error,
						 type45->response[i].phase,
						 type45->response[i].phase_error);
			fprintf (fp,"%s\n",com_strt);
			break;
		}
	}
	if (type45 == NULL) return(0); else return(1);
}

int old_find_type45(fp, code)
FILE *fp;
int code;
{
	struct type45 *type45;					/* looping vbl */
	int i;										/* counter */

	for (type45 = type45_head; type45 != NULL; type45 = type45->next)
	{
		if (type45->response_code == code)
		{
			fprintf (fp,"B045\n");
			fprintf (fp,"Response Lookup Code:                  %d\n",
				type45->response_code);

			fprintf (fp,"Response name:                         %s\n",
				type45->name ? type45->name : "(null)");

			fprintf (fp,"Response in units lookup: %4d         ",
				type45->input_units_code);

			find_type34(fp,type45->input_units_code);

			fprintf (fp,"Response out units lookup: %4d        ",
				type45->output_units_code);

			find_type34(fp,type45->output_units_code);

			fprintf (fp,"Number of responses:                   %d\n",
				type45->number_responses);

			fprintf (fp,"Responses:\n");

			fprintf (fp,"  i, amplitude,    amplitude error, phase,        phase error\n");

			for (i = 0; i < type45->number_responses; i++)
				fprintf (fp,"%3d % E % E    % E % E\n", i, type45->response[i].frequency, 
				type45->response[i].amplitude, type45->response[i].amplitude_error,
				type45->response[i].phase, type45->response[i].phase_error);
			fprintf (fp,"\n");
			break;
		}
	}
	if (type45 == NULL) return(0); else return(1);
}
