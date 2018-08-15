/*===========================================================================*/
/* SEED reader     |             find_type41               |  station header */
/*===========================================================================*/
/*
	Name:		find_type41
	Purpose:	print the channel coefficients response table to standard 
				output
	Usage:		void find_type41 ();
				find_type41 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type41_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type41_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				06/14/90  Sue Schoch      added response type field
				02/16/94  CL changed the string printing to protect from NULL pointers
*/

#include "rdseed.h"
#include "resp_defs.h"

find_type41(fp, code)
FILE *fp;
int code;
{
	struct type41 *type41;						/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B041";						/* blockette id string */

	for (type41 = type41_head; type41 != NULL; type41 = type41->next)
	{
		if (type41->response_code == code)
		{
			if (fp == NULL) break;

			fprintf (fp,"%s+                     +-----------------",com_strt);
			fprintf (fp,"---------------+                      +\n");
			fprintf (fp,"%s+                     |   FIR response, %5s ch %3s",com_strt,
					 current_station->station, current_channel->channel);
			fprintf (fp,"   |                      +\n");
			fprintf (fp,"%s+                     +-----------------",com_strt);
			fprintf (fp,"---------------+                      +\n");
			fprintf (fp,"%s\n",com_strt);

			fprintf(fp,"%s%s%2.2d     Symmetry type:                         %c\n",
				blkt_id,fld_pref,5,type41->symmetry_code);

			fprintf(fp,"%s%s%2.2d     Response in units lookup:              ",
				blkt_id,fld_pref,6);

			find_type34(fp, type41->input_units_code);
			fprintf(fp,"%s%s%2.2d     Response out units lookup:             ",
				blkt_id,fld_pref,7);

			find_type34(fp, type41->output_units_code);

			fprintf(fp,"%s%s%2.2d     Number of numerators:                  %d\n",
				blkt_id,fld_pref,8,type41->number_numerators);

			fprintf(fp,"%sNumerator coefficients:\n",com_strt);

			fprintf(fp,"%s  i, coefficient\n",com_strt);

			for (i = 0; i < type41->number_numerators; i++)
				fprintf(fp,"%s%s%2.2d     %3d % E\n", 
								blkt_id,fld_pref,9,i, 
								type41->numerator[i]);

            fprintf(fp,"%s\n",com_strt);

			break;
		}
	}
	if (type41 == NULL) return(0); else return(1);
}

old_find_type41(fp, code)
FILE *fp;
int code;
{
	struct type41 *type41;							/* looping vbl */
	int i;										/* counter */

	for (type41 = type41_head; type41 != NULL; type41 = type41->next)
	{
		if (type41->response_code == code)
		{
			if (fp == NULL) break;

			fprintf (fp,"B041\n");
			fprintf(fp,"Response Lookup Code:                  %d\n",
				type41->response_code);

			fprintf(fp,"Response name:                         %s\n",
				type41->name ? type41->name : "(null)");

			fprintf(fp,"Symmetry type:                         %c\n",
				type41->symmetry_code);

			fprintf(fp,"Response in units lookup:  %4d        ",
				type41->input_units_code);

			find_type34(fp, type41->input_units_code);
			fprintf(fp,"Response out units lookup: %4d        ",
				type41->output_units_code);

			find_type34(fp, type41->output_units_code);

			fprintf(fp,"Number of numerators:                  %d\n",
				type41->number_numerators);

			fprintf(fp,"Numerator coefficients:\n");

			fprintf(fp,"  i, coefficient\n");

			for (i = 0; i < type41->number_numerators; i++)
				fprintf(fp,"%3d % E\n", 
								i, 
								type41->numerator[i]);

			fprintf(fp,"\n");

			break;
		}
	}
	if (type41 == NULL) return(0); else return(1);
}
