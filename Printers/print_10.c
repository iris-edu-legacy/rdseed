/*===========================================================================*/
/* SEED reader     |             print_type10                |   volume header */
/*===========================================================================*/
/*
	Name:		print_type10
	Purpose:	print volume id blockette contents to standard output
	Usage:		void print_type10 ();
				print_type10 ();
	Input:		none
	Output:		none
	Externals:	type10 - defined in structures.h, allocated in global.h
	Warnings:	none
	Errors:		none
	Called by:	print_volh
	Calls to:	pow - C power function
	Algorithm:	extract the data from the structure, print it
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				04/26/93  Allen Nance     added fields 7,8,9 for v2.3
				09/01/93  CL			  added printing of volume label
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type10 ()
{
	int lrecl;									/* logical record length */
	char *blkt_id="B010";						/* blockette id string */

	fprintf(outputfile,"%s+---------------------------------------",com_strt);
	fprintf(outputfile,"--------------------------------------+\n");
	fprintf(outputfile,"%s| Volume Header    |  General informatio",com_strt);
	fprintf(outputfile,"n about this tape  |        SEED tape |\n");
	fprintf(outputfile,"%s+---------------------------------------",com_strt);
	fprintf(outputfile,"--------------------------------------+\n");
	fprintf(outputfile,"%s\n",com_strt);

	/* calculate logical record length from log2lrecl */
	lrecl = (int) pow ((double) 2, (double) type10.log2lrecl);

	fprintf(outputfile,"%s%s%2.2d     %-30s%47.1f\n", blkt_id,fld_pref,3,
			"SEED Format version:",type10.version);
	fprintf(outputfile,"%s%s%2.2d     %-30s%41d bytes\n",blkt_id,fld_pref,4,
			"Logical record length:",lrecl);

	fprintf(outputfile,"%s%s%2.2d     %-30s%47s\n",blkt_id,fld_pref,5,
			"Starting date of this volume:",type10.bov ? type10.bov : "(null)");

	fprintf(outputfile, "%s%s%2.2d     %-30s%47s\n",blkt_id,fld_pref,6,
			"Ending date of this volume:",type10.eov ? type10.eov : "(null)");

	if (type10.version >= 2.3)
	{
		fprintf(outputfile, "%s%s%2.2d     %-30s%47s\n", 
				blkt_id,fld_pref,7,"Creation Date of this volume:", 
				type10.volume_time ? type10.volume_time :"(null)");

		fprintf(outputfile, "%s%s%2.2d     %-30s%47s\n",blkt_id,fld_pref,8,
				"Originating Organization:",
				type10.organization ? type10.organization : "(null)");

		fprintf(outputfile, "%s%s%2.2d     %-30s%47s\n",blkt_id,fld_pref,9,
				"Volume Label:",
				type10.volume_label ? type10.volume_label : "(null)");
	}
	fprintf(outputfile,"%s\n",com_strt);
}
