/*===========================================================================*/
/* SEED reader     |            print_type44               |  station header */
/*===========================================================================*/
/*
	Name:		print_type44
	Purpose:	print the channel coefficients response table to standard 
				output
	Usage:		void print_type44 ();
				print_type44 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type44_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type44_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				06/14/90  Sue Schoch      added response type field
				02/16/94  CL made printfs null-pointer protected
*/

#include "rdseed.h"
#include "resp_defs.h"

int find_type44(fp, code)
FILE *fp;
int code;
{
	struct type44 *type44;							/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B044";						/* blockette id string */

	for (type44 = type44_head; type44 != NULL; type44 = type44->next)
	{
		if (type44->response_code == code)
		{
			if (fp == NULL) break;

			fprintf (fp,"%s+               +-----------------------", com_strt);
			fprintf (fp,"--------------------+                 +\n");
			fprintf (fp,"%s+               |   Response (Coefficients), %5s ch %3s", com_strt,
					 current_station->station, current_channel->channel);
			fprintf (fp,"   |                 +\n");
			fprintf (fp,"%s+               +-----------------------", com_strt);
			fprintf (fp,"--------------------+                 +\n");
			fprintf (fp,"%s\n", com_strt);

			fprintf (fp,"%s%s%2.2d     Response type:                         %c\n",
				blkt_id,fld_pref,5,type44->response_type);

			fprintf (fp,"%s%s%2.2d     Response in units lookup:              ",
				blkt_id,fld_pref,6);

			find_type34(fp,type44->input_units_code);

			fprintf (fp,"%s%s%2.2d     Response out units lookup:             ",
				blkt_id,fld_pref,7);
			find_type34(fp,type44->output_units_code);

			fprintf (fp,"%s%s%2.2d     Number of numerators:                  %d\n",
				blkt_id,fld_pref,8,type44->number_numerators);

			fprintf (fp,"%s%s%2.2d     Number of denominators:                %d\n",
				blkt_id,fld_pref,11,type44->number_denominators);

			fprintf (fp,"%sNumerator coefficients:\n",com_strt);
			fprintf (fp,"%s  i, coefficient,  error\n",com_strt);
			for (i = 0; i < type44->number_numerators; i++)
				fprintf (fp,"%s%s%2.2d-%2.2d  %3d % E % E\n",blkt_id,fld_pref,9,10,i, 
						 type44->numerator[i].coefficient,type44->numerator[i].error);

			if (type44->number_denominators != 0)
			{
				fprintf (fp,"%sDenominator coefficients:\n",com_strt);
				fprintf (fp,"%s i, coefficient, error\n",com_strt);
				for (i = 0; i < type44->number_denominators; i++)
					fprintf (fp,"%s%s%2.2d-%2.2d  %3d % E % E\n",blkt_id,fld_pref,12,13,
							 i,type44->denominator[i].coefficient,
							 type44->denominator[i].error);
			}
			fprintf (fp,"%s\n",com_strt);
			break;
		}
	}
	if (type44 == NULL) return(0); else return(1);
}

int old_find_type44(fp, code)
FILE *fp;
int code;
{
	struct type44 *type44;							/* looping vbl */
	int i;										/* counter */

	for (type44 = type44_head; type44 != NULL; type44 = type44->next)
	{
		if (type44->response_code == code)
		{
			if (fp == NULL) break;

			fprintf (fp,"B044\n");
			fprintf (fp,"Response Lookup Code:                  %d\n",
				type44->response_code);

			fprintf (fp,"Response name:                         %s\n",
				type44->name ? type44->name : "(null)");

			fprintf (fp,"Response type:                         %c\n",
				type44->response_type);

			fprintf (fp,"Response in units lookup: %4d         ",
				type44->input_units_code);

			find_type34(fp,type44->input_units_code);

			fprintf (fp,"Response out units lookup: %4d        ",
				type44->output_units_code);
			find_type34(fp,type44->output_units_code);

			fprintf (fp,"Number of numerators:                  %d\n",
				type44->number_numerators);

			fprintf (fp,"Number of denominators:                %d\n",
				type44->number_denominators);

			fprintf (fp,"Numerator coefficients:\n");
			fprintf (fp,"  i, coefficient,  error\n");
			for (i = 0; i < type44->number_numerators; i++)
				fprintf (fp,"%3d % E % E\n", i, 
				type44->numerator[i].coefficient, type44->numerator[i].error);

			if (type44->number_denominators != 0)
			{
				fprintf (fp,"Denominator coefficients:\n");
				fprintf (fp,"  i, coefficient,  error\n");
				for (i = 0; i < type44->number_denominators; i++)
					fprintf (fp,"%3d % E % E\n", i, 
					type44->denominator[i].coefficient, type44->denominator[i].error);
			}
			fprintf (fp,"\n");
			break;
		}
	}
	if (type44 == NULL) return(0); else return(1);
}
