/*===========================================================================*/
/* SEED reader     |            print_type42               |  station header */
/*===========================================================================*/
/*
	Name:		print_type42
	Purpose:	print the channel polynomial response table to standard 
				output
	Usage:		void print_type42 ();
				print_type42 ();
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

void print_type42()
{
	struct type42 *type42;			/* looping vbl */
	int i;					/* counter */
	char *blkt_id="B042";			/* blockette id string */

	if (type42_head != NULL)
	{

		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s+------------------|   Response (Polynomial) Dictionary ",
				com_strt);
		fprintf(outputfile,"   |------------------+\n");
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s\n",com_strt);

	
		for (type42 = type42_head; type42 != NULL; type42 = type42->next)
		{
			fprintf(outputfile,"%s%s%2.2d     Response Lookup Code:                  %d\n",
					blkt_id,fld_pref,3,type42->response_code);

			fprintf(outputfile,"%s%s%2.2d     Response name:                         %s\n",
					blkt_id,fld_pref,4,type42->name ? type42->name : "(null)");

			fprintf (outputfile, "%s%s%2.2d     Transfer function type:       ",blkt_id,fld_pref,5);
				switch (type42->transfer_fct_type)
				{
				case 'A' : fprintf(outputfile,"         A [Laplace Transform (Rad/sec)]\n"); break;
				case 'B' : fprintf(outputfile,"         B [Analog (Hz)]\n"); break;
				case 'C' : fprintf(outputfile,"         C [Composite]\n"); break;
				case 'D' : fprintf(outputfile,"         D [Digital (Z-transform)]\n"); break;
				case 'P' : fprintf(outputfile,"         P [Polynomial]\n"); break;
			/* 	default  : fprintf(outputfile,"         %s\n",type42->transfer_fct_type ? (char *)type42->transfer_fct_type : "(null)"); break;
		*/
				default  : fprintf(outputfile,"         %c\n",
						type42->transfer_fct_type); 
						break;
				}

			fprintf(outputfile,"%s%s%2.2d     Response in units lookup:  %4d        ",
					blkt_id,fld_pref,6,type42->input_units_code);
			find_type34(outputfile, type42->input_units_code);

			fprintf(outputfile,"%s%s%2.2d     Response out units lookup: %4d        ",
					blkt_id,fld_pref,7,type42->output_units_code);
			find_type34(outputfile, type42->output_units_code);

			fprintf(outputfile,"%s%s%2.2d     Polynomial Approximation Type:         ", blkt_id,fld_pref,8);
				switch (type42->poly_approx_type)
				{
				case 'M' : fprintf(outputfile,"M [MacLaurin]\n"); break;
				default  : fprintf(outputfile,"%c\n", type42->poly_approx_type); break;
				}

			fprintf(outputfile,"%s%s%2.2d     Valid Frequency Units:                 ", blkt_id,fld_pref,9);
				switch (type42->valid_freq_units)
				{
				case 'A' : fprintf(outputfile,"A [rad/sec]\n"); break;
				case 'B' : fprintf(outputfile,"B [Hz]\n"); break;
				default  : fprintf(outputfile,"%c\n",type42->valid_freq_units); break;
			}

			fprintf(outputfile,"%s%s%2.2d     Lower Valid Frequency Bound:           %G\n",
					blkt_id,fld_pref,10,type42->lower_valid_freq);

			fprintf(outputfile,"%s%s%2.2d     Upper Valid Frequency Bound:           %G\n",
					blkt_id,fld_pref,11,type42->upper_valid_freq);

			fprintf(outputfile,"%s%s%2.2d     Lower Bound of Approximation:          %G\n",
					blkt_id,fld_pref,12,type42->lower_bound_approx);

			fprintf(outputfile,"%s%s%2.2d     Upper Bound of Approximation:          %G\n",
					blkt_id,fld_pref,13,type42->upper_bound_approx);

			fprintf(outputfile,"%s%s%2.2d     Maximum Absolute Error:                %G\n",
					blkt_id,fld_pref,14,type42->max_abs_error);

			fprintf(outputfile,"%s%s%2.2d     Number of coefficients:                %d\n",
					blkt_id,fld_pref,15,type42->number_coefficients);

			fprintf(outputfile,"%sPolynomial coefficients:\n",com_strt);

			fprintf(outputfile,"%s  i, coefficient,  error\n",com_strt);
			for (i = 0; i < type42->number_coefficients; i++)
				fprintf(outputfile,"%s%s%2.2d-%2.2d  %3d % E % E\n",
						blkt_id,fld_pref,16,17,i, 
						type42->coefficient[i].coefficient,
						type42->coefficient[i].error);

			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
