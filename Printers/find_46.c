/*===========================================================================*/
/* SEED reader     |            print_type46               |  station header */
/*===========================================================================*/
/*
	Name:		print_type46
	Purpose:	print the channel generic response table to standard output
	Usage:		void print_type46 ();
				print_type46 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type46_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type46_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				02/16/94  CL made it null-pointer protected
*/

#include "rdseed.h"
#include "resp_defs.h"

int find_type46(fp, code)
FILE *fp;
int code;
{
	struct type46 *type46;						/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B046";						/* blockette id string */

	for (type46 = type46_head; type46 != NULL; type46 = type46->next)
	{
		if (type46->response_code == code)
		{
		    fprintf (fp,"%s+                   +-------------------",com_strt);
			fprintf (fp,"-----------------+                    +\n");
		    fprintf (fp,"%s+                   |   Generic response, %5s ch %3s",com_strt,
					 current_station->station, current_channel->channel);
			fprintf (fp,"   |                    +\n");
			fprintf (fp,"%s+                   +-------------------",com_strt);
			fprintf (fp,"-----------------+                    +\n");
			fprintf (fp,"%s\n",com_strt);

			fprintf (fp,"%s%s%2.2d     Response in units lookup:              ",
				blkt_id,fld_pref,5);

			find_type34(fp,type46->input_units_code);

			fprintf (fp,"%s%s%2.2d     Response out units lookup:             ",
				blkt_id,fld_pref,6);
			find_type34(fp,type46->output_units_code);

			fprintf (fp,"%s%s%2.2d     Number of corners:                     %d\n",
				blkt_id,fld_pref,7,type46->number_corners);
			fprintf (fp,"%sCorners:\n",com_strt);
			fprintf (fp,"%s  i, frequency,    slope\n",com_strt);
			for (i = 0; i < type46->number_corners; i++)
				fprintf (fp,"%s%s%2.2d-%2.2d  %3d % E % E\n", 
						 blkt_id,fld_pref,8,9,i,
						 type46->corner[i].frequency,type46->corner[i].slope);
			fprintf (fp,"%s\n",com_strt);
			break;
		}
	}
	if (type46 == NULL) return(0); else return(1);
}

int old_find_type46(fp, code)
FILE *fp;
int code;
{
	struct type46 *type46;							/* looping vbl */
	int i;										/* counter */

	for (type46 = type46_head; type46 != NULL; type46 = type46->next)
	{
		if (type46->response_code == code)
		{
 			fprintf (fp,"B046\n");
			fprintf (fp,"Response Lookup Code:                  %d\n",
				type46->response_code);

			fprintf (fp,"Response name:                         %s\n",
				type46->name ? type46->name : "(null)");

			fprintf (fp,"Response in units lookup: %4d         ",
				type46->input_units_code);

			find_type34(fp,type46->input_units_code);

			fprintf (fp,"Response out units lookup: %4d        ",
				type46->output_units_code);
			find_type34(fp,type46->output_units_code);

			fprintf (fp,"Number of corners:                     %d\n",
				type46->number_corners);
			fprintf (fp,"Corners:\n");
			fprintf (fp,"  i, frequency,    slope\n");
			for (i = 0; i < type46->number_corners; i++)
				fprintf (fp,"%3d % E % E\n", i, 
				type46->corner[i].frequency, type46->corner[i].slope);
			fprintf (fp,"\n");
			break;
		}
	}
	if (type46 == NULL) return(0); else return(1);
}
