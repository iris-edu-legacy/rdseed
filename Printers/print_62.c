/*===========================================================================*/
/* SEED reader     |              print_type62                |  station header */
/*===========================================================================*/
/*
	Name:		print_type62
	Purpose:	print the channel coefficients response table to standard 
				output
	Usage:		void print_type62 ();
				print_type62 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type62_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type62_head is the first member
			of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Stephane Zuzlewski
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type62 (fp,type62_head)
FILE *fp;
struct type62 *type62_head;
{
	struct type62 *type62;				/* looping vbl */
	int i;						/* counter */
	char *blkt_id="B062";				/* blockette id string */

	fprintf (fp,"%s+              +------------------------",com_strt);
	fprintf (fp,"---------------+                      +\n");
	fprintf (fp,"%s+              |   Polynomial response, %5s ch %3s",com_strt,
		current_station->station, current_channel->channel);
	fprintf (fp,"   |                      +\n");
	fprintf (fp,"%s+              +------------------------",com_strt);
	fprintf (fp,"---------------+                      +\n");
	fprintf (fp,"%s\n",com_strt);


	for (type62 = type62_head; type62 != NULL; type62 = type62->next)
	{
		fprintf (fp, "%s%s%2.2d     Transfer function type:       ",blkt_id,fld_pref,3);
			switch (type62->transfer_fct_type)
			{
			case 'A' : fprintf(fp,"         A [Laplace Transform (Rad/sec)]\n"); break;
			case 'B' : fprintf(fp,"         B [Analog (Hz)]\n"); break;
			case 'C' : fprintf(fp,"         C [Composite]\n"); break;
			case 'D' : fprintf(fp,"         D [Digital (Z-transform)]\n"); break;
			case 'P' : fprintf(fp,"         P [Polynomial]\n"); break;
			default  : fprintf(fp,"         Error! Invalid transfer function type: %c\n", type62->transfer_fct_type); break;
			}

		fprintf (fp,"%s%s%2.2d     Stage sequence number:                 %d\n",
			blkt_id,fld_pref,4,type62->stage);

		fprintf(fp,"%s%s%2.2d     Response in units lookup:              ",
			blkt_id,fld_pref,5);
		find_type34(fp, type62->input_units_code);

		fprintf(fp,"%s%s%2.2d     Response out units lookup:             ",
			blkt_id,fld_pref,6);
		find_type34(fp, type62->output_units_code);

		fprintf(fp,"%s%s%2.2d     Polynomial Approximation Type:         ", blkt_id,fld_pref,7);
			switch (type62->poly_approx_type)
			{
			case 'M' : fprintf(fp,"M [MacLaurin]\n"); break;
			default  : fprintf(fp,"Error! Invalid polynomial approximation type: %c\n", type62->poly_approx_type); break;
			}

		fprintf(fp,"%s%s%2.2d     Valid Frequency Units:                 ", blkt_id,fld_pref,8);
			switch (type62->valid_freq_units)
			{
			case 'A' : fprintf(fp,"A [rad/sec]\n"); break;
			case 'B' : fprintf(fp,"B [Hz]\n"); break;
			default  : fprintf(fp,"Error! Invalid Frequency Unit: %c\n", type62->valid_freq_units); break;
			}

		fprintf(fp,"%s%s%2.2d     Lower Valid Frequency Bound:           %G\n",
			blkt_id,fld_pref,9,type62->lower_valid_freq);

		fprintf(fp,"%s%s%2.2d     Upper Valid Frequency Bound:           %G\n",
			blkt_id,fld_pref,10,type62->upper_valid_freq);

		fprintf(fp,"%s%s%2.2d     Lower Bound of Approximation:          %G\n",
			blkt_id,fld_pref,11,type62->lower_bound_approx);

		fprintf(fp,"%s%s%2.2d     Upper Bound of Approximation:          %G\n",
			blkt_id,fld_pref,12,type62->upper_bound_approx);

		fprintf(fp,"%s%s%2.2d     Maximum Absolute Error:                %G\n",
			blkt_id,fld_pref,13,type62->max_abs_error);

		fprintf(fp,"%s%s%2.2d     Number of coefficients:                %d\n",
			blkt_id,fld_pref,14,type62->number_coefficients);

		fprintf(fp,"%sPolynomial coefficients:\n",com_strt);

		fprintf(fp,"%s  i, coefficient,  error\n",com_strt);
		for (i = 0; i < type62->number_coefficients; i++)
			fprintf(fp,"%s%s%2.2d-%2.2d  %3d % E % E\n",
				blkt_id,fld_pref,15,16,i, 
				type62->coefficient[i].coefficient,
				type62->coefficient[i].error);

		fprintf (fp,"%s\n",com_strt);

	}
}

void old_print_type62 (fp,type62_head)
FILE *fp;
struct type62 *type62_head;
{
	struct type62 *type62;					/* looping vbl */
	int i;							/* counter */

	fprintf (fp,"+                  +--------------------");
	fprintf (fp,"-------------------+                  +\n");
	fprintf (fp,"+                  |   Instrument response, %5s ch %3s",
		current_station->station, current_channel->channel);
	fprintf (fp,"   |                  +\n");
	fprintf (fp,"+                  +--------------------");
	fprintf (fp,"-------------------+                  +\n");
	fprintf (fp,"\n");

	fprintf(fp, "\nBlockette 62\n\n");

	fprintf (fp,"Response type:                         Coefficients\n");

	for (type62 = type62_head; type62 != NULL; type62 = type62->next)
	{
		fprintf (fp,"Transfer function type:               \n");
			switch (type62->transfer_fct_type)
			{
			case 'A' : fprintf(fp,"         A [Laplace Transform (Rad/sec)]\n"); break;
			case 'B' : fprintf(fp,"         B [Analog (Hz)]\n"); break;
			case 'C' : fprintf(fp,"         C [Composite]\n"); break;
			case 'D' : fprintf(fp,"         D [Digital (Z-transform)]\n"); break;
			case 'P' : fprintf(fp,"         P [Polynomial]\n"); break;
			default  : fprintf(fp,"         Error! Invalid transfer function type: %c\n",type62->transfer_fct_type); break;
			}

		fprintf (fp,"Stage sequence number:                %d\n",
			type62->stage);

		fprintf (fp,"Response in units lookup:  %4d        ",
			type62->input_units_code);
		find_type34(fp,type62->input_units_code);

		fprintf (fp,"Response out units lookup:  %4d       ",
			type62->output_units_code);
		find_type34(fp,type62->output_units_code);

		fprintf(fp,"Polynomial Approximation Type:         \n");
			switch (type62->poly_approx_type)
			{
			case 'M' : fprintf(fp,"M [MacLaurin]\n"); break;
			default  : fprintf(fp,"Error! Invalid polynomial approximation type: %c\n", type62->poly_approx_type); break;
			}

		fprintf(fp,"Valid Frequency Units:                 \n");
			switch (type62->valid_freq_units)
			{
			case 'A' : fprintf(fp,"A [rad/sec]\n"); break;
			case 'B' : fprintf(fp,"B [Hz]\n"); break;
			default  : fprintf(fp,"Error! Invalid frequency units: %c\n", type62->valid_freq_units); break;
			}

		fprintf(fp,"Lower Valid Frequency Bound:            %G\n",
			type62->lower_valid_freq);

		fprintf(fp,"Upper Valid Frequency Bound:            %G\n",
			type62->upper_valid_freq);

		fprintf(fp,"Lower Bound of Approximation:           %G\n",
			type62->lower_bound_approx);

		fprintf(fp,"Upper Bound of Approximation:           %G\n",
			type62->upper_bound_approx);

		fprintf(fp,"Maximum Absolute Error:                 %G\n",
			type62->max_abs_error);

		fprintf (fp,"Number of coefficients:                %d\n",
			type62->number_coefficients);

		if( type62->number_coefficients != 0 )
		{
			fprintf (fp,"Polynomial coefficients:\n");

			fprintf (fp,"  i, coefficient, error\n");

			for (i = 0; i < type62->number_coefficients; i++)
				fprintf(fp,"3d % E % E\n",
				i, 
				type62->coefficient[i].coefficient,
				type62->coefficient[i].error);
		}

		fprintf (fp,"\n");

	}
}
