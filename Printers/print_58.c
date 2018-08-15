/*===========================================================================*/
/* SEED reader     |              print_type58                |  station header */
/*===========================================================================*/
/*
	Name:		print_type58
	Purpose:	print the channel sensitivity table to standard output
	Usage:		void print_type58 ();
				print_type58 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type58_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type58_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				05/01/92   Allen Nance    Added \n to  printf call
                02/17/93  CL    nullpointer protect fprintfs
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type58 (fp,type58_head)
FILE *fp;
struct type58 *type58_head;
{
	struct type58 *type58;						/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B058";						/* blockette id string */

	for (type58 = type58_head; type58 != NULL; type58 = type58->next)
	{
		fprintf (fp,"%s+                  +--------------------", com_strt);
		fprintf (fp,"-------------------+                  +\n");
		if (type58->stage == 0) 
			fprintf (fp,"%s+                  |   Channel Sensitivity, %5s ch %3s   ", com_strt,
					current_station->station, 
					current_channel->channel);

		else
			fprintf (fp,"%s+                  |       Channel Gain, %5s ch %3s      ", com_strt,
					current_station->station, current_channel->channel);

		fprintf (fp,"|                  +\n");
		fprintf (fp,"%s+                  +--------------------", com_strt);
		fprintf (fp,"-------------------+                  +\n");
		fprintf (fp,"%s\n", com_strt);
	
		fprintf (fp,"%s%s%2.2d     Stage sequence number:                 %d\n",
			blkt_id,fld_pref,3,type58->stage);
		if (type58->stage == 0) {
			fprintf (fp,"%s%s%2.2d     Sensitivity:                           %E\n",
					 blkt_id,fld_pref,4,type58->sensitivity);
			fprintf (fp,"%s%s%2.2d     Frequency of sensitivity:              %5E HZ\n",
					 /* WAN 5-1-92 */			blkt_id,fld_pref,5,type58->frequency);
		}
		else {
			fprintf (fp,"%s%s%2.2d     Gain:                                  %5E\n",
					 blkt_id,fld_pref,4,type58->sensitivity);
			fprintf (fp,"%s%s%2.2d     Frequency of gain:                     %5E HZ\n",
					 /* WAN 5-1-92 */			blkt_id,fld_pref,5,type58->frequency);
		}

/*		find_type34(fp, current_channel->signal_units_code); */
		fprintf (fp,"%s%s%2.2d     Number of calibrations:                %d\n",
			blkt_id,fld_pref,6,type58->number_calibrations);
		if (type58->number_calibrations != 0)
		{
			fprintf (fp,"%sCalibrations:\n",com_strt);
			fprintf (fp,"%s i, sensitivity, frequency, time of calibration\n", com_strt);
			for (i = 0; i < type58->number_calibrations; i++)
				fprintf (fp,"%s%s%2.2d-%2.2d  %3d % 5E % 5E %s\n",
				  blkt_id,fld_pref,7,8,i, 
				  type58->calibration[i].sensitivity, 
				  type58->calibration[i].frequency,
				  type58->calibration[i].time ? type58->calibration[i].time : "(null)");
		}
		fprintf (fp,"%s\n", com_strt);
	}
}

void old_print_type58 (fp,type58_head)
FILE *fp;
struct type58 *type58_head;
{
	struct type58 *type58;							/* looping vbl */
	int i;										/* counter */


	for (type58 = type58_head; type58 != NULL; type58 = type58->next)
	{
		fprintf (fp,"+                  +--------------------");
		fprintf (fp,"-------------------+                  +\n");
		if (type58->stage == 0) 
			fprintf (fp,"+                  |   Channel Sensitivity, %5s ch %3s   ",
					current_station->station, 
					current_channel->channel);

		else
			fprintf (fp,"+                  |       Channel Gain, %5s ch %3s      ",
					current_station->station, current_channel->channel);

		fprintf (fp,"|                  +\n");
		fprintf (fp,"+                  +--------------------");
		fprintf (fp,"-------------------+                  +\n");
		fprintf (fp,"\n");
	
		fprintf (fp,"Stage sequence number:                 %d\n",
			type58->stage);
		if (type58->stage == 0) 
			fprintf (fp,"Sensitivity:                           %E\n", type58->sensitivity);
		else
			fprintf (fp,"Gain:                                  %5E\n", type58->sensitivity);
		fprintf (fp,"Frequency of sensitivity:              %5E HZ\n",  /* WAN 5-1-92 */			type58->frequency);
/*		find_type34(fp, current_channel->signal_units_code); */
		fprintf (fp,"Number of calibrations:                %d\n",
			type58->number_calibrations);
		if (type58->number_calibrations != 0)
		{
			fprintf (fp,"Calibrations:\n");
			fprintf (fp," i, sensitivity, frequency, time of calibration\n");
			for (i = 0; i < type58->number_calibrations; i++)
				fprintf (fp,"%3d % 5E % 5E %s\n", i, 
				type58->calibration[i].sensitivity, 
				type58->calibration[i].frequency,
				type58->calibration[i].time ? type58->calibration[i].time : "(null)");
		}
		fprintf (fp,"\n");
	}
}
