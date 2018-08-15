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
	Revisions:
                02/17/93  CL    nullpointer protect fprintfs

*/

#include "rdseed.h"
#include "resp_defs.h"

int find_type48(fp, code)
FILE *fp;
int code;
{
	struct type48 *type48;						/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B048";						/* blockette id string */

	for (type48 = type48_head; type48 != NULL; type48 = type48->next)
	{
	
		if (type48->response_code == code)
		{
			if (fp == NULL) break;

			fprintf (fp,"%s+                  +--------------------", com_strt);
			fprintf (fp,"-------------------+                  +\n");
			fprintf (fp,"%s+                  |   Channel Sensitivity, %5s ch %3s   ",
					 com_strt,current_station->station,current_channel->channel);
			fprintf (fp,"|                  +\n");
			fprintf (fp,"%s+                  +--------------------", com_strt);
			fprintf (fp,"-------------------+                  +\n");
			fprintf (fp,"%s\n", com_strt);

			fprintf (fp,"%s%s%2.2d     Sensitivity:                           %E\n",
				blkt_id,fld_pref,5,type48->sensitivity);
			fprintf (fp,"%s%s%2.2d     Frequency of sensitivity:              %E\n",
				blkt_id,fld_pref,6,type48->frequency);
			fprintf (fp,"%s%s%2.2d     Number of calibrations:                %d\n",
				blkt_id,fld_pref,7,type48->number_calibrations);
			if (type48->number_calibrations != 0)
			{
				fprintf (fp,"%sCalibrations:\n",com_strt);
				fprintf (fp,"%s  i, sensitivity,  frequency,   time of calibration\n",
						 com_strt);
				for (i = 0; i < type48->number_calibrations; i++)
					fprintf (fp,"%s%s%2.2d-%2.2d  %3d % E % E %s\n",
					   blkt_id,fld_pref,8,10,i, 
					   type48->calibration[i].sensitivity, 
					   type48->calibration[i].frequency,
					   type48->calibration[i].time ? type48->calibration[i].time :
							     "(null)");
			}
			fprintf (fp,"%s\n",com_strt);
			break;
		}
	}
	if (type48 == NULL) return(0); else return(1);
}

int old_find_type48(fp, code)
FILE *fp;
int code;
{
	struct type48 *type48;							/* looping vbl */
	int i;										/* counter */

	for (type48 = type48_head; type48 != NULL; type48 = type48->next)
	{
		if (type48->response_code == code)
		{
			if (fp == NULL) break;

			fprintf (fp,"B048\n");
			fprintf (fp,"Response Lookup Code:                  %d\n",
				type48->response_code);
			fprintf (fp,"Response name:                         %s\n",
				type48->name ? type48->name : "(null)");
			fprintf (fp,"Sensitivity:                           %E\n",
				type48->sensitivity);
			fprintf (fp,"Frequency of sensitivity:              %E\n",
				type48->frequency);
			fprintf (fp,"Number of calibrations:                %d\n",
				type48->number_calibrations);
			if (type48->number_calibrations != 0)
			{
				fprintf (fp,"Calibrations:\n");
				fprintf (fp,"  i, sensitivity,  frequency,   time of calibration\n");
				for (i = 0; i < type48->number_calibrations; i++)
					fprintf (fp,"%3d % E % E %s\n", i, 
					type48->calibration[i].sensitivity, 
					type48->calibration[i].frequency,
					type48->calibration[i].time ? type48->calibration[i].time :
									 "(null)");
			}
			fprintf (fp,"\n");
			break;
		}
	}
	if (type48 == NULL) return(0); else return(1);
}
