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
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type44()
{
	struct type44 *type44;							/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B044";						/* blockette id string */

	if (type44_head != NULL)
	{

		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s+------------------|  Response (Coefficients) Dictionary",
				com_strt);
		fprintf(outputfile,"   |------------------+\n");
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s\n",com_strt);

	
		for (type44 = type44_head; type44 != NULL; type44 = type44->next)
		{
			fprintf(outputfile,"%s%s%2.2d     Response Lookup Code:                  %d\n",
					blkt_id,fld_pref,3,type44->response_code);
			fprintf(outputfile,"%s%s%2.2d     Response name:                         %s\n",
					blkt_id,fld_pref,4,type44->name ? type44->name : "(null)");
			fprintf(outputfile,"%s%s%2.2d     Response type:                         %c\n",
					blkt_id,fld_pref,5,type44->response_type);
			fprintf(outputfile,"%s%s%2.2d     Response in units lookup:  %4d        ",
					blkt_id,fld_pref,6,type44->input_units_code);
			find_type34(outputfile, type44->input_units_code);
			fprintf(outputfile,"%s%s%2.2d     Response out units lookup: %4d        ",
					blkt_id,fld_pref,7,type44->output_units_code);
			find_type34(outputfile, type44->output_units_code);
			fprintf(outputfile,"%s%s%2.2d     Number of numerators:                  %d\n",
					blkt_id,fld_pref,8,type44->number_numerators);
			fprintf(outputfile,"%s%s%2.2d     Number of denominators:                %d\n",
					blkt_id,fld_pref,11,type44->number_denominators);
			fprintf(outputfile,"%sNumerator coefficients:\n",com_strt);
			fprintf(outputfile,"%s  i, coefficient,  error\n",com_strt);
			for (i = 0; i < type44->number_numerators; i++)
				fprintf(outputfile,"%s%s%2.2d-%2.2d  %3d % E % E\n",
						blkt_id,fld_pref,9,10,i, 
						type44->numerator[i].coefficient,type44->numerator[i].error);
			if (type44->number_denominators != 0)
			{
				fprintf(outputfile,"%sDenominator coefficients:\n",com_strt);
				fprintf(outputfile,"%s  i, coefficient,  error\n",com_strt);
				for (i = 0; i < type44->number_denominators; i++)
					fprintf(outputfile,"%%s%s%2.2d-%2.2d  3d % E % E\n",
							blkt_id,fld_pref,12,13,i, 
							type44->denominator[i].coefficient,
							type44->denominator[i].error);
			}
			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
