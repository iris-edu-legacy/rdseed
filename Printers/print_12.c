/*===========================================================================*/
/* SEED reader     |             print_type12               |   volume header */
/*===========================================================================*/
/*
	Name:		print_type12
	Purpose:	print volume time span index blockette contents to 
				standard output
	Usage:		void print_type12 ();
				print_type12 ();
	Input:		none
	Output:		none
	Externals:	type12 - defined in structures.h, allocated in global.h
	Warnings:	none
	Errors:		none
	Called by:	print_volh
	Calls to:	none
	Algorithm:	extract the data from the structure, print it.  If there is no
				time specified for beginning of span or end of span, default is
				midnight.
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

void print_type12 ()
{
	int i;										/* counter */
	char *blkt_id="B012";						/* blockette id string */

	fprintf(outputfile,"%sVolume Time Span Index:\n",com_strt);
	fprintf(outputfile,"%s\tBeginning of Span       End of Span             ",
			com_strt);
	fprintf(outputfile,"Starts at record number\n");

	for (i = 0; i < type12.number_spans; i++)
	{
		fprintf(outputfile,"%s%s%2.2d-%2.2d  \t",blkt_id,fld_pref,4,6);
		if (strlen (type12.timespan[i].bos) == 8)
			fprintf(outputfile,"%-8s midnight       ", type12.timespan[i].bos);
		else fprintf(outputfile,"%-22s  ", 
					 type12.timespan[i].bos ? type12.timespan[i].bos : "(null)");

		if (strlen (type12.timespan[i].eos) == 8)
			fprintf(outputfile,"%-8s midnight       ", type12.timespan[i].eos);
		else fprintf(outputfile,"%-22s  ", 
					 type12.timespan[i].eos ? type12.timespan[i].eos : "(null)");

		fprintf(outputfile,"%6ld\n", type12.timespan[i].sequence_number);
	}
	fprintf(outputfile,"%s\n",com_strt);
}
