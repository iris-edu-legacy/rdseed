/*===========================================================================*/
/* SEED reader     |              print_type54                |  station header */
/*===========================================================================*/
/*
	Name:		print_type54
	Purpose:	print the channel coefficients response table to standard 
				output
	Usage:		void print_type54 ();
				print_type54 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type54_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type54_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				05/15/90  Sue Schoch      Don't print numerators if num = 0
                02/17/93  CL    nullpointer protect fprintfs
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type54 (fp,type54_head)
FILE *fp;
struct type54 *type54_head;
{
	struct type54 *type54;						/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B054";						/* blockette id string */

	fprintf (fp,"%s+               +-----------------------", com_strt);
	fprintf (fp,"--------------------+                 +\n");
	fprintf (fp,"%s+               |   Response (Coefficients), %5s ch %3s", com_strt,
		current_station->station, current_channel->channel);
	fprintf (fp,"   |                 +\n");
	fprintf (fp,"%s+               +-----------------------", com_strt);
	fprintf (fp,"--------------------+                 +\n");
	fprintf (fp,"%s\n", com_strt);

	for (type54 = type54_head; type54 != NULL; type54 = type54->next)
	{
		fprintf (fp,"%s%s%2.2d     Transfer function type:                %s\n",
			blkt_id,fld_pref,3,type54->transfer ? type54->transfer : "(null)");
		fprintf (fp,"%s%s%2.2d     Stage sequence number:                 %d\n",
			blkt_id,fld_pref,4,type54->stage);
		fprintf (fp,"%s%s%2.2d     Response in units lookup:              ",
			blkt_id,fld_pref,5);
		find_type34(fp,type54->input_units_code);
		fprintf (fp,"%s%s%2.2d     Response out units lookup:             ",
			blkt_id,fld_pref,6);
		find_type34(fp,type54->output_units_code);
		fprintf (fp,"%s%s%2.2d     Number of numerators:                  %d\n",
			blkt_id,fld_pref,7,type54->number_numerators);
		fprintf (fp,"%s%s%2.2d     Number of denominators:                %d\n",
			blkt_id,fld_pref,10,type54->number_denominators);
		if( type54->number_numerators != 0 )
		{
			fprintf (fp,"%sNumerator coefficients:\n",com_strt);
			fprintf (fp,"%s  i, coefficient,  error\n",com_strt);
			for (i = 0; i < type54->number_numerators; i++)
				fprintf (fp,"%s%s%2.2d-%2.2d  %3d % E % E\n", blkt_id,fld_pref,8,9,i, 
				type54->numerator[i].coefficient,type54->numerator[i].error);
			if (type54->number_denominators != 0)
			{
				fprintf (fp,"%sDenominator coefficients:\n",com_strt);
				fprintf (fp,"%s i, coefficient, error\n",com_strt);
				for (i = 0; i < type54->number_denominators; i++)
					fprintf (fp,"%s%s%2.2d-%2.2d  %3d % E % E\n", blkt_id,fld_pref,11,12,i, 
					type54->denominator[i].coefficient,type54->denominator[i].error);
			}
		}
		fprintf (fp,"%s\n",com_strt);
	}
}

void old_print_type54 (fp,type54_head)
FILE *fp;
struct type54 *type54_head;
{
	struct type54 *type54;							/* looping vbl */
	int i;										/* counter */

	fprintf (fp,"+                  +--------------------");
	fprintf (fp,"-------------------+                  +\n");
	fprintf (fp,"+                  |   Instrument response, %5s ch %3s",
		current_station->station, current_channel->channel);
	fprintf (fp,"   |                  +\n");
	fprintf (fp,"+                  +--------------------");
	fprintf (fp,"-------------------+                  +\n");
	fprintf (fp,"\n");

	fprintf (fp,"Response type:                         Coefficients\n");

	for (type54 = type54_head; type54 != NULL; type54 = type54->next)
	{
		fprintf (fp,"Transfer function type:                %s\n",
			type54->transfer ? type54->transfer : "(null)");
		fprintf (fp,"Stage sequence number:                 %d\n",
			type54->stage);
		fprintf (fp,"Response in units lookup:  %4d        ",
			type54->input_units_code);
		find_type34(fp,type54->input_units_code);
		fprintf (fp,"Response out units lookup:  %4d       ",
			type54->output_units_code);
		find_type34(fp,type54->output_units_code);
		fprintf (fp,"Number of numerators:                  %d\n",
			type54->number_numerators);
		fprintf (fp,"Number of denominators:                %d\n",
			type54->number_denominators);
		if( type54->number_numerators != 0 )
		{
			fprintf (fp,"Numerator coefficients:\n");
			fprintf (fp,"  i, coefficient,  error\n");
			for (i = 0; i < type54->number_numerators; i++)
				fprintf (fp,"%3d % E % E\n", i, 
				type54->numerator[i].coefficient, type54->numerator[i].error);
			if (type54->number_denominators != 0)
			{
				fprintf (fp,"Denominator coefficients:\n");
				fprintf (fp," i, coefficient, error\n");
				for (i = 0; i < type54->number_denominators; i++)
					fprintf (fp,"%3d % E % E\n", i, 
					type54->denominator[i].coefficient, type54->denominator[i].error);
			}
		}
		fprintf (fp,"\n");
	}
}
