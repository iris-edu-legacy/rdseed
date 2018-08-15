/*===========================================================================*/
/* SEED reader     |             print_type11               |   volume header */
/*===========================================================================*/
/*
	Name:		print_type11
	Purpose:	print volume station header index blockette contents to 
				standard output
	Usage:		void print_type11 ();
				print_type11 ();
	Input:		none
	Output:		none
	Externals:	type11 - defined in structures.h, allocated in global.h
	Warnings:	none
	Errors:		none
	Called by:	print_volh
	Calls to:	none
	Algorithm:	extract the data from the structure, print it
	Notes:		none
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

void print_type11 ()
{
	int i;										/* counter */
	char *blkt_id="B011";						/* blockette id string */

	struct type11 *type11; 

	if (type11_head != NULL)
        {

		fprintf(outputfile,"%sVolume Station Header Index:\n",com_strt);
		fprintf(outputfile,"%sStation  Station header starts at record\n",com_strt);

		for (type11 = type11_head; type11 != NULL; type11 = type11->next)
		{

			for (i = 0; i < type11->number_stations; i++)
				fprintf(outputfile,"%s%s%2.2d-%2.2d  %-7s  %6ld\n",
					blkt_id,fld_pref,4,5,
					type11->station[i].station_id ? type11->station[i].station_id : "(null)",
					type11->station[i].sequence_number);

			fprintf(outputfile,"%s\n",com_strt);
		}
	}

}
