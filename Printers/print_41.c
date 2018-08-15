/*===========================================================================*/
/* SEED reader     |            print_type41               |  station header */
/*===========================================================================*/
/*
	Name:		print_type41
	Purpose:	print the channel coefficients response table to standard 
				output
	Usage:		void print_type41 ();
				print_type41 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type41_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type41_head is the first member
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

void print_type41()
{
	struct type41 *type41;							/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B041";						/* blockette id string */

	if (type41_head != NULL)
	{

		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s+------------------|  Response (Coefficients) Dictionary",
				com_strt);
		fprintf(outputfile,"   |------------------+\n");
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s\n",com_strt);

	
		for (type41 = type41_head; type41 != NULL; type41 = type41->next)
		{
			fprintf(outputfile,"%s%s%2.2d     Response Lookup Code:                  %d\n",
					blkt_id,fld_pref,3,type41->response_code);
			fprintf(outputfile,"%s%s%2.2d     Response name:                         %s\n",
					blkt_id,fld_pref,4,type41->name ? type41->name : "(null)");
			fprintf(outputfile,"%s%s%2.2d     Symmetry type:                         %c\n",
					blkt_id,fld_pref,5,type41->symmetry_code);
			fprintf(outputfile,"%s%s%2.2d     Response in units lookup:  %4d        ",
					blkt_id,fld_pref,6,type41->input_units_code);
			find_type34(outputfile, type41->input_units_code);
			fprintf(outputfile,"%s%s%2.2d     Response out units lookup: %4d        ",
					blkt_id,fld_pref,7,type41->output_units_code);
			find_type34(outputfile, type41->output_units_code);
			fprintf(outputfile,"%s%s%2.2d     Number of numerators:                  %d\n",
					blkt_id,fld_pref,8,type41->number_numerators);
			fprintf(outputfile,"%sNumerator coefficients:\n",com_strt);
			fprintf(outputfile,"%s  i, coefficient\n",com_strt);
			for (i = 0; i < type41->number_numerators; i++)
				fprintf(outputfile,"%s%s%2.2d     %3d % E\n",blkt_id,fld_pref,9,i, 
						type41->numerator[i]);
			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
