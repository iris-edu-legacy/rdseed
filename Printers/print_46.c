/*===========================================================================*/
/* SEED reader     |            print_type46               |  station header */
/*===========================================================================*/
/*
	Name:		print_type46
	Purpose:	print the channel generic response table to standard output
	Usage:		void print_type46 ();
				print_type46 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type46_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type46_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type46 ()
{
	struct type46 *type46;							/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B046";						/* blockette id string */

	if (type46_head != NULL)
	{
    	fprintf(outputfile,"%s+                  +--------------------",com_strt);
    	fprintf(outputfile,"-------------------+                  +\n");
    	fprintf(outputfile,"%s+------------------|      Generic Response Dictionary",
				com_strt);
    	fprintf(outputfile,"      |------------------+\n");
    	fprintf(outputfile,"%s+                  +--------------------",com_strt);
    	fprintf(outputfile,"-------------------+                  +\n");
    	fprintf(outputfile,"%s\n",com_strt);
	
	
		for (type46 = type46_head; type46 != NULL; type46 = type46->next)
		{
			fprintf(outputfile,"Response Lookup Code:                  %d\n",
					blkt_id,fld_pref,3,type46->response_code);
			fprintf(outputfile,"Response name:                         %s\n",
					blkt_id,fld_pref,4,type46->name ? type46->name : "(null)");
			fprintf(outputfile,"Response in units lookup:  %4d        ",
					blkt_id,fld_pref,5,type46->input_units_code);
			find_type34(outputfile, type46->input_units_code);
			fprintf(outputfile,"Response out units lookup: %4d        ",
					blkt_id,fld_pref,6,type46->output_units_code);
			find_type34(outputfile, type46->output_units_code);
			fprintf(outputfile,"Number of corners:                     %d\n",
					blkt_id,fld_pref,7,type46->number_corners);
			fprintf(outputfile,"%sCorners:\n",com_strt);
			fprintf(outputfile,"%s  i, frequency,    slope\n",com_strt);
			for (i = 0; i < type46->number_corners; i++)
				fprintf(outputfile,"%s%s%2.2d-%2.2d  %3d % E % E\n",
						blkt_id,fld_pref,8,9,i, 
						type46->corner[i].frequency, type46->corner[i].slope);
			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
