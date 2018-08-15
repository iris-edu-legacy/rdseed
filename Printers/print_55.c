/* UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED */
/*      Data of this type were unavailable when this process was written     */

/*===========================================================================*/
/* SEED reader     |              print_type55                |  station header */
/*===========================================================================*/
/*
	Name:		print_type55
	Purpose:	print the channel list response table to standard output
	Usage:		void print_type55 ();
				print_type55 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type55_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type55_root is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type55 (fp,type55_root)
FILE *fp;
struct type55 *type55_root;
{
	struct type55 *type55;						/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B055";						/* blockette id string */

	fprintf (fp,"%s+                     +-----------------",com_strt);
	fprintf (fp,"----------------+                     +\n");
	fprintf (fp,"%s+                     |   Response List, %5s ch %3s",com_strt,
		current_station->station, current_channel->channel);
	fprintf (fp,"   |                     +\n");
	fprintf (fp,"%s+                     +-----------------",com_strt);
	fprintf (fp,"----------------+                     +\n");
	fprintf (fp,"%s\n",com_strt);

	for (type55 = type55_root; type55 != NULL; type55 = type55->next)
	{
		fprintf (fp,"%s%s%2.2d     Stage sequence number:                 %d\n",
			blkt_id,fld_pref,3,type55->stage);
		fprintf (fp,"%s%s%2.2d     Response in units lookup:              ",
			blkt_id,fld_pref,4);
		find_type34(fp,type55->input_units_code);
		fprintf (fp,"%s%s%2.2d     Response out units lookup:             ",
			blkt_id,fld_pref,5);
		find_type34(fp,type55->output_units_code);
		fprintf (fp,"%s%s%2.2d     Number of responses:                   %d\n",
			blkt_id,fld_pref,6,type55->number_responses);
		fprintf (fp,"%sResponses:\n",com_strt);
		fprintf (fp,"%s  frequency\t amplitude\t amp error\t    phase\t phase error\n",
				 com_strt);
		for (i = 0; i < type55->number_responses; i++)
			fprintf (fp,"%s%s%2.2d-%2.2d  %E\t%E\t%E\t%E\t%E\n",
					    blkt_id,fld_pref,7,11, 
						type55->response[i].frequency, 
						type55->response[i].amplitude, 
						type55->response[i].amplitude_error,
						type55->response[i].phase, 	
						type55->response[i].phase_error);
		fprintf (fp,"%s\n",com_strt);
	}
}

void old_print_type55 (fp,type55_root)
FILE *fp;
struct type55 *type55_root;
{
	struct type55 *type55;							/* looping vbl */
	int i;										/* counter */

	fprintf (fp,"+                  +--------------------");
	fprintf (fp,"-------------------+                  +\n");
	fprintf (fp,"+                  |   Instrument response, %5s ch %3s",
		current_station->station, current_channel->channel);
	fprintf (fp,"   |                  +\n");
	fprintf (fp,"+                  +--------------------");
	fprintf (fp,"-------------------+                  +\n");
	fprintf (fp,"\n");

	fprintf (fp,"Response type:                         List\n");

	for (type55 = type55_root; type55 != NULL; type55 = type55->next)
	{
		fprintf (fp,"Stage sequence number:                 %d\n",
			type55->stage);
		fprintf (fp,"Response in units lookup:  %4d        ",
			type55->input_units_code);
		find_type34(fp,type55->input_units_code);
		fprintf (fp,"Response out units lookup:  %4d       ",
			type55->output_units_code);
		find_type34(fp,type55->output_units_code);
		fprintf (fp,"Number of responses:                   %d\n",
			type55->number_responses);
		fprintf (fp,"Responses:\n");
		fprintf (fp,"  frequency\t amplitude\t amp error\t    phase\t phase error\n");
		for (i = 0; i < type55->number_responses; i++)
			fprintf (fp,"%E\t%E\t%E\t%E\t%E\n", 
						type55->response[i].frequency, 
						type55->response[i].amplitude, 
						type55->response[i].amplitude_error,
						type55->response[i].phase, 	
						type55->response[i].phase_error);
		fprintf (fp,"\n");
	}
}
