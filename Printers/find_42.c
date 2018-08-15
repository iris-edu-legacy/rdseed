/*===========================================================================*/
/* SEED reader     |             find_type42               |  station header */
/*===========================================================================*/
/*
	Name:		find_type42
	Purpose:	print the channel coefficients response table to standard 
				output
	Usage:		void find_type42 ();
				find_type42 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type42_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type42_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Stephane Zuzlewski
*/

#include "rdseed.h"
#include "resp_defs.h"

find_type42(fp, code)
FILE *fp;
int code;
{
	struct type42 *type42;						/* looping vbl */
	int i;								/* counter */
	char *blkt_id="B042";						/* blockette id string */

	for (type42 = type42_head; type42 != NULL; type42 = type42->next)
	{
		if (type42->response_code == code)
		{
			if (fp == NULL) break;

			fprintf (fp,"%s+              +------------------------",com_strt);
			fprintf (fp,"---------------+                      +\n");
			fprintf (fp,"%s+              |   Polynomial response, %5s ch %3s",com_strt,
					 current_station->station, current_channel->channel);
			fprintf (fp,"   |                      +\n");
			fprintf (fp,"%s+              +------------------------",com_strt);
			fprintf (fp,"---------------+                      +\n");
			fprintf (fp,"%s\n",com_strt);

			fprintf (fp, "%s%s%2.2d     Transfer function type:       ",blkt_id,fld_pref,5);
				switch (type42->transfer_fct_type)
				{
				case 'A' : fprintf(fp,"         A [Laplace Transform (Rad/sec)]\n"); break;
				case 'B' : fprintf(fp,"         B [Analog (Hz)]\n"); break;
				case 'C' : fprintf(fp,"         C [Composite]\n"); break;
				case 'D' : fprintf(fp,"         D [Digital (Z-transform)]\n"); break;
				case 'P' : fprintf(fp,"         P [Polynomial]\n"); break;
				/* default  : fprintf(fp,"         %s\n",type42->transfer_fct_type ? (char *)type42->transfer_fct_type : "(null)"); break;
				*/
					default  : fprintf(fp,"         %c\n",
						type42->transfer_fct_type); 
						break;
				}

			fprintf(fp,"%s%s%2.2d     Response in units lookup:              ",
					blkt_id,fld_pref,6);
			find_type34(fp, type42->input_units_code);

			fprintf(fp,"%s%s%2.2d     Response out units lookup:             ",
					blkt_id,fld_pref,7);
			find_type34(fp, type42->output_units_code);

			fprintf(fp,"%s%s%2.2d     Polynomial Approximation Type:         ", blkt_id,fld_pref,8);
				switch (type42->poly_approx_type)
				{
				case 'M' : fprintf(fp,"M [MacLaurin]\n"); break;
				default  : fprintf(fp,"%c\n", type42->poly_approx_type); break;
				}

			fprintf(fp,"%s%s%2.2d     Valid Frequency Units:                 ", blkt_id,fld_pref,9);
				switch (type42->valid_freq_units)
				{
				case 'A' : fprintf(fp,"A [rad/sec]\n"); break;
				case 'B' : fprintf(fp,"B [Hz]\n"); break;
				default  : fprintf(fp,"%c\n",type42->valid_freq_units); break;
			}

			fprintf(fp,"%s%s%2.2d     Lower Valid Frequency Bound:           %G\n",
					blkt_id,fld_pref,10,type42->lower_valid_freq);

			fprintf(fp,"%s%s%2.2d     Upper Valid Frequency Bound:           %G\n",
					blkt_id,fld_pref,11,type42->upper_valid_freq);

			fprintf(fp,"%s%s%2.2d     Lower Bound of Approximation:          %G\n",
					blkt_id,fld_pref,12,type42->lower_bound_approx);

			fprintf(fp,"%s%s%2.2d     Upper Bound of Approximation:          %G\n",
					blkt_id,fld_pref,13,type42->upper_bound_approx);

			fprintf(fp,"%s%s%2.2d     Maximum Absolute Error:                %G\n",
					blkt_id,fld_pref,14,type42->max_abs_error);

			fprintf(fp,"%s%s%2.2d     Number of coefficients:                %d\n",
					blkt_id,fld_pref,15,type42->number_coefficients);

			fprintf(fp,"%sPolynomial coefficients:\n",com_strt);

			fprintf(fp,"%s  i, coefficient,  error\n",com_strt);
			for (i = 0; i < type42->number_coefficients; i++)
				fprintf(fp,"%s%s%2.2d-%2.2d  %3d % E % E\n",
						blkt_id,fld_pref,16,17,i, 
						type42->coefficient[i].coefficient,
						type42->coefficient[i].error);


            fprintf(fp,"%s\n",com_strt);

			break;
		}
	}
	if (type42 == NULL) return(0); else return(1);
}

old_find_type42(fp, code)
FILE *fp;
int code;
{
	struct type42 *type42;					/* looping vbl */
	int i;							/* counter */

	for (type42 = type42_head; type42 != NULL; type42 = type42->next)
	{
		if (type42->response_code == code)
		{
			if (fp == NULL) break;

			fprintf (fp,"B042\n");
			fprintf(fp,"Response Lookup Code:                  %d\n",
				type42->response_code);

			fprintf(fp,"Response name:                         %s\n",
				type42->name ? type42->name : "(null)");

			fprintf (fp, "Transfer function type:       ");
				switch (type42->transfer_fct_type)
				{
				case 'A' : fprintf(fp,"         A [Laplace Transform (Rad/sec)]\n"); break;
				case 'B' : fprintf(fp,"         B [Analog (Hz)]\n"); break;
				case 'C' : fprintf(fp,"         C [Composite]\n"); break;
				case 'D' : fprintf(fp,"         D [Digital (Z-transform)]\n"); break;
				case 'P' : fprintf(fp,"         P [Polynomial]\n"); break;
				default  : fprintf(fp,"         %c\n",type42->transfer_fct_type); break;
				}

			fprintf (fp,"Response in units lookup:  %4d        ",
				type42->input_units_code);
			find_type34(fp,type42->input_units_code);

			fprintf (fp,"Response out units lookup:  %4d       ",
				type42->output_units_code);
			find_type34(fp,type42->output_units_code);

			fprintf(fp,"Polynomial Approximation Type:         ");
				switch (type42->poly_approx_type)
				{
				case 'M' : fprintf(fp,"M [MacLaurin]\n"); break;
				default  : fprintf(fp,"%c\n",type42->poly_approx_type); break;
				}

			fprintf(fp,"Valid Frequency Units:                 ");
				switch (type42->valid_freq_units)
				{
				case 'A' : fprintf(fp,"A [rad/sec]\n"); break;
				case 'B' : fprintf(fp,"B [Hz]\n"); break;
				default  : fprintf(fp,"%c\n",type42->valid_freq_units); break;
			}

			fprintf(fp,"Lower Valid Frequency Bound:            %G\n",
				type42->lower_valid_freq);

			fprintf(fp,"Upper Valid Frequency Bound:            %G\n",
				type42->upper_valid_freq);

			fprintf(fp,"Lower Bound of Approximation:           %G\n",
				type42->lower_bound_approx);

			fprintf(fp,"Upper Bound of Approximation:           %G\n",
				type42->upper_bound_approx);

			fprintf(fp,"Maximum Absolute Error:                 %G\n",
				type42->max_abs_error);

			fprintf (fp,"Number of coefficients:                %d\n",
				type42->number_coefficients);

			if( type42->number_coefficients != 0 )
			{
				fprintf (fp,"Polynomial coefficients:\n");

				fprintf (fp,"  i, coefficient, error\n");

				for (i = 0; i < type42->number_coefficients; i++)
					fprintf(fp,"%03d % E % E\n",
					i, 
					type42->coefficient[i].coefficient,
					type42->coefficient[i].error);
			}

			fprintf(fp,"\n");

			break;
		}
	}
	if (type42 == NULL) return(0); else return(1);
}
