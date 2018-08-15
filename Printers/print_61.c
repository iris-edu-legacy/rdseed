/*===========================================================================*/
/* SEED reader     |              print_type61                |  station header */
/*===========================================================================*/
/*
	Name:		print_type61
	Purpose:	print the channel coefficients response table to standard 
				output
	Usage:		void print_type61 ();
				print_type61 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type61_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type61_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Allen Nnace
	Revisions:	07/10/92 Allen Nance  adapted from print_54() for SEED version 2.2
                02/17/93  CL    nullpointer protect fprintfs
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type61 (fp,type61_head)
FILE *fp;
struct type61 *type61_head;
{
	struct type61 *type61;						/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B061";						/* blockette id string */

	fprintf (fp,"%s+                     +-----------------",com_strt);
	fprintf (fp,"---------------+                      +\n");
	fprintf (fp,"%s+                     |   FIR response, %5s ch %3s",com_strt,
		current_station->station, current_channel->channel);
	fprintf (fp,"   |                      +\n");
	fprintf (fp,"%s+                     +-----------------",com_strt);
	fprintf (fp,"---------------+                      +\n");
	fprintf (fp,"%s\n",com_strt);

	/* fprintf (fp,"%sResponse type:                         Coefficients\n",com_strt); */

	for (type61 = type61_head; type61 != NULL; type61 = type61->next)
	{
		fprintf (fp,"%s%s%2.2d     Stage sequence number:                 %d\n",
			blkt_id,fld_pref,3,type61->stage);

/* 		fprintf(fp,"%s%s%2.2d     Response name:                         %s\n", */
/* 			blkt_id,fld_pref,4,type61->name ? type61->name : "(null)"); */

		fprintf(fp,"%s%s%2.2d     Symmetry type:                         %c\n",
			blkt_id,fld_pref,5,type61->symmetry_code);

		fprintf (fp,"%s%s%2.2d     Response in units lookup:              ",
			blkt_id,fld_pref,6);

		find_type34(fp,type61->input_units_code);

		fprintf (fp,"%s%s%2.2d     Response out units lookup:             ",
			blkt_id,fld_pref,7);

		find_type34(fp,type61->output_units_code);

		fprintf (fp,"%s%s%2.2d     Number of numerators:                  %d\n",
			blkt_id,fld_pref,8,type61->number_numerators);

		if( type61->number_numerators != 0 )
		{
			fprintf (fp,"%sNumerator coefficients:\n",com_strt);

			fprintf (fp,"%s  i, coefficient\n",com_strt);

			for (i = 0; i < type61->number_numerators; i++)
				fprintf (fp,"%s%s%2.2d     %3d % E\n",blkt_id,fld_pref,9,i,
						 type61->numerator[i]);

		}

		fprintf (fp,"%s\n",com_strt);

	}
}

void old_print_type61 (fp,type61_head)
FILE *fp;
struct type61 *type61_head;
{
	struct type61 *type61;							/* looping vbl */
	int i;										/* counter */

	fprintf (fp,"+                  +--------------------");
	fprintf (fp,"-------------------+                  +\n");
	fprintf (fp,"+                  |   Instrument response, %5s ch %3s",
		current_station->station, current_channel->channel);
	fprintf (fp,"   |                  +\n");
	fprintf (fp,"+                  +--------------------");
	fprintf (fp,"-------------------+                  +\n");
	fprintf (fp,"\n");

	fprintf(fp, "\nBlockette 61\n\n");

	fprintf (fp,"Response type:                         Coefficients\n");

	for (type61 = type61_head; type61 != NULL; type61 = type61->next)
	{
		fprintf (fp,"Stage sequence number:                 %d\n",
			type61->stage);

		fprintf(fp,"Response name:                         %s\n",
			type61->name ? type61->name : "(null)");

		fprintf(fp,"Symmetry type:                         %c\n",
			type61->symmetry_code);

		fprintf (fp,"Response in units lookup:  %4d        ",
			type61->input_units_code);

		find_type34(fp,type61->input_units_code);

		fprintf (fp,"Response out units lookup:  %4d       ",
			type61->output_units_code);

		find_type34(fp,type61->output_units_code);

		fprintf (fp,"Number of numerators:                  %d\n",
			type61->number_numerators);

		if( type61->number_numerators != 0 )
		{
			fprintf (fp,"Numerator coefficients:\n");

			fprintf (fp,"  i, coefficient\n");

			for (i = 0; i < type61->number_numerators; i++)
				fprintf (fp,"%3d % E\n", i, type61->numerator[i]);

		}

		fprintf (fp,"\n");

	}
}
