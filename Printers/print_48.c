/*===========================================================================*/
/* SEED reader     |            print_type48               |  station header */
/*===========================================================================*/
/*
	Name:		print_type48
	Purpose:	print the channel sensitivity table to standard output
	Usage:		void print_type48 ();
				print_type48 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type48_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type48_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Sue Schoch
	Revisions:	06/05/90  Sue Schoch      Initial preliminary release in 2.0
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type48 ()
{
	struct type48 *type48;							/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B048";						/* blockette id string */

	if (type48_head != NULL)
	{
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s+------------------|  Channel sensitivity/gain Dictionary",
				com_strt);
		fprintf(outputfile,"  |------------------+\n");
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s\n",com_strt);
	
		for (type48 = type48_head; type48 != NULL; type48 = type48->next)
		{
			fprintf(outputfile,"%s%s%2.2d     Response Lookup Code:                  %d\n",
					blkt_id,fld_pref,3,type48->response_code);
			fprintf(outputfile,"%s%s%2.2d     Response name:                         %s\n",
					blkt_id,fld_pref,4,type48->name ? type48->name : "(null)");
			fprintf(outputfile,"%s%s%2.2d     Sensitivity:                           %E\n",
					blkt_id,fld_pref,5,type48->sensitivity);
			fprintf(outputfile,"%s%s%2.2d     Frequency of sensitivity:              %E\n",
					blkt_id,fld_pref,6,type48->frequency);
			fprintf(outputfile,"%s%s%2.2d     Number of calibrations:                %d\n",
					blkt_id,fld_pref,7,type48->number_calibrations);
			if (type48->number_calibrations != 0)
			{
				fprintf(outputfile,"%sCalibrations:\n",com_strt);
				fprintf(outputfile,"%s  i, sensitivity,  frequency,   time of calibration\n",
						com_strt);
				for (i = 0; i < type48->number_calibrations; i++)
					fprintf(outputfile,"%s%s%2.2d-%2.2d  %3d % E % E %s\n",
							blkt_id,fld_pref,9,10,i, 
							type48->calibration[i].sensitivity, 
							type48->calibration[i].frequency,
							type48->calibration[i].time ? type48->calibration[i].time : "(null)");
			}
			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
