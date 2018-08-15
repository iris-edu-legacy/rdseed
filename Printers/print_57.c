/*===========================================================================*/
/* SEED reader     |              print_type57                |  station header */
/*===========================================================================*/
/*
	Name:		print_type57
	Purpose:	print the channel decimation blockette table to standard output
	Usage:		void print_type57 ();
				print_type57 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type57_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type57_head is the first member
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

void print_type57 (fp,type57_head)
FILE *fp;
struct type57 *type57_head;
{
	struct type57 *type57;						/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B057";						/* blockette id string */

    fprintf (fp,"%s+                      +----------------", com_strt);
    fprintf (fp,"--------------+                       +\n");
    fprintf (fp,"%s+                      |   Decimation, %5s ch %3s", com_strt,
        current_station->station, current_channel->channel);
    fprintf (fp,"   |                       +\n");
    fprintf (fp,"%s+                      +----------------", com_strt);
    fprintf (fp,"--------------+                       +\n");
    fprintf (fp,"%s\n", com_strt);

    /* fprintf (fp,"%sResponse type:                         Decimation\n", com_strt); */

	for (type57 = type57_head; type57 != NULL; type57 = type57->next)
	{
		fprintf (fp,"%s%s%2.2d     Stage sequence number:                 %d\n",
				 blkt_id,fld_pref,3,type57->stage);
		fprintf (fp,"%s%s%2.2d     Input sample rate:                     %4E\n",
				 blkt_id,fld_pref,4,type57->input_sample_rate);
		fprintf (fp,"%s%s%2.2d     Decimation factor:                     %d\n",
				 blkt_id,fld_pref,5,type57->decimation_factor);
		fprintf (fp,"%s%s%2.2d     Decimation offset:                     %d\n",
				 blkt_id,fld_pref,6,type57->decimation_offset);
		fprintf (fp,"%s%s%2.2d     Estimated delay (seconds):             %4E\n",
				 blkt_id,fld_pref,7,type57->delay);
		fprintf (fp,"%s%s%2.2d     Correction applied (seconds):          %4E\n%s\n",
				 blkt_id,fld_pref,8,type57->correction, com_strt);
	}
}

void old_print_type57 (fp,type57_head)
FILE *fp;
struct type57 *type57_head;
{
	struct type57 *type57;							/* looping vbl */
	int i;										/* counter */

    fprintf (fp,"+                  +--------------------");
    fprintf (fp,"-------------------+                  +\n");
    fprintf (fp,"+                  |   Instrument response, %5s ch %3s",
        current_station->station, current_channel->channel);
    fprintf (fp,"   |                  +\n");
    fprintf (fp,"+                  +--------------------");
    fprintf (fp,"-------------------+                  +\n");
    fprintf (fp,"\n");

    fprintf (fp,"Response type:                         Decimation\n");

	for (type57 = type57_head; type57 != NULL; type57 = type57->next)
	{
		fprintf (fp,"Stage sequence number:                 %d\n", type57->stage);
		fprintf (fp,"Input sample rate:                     %4E\n",
			type57->input_sample_rate);
		fprintf (fp,"Decimation factor:                     %d\n",
			type57->decimation_factor);
		fprintf (fp,"Decimation offset:                     %d\n",
			type57->decimation_offset);
		fprintf (fp,"Estimated delay (seconds):             %4E\n",
			type57->delay);
		fprintf (fp,"Correction applied (seconds):          %4E\n\n",
			type57->correction);
	}
}

