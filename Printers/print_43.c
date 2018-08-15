/*===========================================================================*/
/* SEED reader     |              print_type43               |  station header */
/*===========================================================================*/
/*
	Name:		print_type43
	Purpose:	print channel poles and zeroes response table to standard 
				output
	Usage:		void print_type43 ();
				print_type43 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type43_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type43_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				06/14/90  Sue Schoch      added response type field
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type43( )
{
	struct type43 *type43;							/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B043";						/* blockette id string */

	if (type43_head != NULL)
	{

		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s+------------------|  Response (Poles & Zeros) Dictionary",
				com_strt);
		fprintf(outputfile,"  |------------------+\n");
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s\n",com_strt);
	
	
		for (type43 = type43_head; type43 != NULL; type43 = type43->next)
		{
			fprintf(outputfile,"%s%s%2.2d     Response lookup code:                  %d\n",
					blkt_id,fld_pref,3,type43->response_code);
			fprintf(outputfile,"%s%s%2.2d     Response name:                         %s\n",
					blkt_id,fld_pref,4,type43->name ? type43->name : "(null)");
			fprintf(outputfile,"%s%s%2.2d     Response type:                         %c\n",
					blkt_id,fld_pref,5,type43->response_type);
			fprintf(outputfile,"%s%s%2.2d     Response in units lookup:  %4d        ",
					blkt_id,fld_pref,6,type43->input_units_code);
			find_type34(outputfile, type43->input_units_code);
			fprintf(outputfile,"%s%s%2.2d     Response out units lookup: %4d        ",
					blkt_id,fld_pref,7,type43->output_units_code);
			find_type34(outputfile, type43->output_units_code);
			fprintf(outputfile,"%s%s%2.2d     AO normalization factor:               %G\n",
					blkt_id,fld_pref,8,type43->ao_norm);
			fprintf(outputfile,"%s%s%2.2d     Normalization frequency:               %G\n",
					blkt_id,fld_pref,9,type43->norm_freq);
			fprintf(outputfile,"%s%s%2.2d     Number of zeroes:                      %d\n",
					blkt_id,fld_pref,10,type43->number_zeroes);
			fprintf(outputfile,"%s%s%2.2d     Number of poles:                       %d\n",
					blkt_id,fld_pref,15,type43->number_poles);
			fprintf(outputfile,"%sComplex zeroes:\n",com_strt);
			fprintf(outputfile,"%s  i  real          imag          real_error    imag_error\n",
					com_strt);
			for (i = 0; i < type43->number_zeroes; i++)
				fprintf(outputfile,"%s%s%2.2d-%2.2d  %3d % E % E % E % E\n",
						blkt_id,fld_pref,11,14,i,
						type43->zero[i].real, 
						type43->zero[i].imag,
						type43->zero[i].real_error, 
						type43->zero[i].imag_error);
			fprintf(outputfile,"%sComplex poles:\n",com_strt);
			fprintf(outputfile,"%s  i  real          imag          real_error    imag_error\n",
					com_strt);
			for (i = 0; i < type43->number_poles; i++)
				fprintf(outputfile,"%s%s%2.2d-%2.2d  %3d % E % E % E % E\n",
						blkt_id,fld_pref,16,19,i,
						type43->pole[i].real, 
						type43->pole[i].imag,
						type43->pole[i].real_error, 
						type43->pole[i].imag_error);
			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
