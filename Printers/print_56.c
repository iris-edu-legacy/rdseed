/* UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED */
/*      Data of this type were unavailable when this process was written     */

/*===========================================================================*/
/* SEED reader     |              print_type56                |  station header */
/*===========================================================================*/
/*
	Name:		print_type56
	Purpose:	print the channel generic response table to standard output
	Usage:		void print_type56 ();
				print_type56 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type56_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type56_head is the first member
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

void print_type56 (fp,type56_head)
FILE *fp;
struct type56 *type56_head;
{
	struct type56 *type56;							/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B056";						/* blockette id string */

	fprintf (fp,"%s+                   +-------------------",com_strt);
	fprintf (fp,"-----------------+                    +\n");
	fprintf (fp,"%s+                   |   Generic response, %5s ch %3s",com_strt,
		current_station->station, current_channel->channel);
	fprintf (fp,"   |                    +\n");
	fprintf (fp,"%s+                   +-------------------",com_strt);
	fprintf (fp,"-----------------+                    +\n");
	fprintf (fp,"%s\n",com_strt);

	for (type56 = type56_head; type56 != NULL; type56 = type56->next)
	{
		fprintf (fp,"%s%s%2.2d     Stage sequence number:                 %d\n",
			blkt_id,fld_pref,3,type56->stage);
		fprintf (fp,"%s%s%2.2d     Response in units lookup:              ",
			blkt_id,fld_pref,4);
		find_type34(fp,type56->input_units_code);
		fprintf (fp,"%s%s%2.2d     Response out units lookup:             ",
			blkt_id,fld_pref,5);
		find_type34(fp,type56->output_units_code);
		fprintf (fp,"%s%s%2.2d     Number of corners:                     %d\n",
			blkt_id,fld_pref,6,type56->number_corners);
		fprintf (fp,"%sCorners:\n",com_strt);
		fprintf (fp,"%si, frequency, slope\n",com_strt);
		for (i = 0; i < type56->number_corners; i++)
			fprintf (fp,"%s%s%2.2d-%2.2d  %3d % E % E\n",blkt_id,fld_pref,7,8,i, 
			type56->corner[i].frequency,type56->corner[i].slope);
		fprintf (fp,"%s\n",com_strt);
	}
}

void old_print_type56 (fp,type56_head)
FILE *fp;
struct type56 *type56_head;
{
	struct type56 *type56;							/* looping vbl */
	int i;										/* counter */

    fprintf (fp,"+                  +--------------------");
    fprintf (fp,"-------------------+                  +\n");
    fprintf (fp,"+                  |   Instrument response, %5s ch %3s",
        current_station->station, current_channel->channel);
    fprintf (fp,"   |                  +\n");
    fprintf (fp,"+                  +--------------------");
    fprintf (fp,"-------------------+                  +\n");
    fprintf (fp,"\n");

    fprintf (fp,"Response type:                         Generic ");
	fprintf (fp,"(Corner Frequency/slope)\n");

	for (type56 = type56_head; type56 != NULL; type56 = type56->next)
	{
		fprintf (fp,"Stage sequence number:                 %d\n", type56->stage);
		fprintf (fp,"Response in units lookup:  %4d        ",
			type56->input_units_code);
		find_type34(fp,type56->input_units_code);
		fprintf (fp,"Response out units lookup:  %4d       ",
			type56->output_units_code);
		find_type34(fp,type56->output_units_code);
		fprintf (fp,"Number of corners:                     %d\n",
			type56->number_corners);
		fprintf (fp,"Corners:\n");
		fprintf (fp,"i, frequency, slope\n");
		for (i = 0; i < type56->number_corners; i++)
			fprintf (fp,"\t%3d % E % E\n", i, 
			type56->corner[i].frequency, type56->corner[i].slope);
		fprintf (fp,"\n");
	}
}

