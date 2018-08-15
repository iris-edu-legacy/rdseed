/*===========================================================================*/
/* SEED reader     |              print_type53               |  station header */
/*===========================================================================*/
/*
	Name:		print_type53
	Purpose:	print channel poles and zeroes response table to standard 
				output
	Usage:		void print_type53 ();
				print_type53 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type53_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type53_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
                02/17/93  CL    nullpointer protect fprintfs
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type53 (fp,type53_head)
FILE *fp;
struct type53 *type53_head;
{
	struct type53 *type53;						/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B053";						/* blockette id string */

	fprintf (fp,"%s+               +-----------------------",com_strt);
	fprintf (fp,"---------------------+                +\n");
	fprintf (fp,"%s+               |   Response (Poles & Zeros), %5s ch %3s",com_strt,
		current_station->station, current_channel->channel);
	fprintf (fp,"   |                +\n");
	fprintf (fp,"%s+               +-----------------------",com_strt);
	fprintf (fp,"---------------------+                +\n");
	fprintf (fp,"%s\n",com_strt);

	for (type53 = type53_head; type53 != NULL; type53 = type53->next)
	{
	    fprintf (fp, "%s%s%2.2d     Transfer function type:       ",blkt_id,fld_pref,3);
		switch (*(type53->transfer))
		{
			case 'A' : fprintf(fp,"         A [Laplace Transform (Rad/sec)]\n"); break;
			case 'B' : fprintf(fp,"         B [Analog (Hz)]\n"); break;
			case 'C' : fprintf(fp,"         C [Composite]\n"); break;
			case 'D' : fprintf(fp,"         D [Digital (Z-transform)]\n"); break;
			default  : fprintf(fp,"         %s\n",type53->transfer ? type53->transfer : "(null)"); break;
		}
		fprintf (fp,"%s%s%2.2d     Stage sequence number:                 %d\n",
			blkt_id,fld_pref,4,type53->stage);
		fprintf (fp,"%s%s%2.2d     Response in units lookup:              ",
			blkt_id,fld_pref,5);
		find_type34(fp,type53->input_units_code);
		fprintf (fp,"%s%s%2.2d     Response out units lookup:             ",
			blkt_id,fld_pref,6);
		find_type34(fp,type53->output_units_code);
		fprintf (fp,"%s%s%2.2d     A0 normalization factor:               %G\n",
			blkt_id,fld_pref,7,type53->ao_norm);
		fprintf (fp,"%s%s%2.2d     Normalization frequency:               %G\n",
			blkt_id,fld_pref,8,type53->norm_freq);
		fprintf (fp,"%s%s%2.2d     Number of zeroes:                      %d\n",
			blkt_id,fld_pref,9,type53->number_zeroes);
		fprintf (fp,"%s%s%2.2d     Number of poles:                       %d\n",
			blkt_id,fld_pref,14,type53->number_poles);
		fprintf (fp,"%sComplex zeroes:\n", com_strt);
		fprintf (fp,"%s  i  real          imag          real_error    imag_error\n", com_strt);
		for (i = 0; i < type53->number_zeroes; i++)
			fprintf (fp,"%s%s%2.2d-%2.2d  %3d % E % E % E % E\n", 
			blkt_id,fld_pref,10,13,i,
			type53->zero[i].real, 
			type53->zero[i].imag,
			type53->zero[i].real_error, 
			type53->zero[i].imag_error);
		fprintf (fp,"%sComplex poles:\n", com_strt);
		fprintf (fp,"%s  i  real          imag          real_error    imag_error\n", com_strt);
		for (i = 0; i < type53->number_poles; i++)
			fprintf (fp,"%s%s%2.2d-%2.2d  %3d % E % E % E % E\n", 
			blkt_id,fld_pref,15,18,i,
			type53->pole[i].real, 
			type53->pole[i].imag,
			type53->pole[i].real_error, 
			type53->pole[i].imag_error);
		fprintf (fp,"%s\n", com_strt);
	}
}

void old_print_type53 (fp,type53_head)
FILE *fp;
struct type53 *type53_head;
{
	struct type53 *type53;							/* looping vbl */
	int i;										/* counter */

	fprintf (fp,"+                  +--------------------");
	fprintf (fp,"-------------------+                  +\n");
	fprintf (fp,"+                  |   Instrument response, %5s ch %3s",
		current_station->station, current_channel->channel);
	fprintf (fp,"   |                  +\n");
	fprintf (fp,"+                  +--------------------");
	fprintf (fp,"-------------------+                  +\n");
	fprintf (fp,"\n");

	fprintf (fp,"Response type:                         Poles and zeroes\n");

	for (type53 = type53_head; type53 != NULL; type53 = type53->next)
	{
		switch (*(type53->transfer))
		{
			case 'A' : fprintf (fp,"Transfer function type:                 Laplace Transform (Rad/sec)\n"); break;
			case 'B' : fprintf (fp,"Transfer function type:                 Analog (Hz)\n"); break;
			case 'C' : fprintf (fp,"Transfer function type:                 Composite\n"); break;
			case 'D' : fprintf (fp,"Transfer function type:                 Digital (Z-transform)\n"); break;
			default  : fprintf (fp,"Transfer function type:                %s\n", 
									type53->transfer ? type53->transfer : "(null)"); break;
		}
		fprintf (fp,"Stage sequence number:                 %d\n",
			type53->stage);
		fprintf (fp,"Response in units lookup:  %4d        ",
			type53->input_units_code);
		find_type34(fp,type53->input_units_code);
		fprintf (fp,"Response out units lookup: %4d        ",
			type53->output_units_code);
		find_type34(fp,type53->output_units_code);
		fprintf (fp,"AO normalization factor:               %G\n",
			type53->ao_norm);
		fprintf (fp,"Normalization frequency:               %G\n",
			type53->norm_freq);
		fprintf (fp,"Number of zeroes:                      %d\n",
			type53->number_zeroes);
		fprintf (fp,"Number of poles:                       %d\n",
			type53->number_poles);
		fprintf (fp,"Complex zeroes:\n");
		fprintf (fp,"  i  real          imag          real_error    imag_error\n");
		for (i = 0; i < type53->number_zeroes; i++)
			fprintf (fp,"%3d % E % E % E % E\n", i, 
			type53->zero[i].real, 
			type53->zero[i].imag,
			type53->zero[i].real_error, 
			type53->zero[i].imag_error);
		fprintf (fp,"Complex poles:\n");
		fprintf (fp,"  i  real          imag          real_error    imag_error\n");
		for (i = 0; i < type53->number_poles; i++)
			fprintf (fp,"%3d % E % E % E % E\n", i, 
			type53->pole[i].real, 
			type53->pole[i].imag,
			type53->pole[i].real_error, 
			type53->pole[i].imag_error);
		fprintf (fp,"\n");
	}
}
