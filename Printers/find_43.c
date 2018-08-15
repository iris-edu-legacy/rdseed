/*==========================================================================*/
/* SEED reader     |            find_type43               |  station header */
/*==========================================================================*/
/*
	Name:		find_type43
	Purpose:	print channel poles and zeroes response table to standard 
				output
	Usage:		void find_type43 ();
				find_type43 ();
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
	Revisions:
				02/16/94	CL changed string formated output to protect against null
							pointers
*/

#include "rdseed.h"
#include "resp_defs.h"

int find_type43(fp, code)
FILE *fp;
int code;
{
	struct type43 *type43;					    /* looping vbl */
	int i;										/* counter */
	char *blkt_id="B043";						/* blockette id string */

	for (type43 = type43_head; type43 != NULL; type43 = type43->next)
	{
		if (type43->response_code == code)
		{
			if (fp == NULL) break;

			fprintf (fp,"%s+               +-----------------------",com_strt);
			fprintf (fp,"---------------------+                +\n");
			fprintf (fp,"%s+               |   Response (Poles & Zeros), %5s ch %3s",com_strt,
					 current_station->station, current_channel->channel);
			fprintf (fp,"   |                +\n");
			fprintf (fp,"%s+               +-----------------------",com_strt);
			fprintf (fp,"---------------------+                +\n");
			fprintf (fp,"%s\n",com_strt);

			fprintf (fp,"%s%s%2.2d     Response type:                ",blkt_id,fld_pref,5);
			switch (type43->response_type)
			{
			  case 'A' : fprintf(fp,"         A [Laplace Transform (Rad/sec)]\n"); break;
			  case 'B' : fprintf(fp,"         B [Analog (Hz)]\n"); break;
			  case 'C' : fprintf(fp,"         C [Composite]\n"); break;
			  case 'D' : fprintf(fp,"         D [Digital (Z-transform)]\n"); break;
			  default  : fprintf(fp,"         %c\n",type43->response_type ? type43->response_type : ' '); break;
			}
			fprintf (fp,"%s%s%2.2d     Response in units lookup:              ",
				blkt_id,fld_pref,6);

			find_type34(fp,type43->input_units_code);

			fprintf (fp,"%s%s%2.2d     Response out units lookup:             ",
				blkt_id,fld_pref,7);

			find_type34(fp,type43->output_units_code);

			fprintf (fp,"%s%s%2.2d     A0 normalization factor:               %G\n",
				blkt_id,fld_pref,8,type43->ao_norm);

			fprintf (fp,"%s%s%2.2d     Normalization frequency:               %G\n",
				blkt_id,fld_pref,9,type43->norm_freq);

			fprintf (fp,"%s%s%2.2d     Number of zeroes:                      %d\n",
				blkt_id,fld_pref,10,type43->number_zeroes);

			fprintf (fp,"%s%s%2.2d     Number of poles:                       %d\n",
				blkt_id,fld_pref,15,type43->number_poles);

			fprintf (fp,"%sComplex zeroes:\n",com_strt);

			fprintf (fp,"%s  i  real          imag          real_error    imag_error\n",
					 com_strt);
			for (i = 0; i < type43->number_zeroes; i++)
				fprintf (fp,"%s%s%2.2d-%2.2d  %3d % E % E % E % E\n",
						 blkt_id,fld_pref,11,14,i, 
						 type43->zero[i].real, 
						 type43->zero[i].imag,
						 type43->zero[i].real_error, 
						 type43->zero[i].imag_error);

			fprintf (fp,"%sComplex poles:\n",com_strt);

			fprintf (fp,"%s  i  real          imag          real_error    imag_error\n",
					 com_strt);

			for (i = 0; i < type43->number_poles; i++)
				fprintf (fp,"%s%s%2.2d-%2.2d  %3d % E % E % E % E\n",
						 blkt_id,fld_pref,16,19,i, 
						 type43->pole[i].real, 
						 type43->pole[i].imag,
						 type43->pole[i].real_error, 
						 type43->pole[i].imag_error);
			fprintf (fp,"%s\n",com_strt);
			break;
		}
	}
	if (type43 == NULL) return(0); else return(1);
}

int old_find_type43(fp, code)
FILE *fp;
int code;
{
	struct type43 *type43;					/* looping vbl */
	int i;										/* counter */

	for (type43 = type43_head; type43 != NULL; type43 = type43->next)
	{
		if (type43->response_code == code)
		{
			if (fp == NULL) break;

			fprintf (fp,"B043\n");
			fprintf (fp,"Response lookup code:                  %d\n",
				type43->response_code);

			fprintf (fp,"Response name:                         %s\n",
						type43->name ? type43->name : "(null)");

			fprintf (fp,"Response type:                         %c\n",
				type43->response_type);

			fprintf (fp,"Response in units lookup: %4d         ",
				type43->input_units_code);

			find_type34(fp,type43->input_units_code);

			fprintf (fp,"Response out units lookup: %4d        ",
				type43->output_units_code);

			find_type34(fp,type43->output_units_code);

			fprintf (fp,"AO normalization factor:               %G\n",
				type43->ao_norm);

			fprintf (fp,"Normalization frequency:               %G\n",
				type43->norm_freq);

			fprintf (fp,"Number of zeroes:                      %d\n",
				type43->number_zeroes);

			fprintf (fp,"Number of poles:                       %d\n",
				type43->number_poles);

			fprintf (fp,"Complex zeroes:\n");

			fprintf (fp,"  i  real          imag          real_error    imag_error\n");
			for (i = 0; i < type43->number_zeroes; i++)
				fprintf (fp,"%3d % E % E % E % E\n", i, 
				type43->zero[i].real, 
				type43->zero[i].imag,
				type43->zero[i].real_error, 
				type43->zero[i].imag_error);

			fprintf (fp,"Complex poles:\n");

			fprintf (fp,"  i  real          imag          real_error    imag_error\n");

			for (i = 0; i < type43->number_poles; i++)
				fprintf (fp,"%3d % E % E % E % E\n", i, 
				type43->pole[i].real, 
				type43->pole[i].imag,
				type43->pole[i].real_error, 
				type43->pole[i].imag_error);
			fprintf (fp,"\n");
			break;
		}
	}
	if (type43 == NULL) return(0); else return(1);
}
