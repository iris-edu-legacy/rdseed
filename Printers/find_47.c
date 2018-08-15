/*===========================================================================*/
/* SEED reader     |             print_type47              |  station header */
/*===========================================================================*/
/*
	Name:		print_type47
	Purpose:	print the channel generic response table to standard output
	Usage:		void print_type47 ();
				print_type47 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type47_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type47_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill
				05/09/90  Sue Schoch      Initial release 2.0
                02/17/93  CL    nullpointer protect fprintfs
*/

#include "rdseed.h"
#include "resp_defs.h"

int find_type47(fp, code)
FILE *fp;
int code;
{
	struct type47 *type47;						/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B047";						/* blockette id string */

	for (type47 = type47_head; type47 != NULL; type47 = type47->next)
	{
		if (type47->response_code == code)
		{
			if (fp == NULL) break;

			fprintf (fp,"%s+                      +----------------", com_strt);
			fprintf (fp,"--------------+                       +\n");
			fprintf (fp,"%s+                      |   Decimation, %5s ch %3s", com_strt,
					 current_station->station, current_channel->channel);
			fprintf (fp,"   |                       +\n");
			fprintf (fp,"%s+                      +----------------", com_strt);
			fprintf (fp,"--------------+                       +\n");
			fprintf (fp,"%s\n", com_strt);

			fprintf (fp,"%s%s%2.2d     Response input sample rate:            %E\n",
				blkt_id,fld_pref,5,type47->input_sample_rate );
			fprintf (fp,"%s%s%2.2d     Response decimation factor:            %d\n",
				blkt_id,fld_pref,6,type47->decimation_factor );
			fprintf (fp,"%s%s%2.2d     Response decimation offset:            %d\n",
				blkt_id,fld_pref,7,type47->decimation_offset );
			fprintf (fp,"%s%s%2.2d     Response delay:                        %E\n",
				blkt_id,fld_pref,8,type47->delay );
			fprintf (fp,"%s%s%2.2d     Response correction:                   %E\n",
				blkt_id,fld_pref,9,type47->correction );
			fprintf (fp,"%s\n",com_strt);
			break;
		}
	}
	if (type47 == NULL) return(0); else return(1);
}

int old_find_type47(fp, code)
FILE *fp;
int code;
{
	struct type47 *type47;							/* looping vbl */
	int i;										/* counter */

	for (type47 = type47_head; type47 != NULL; type47 = type47->next)
	{
		if (type47->response_code == code)
		{
			if (fp == NULL) break;

			fprintf (fp,"B047\n");
			fprintf (fp,"Response Lookup Code:                  %d\n",
				type47->response_code);
			fprintf (fp,"Response name:                         %s\n",
				type47->name ? type47->name : "(null)");
			fprintf (fp,"Response input sample rate:            %E\n",
				type47->input_sample_rate );
			fprintf (fp,"Response decimation factor:            %d\n",
				type47->decimation_factor );
			fprintf (fp,"Response decimation offset:            %d\n",
				type47->decimation_offset );
			fprintf (fp,"Response delay:                        %E\n",
				type47->delay );
			fprintf (fp,"Response correction:                   %E\n",
				type47->correction );
			fprintf (fp,"\n");
			break;
		}
	}
	if (type47 == NULL) return(0); else return(1);
}
