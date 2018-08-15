/*===========================================================================*/
/* SEED reader     |               pagetop                 |         utility */
/*===========================================================================*/
/*
	Name:		pagetop
	Purpose:	print a top-of-report heading for the SEED reader
+-----------------------------------------------------------------------------+
| +-------------------------------------------------------------------------+ |
| |                       RDSEED version 2 release 0                        | |
| |    FDSN Standard for Exchange of Earthquake Data (SEED) data reader     | |
| +-------------------------------------------------------------------------+ |
+-----------------------------------------------------------------------------+
	Usage:		void pagetop ();
				pagetop ();
	Input:		none
	Output:		none
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	anything; typically by process_*
	Calls to:	none
	Algorithm:	print the heading
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "version.h"
#include "resp_defs.h"

void pagetop ()
{
	fprintf(outputfile,"%s+---------------------------------------",com_strt);
	fprintf(outputfile,"--------------------------------------+\n");
	fprintf(outputfile,"%s| +-------------------------------------",com_strt);
	fprintf(outputfile,"------------------------------------+ |\n");
	fprintf(outputfile,"%s| |                       RDSEED version",com_strt);
	fprintf(outputfile," %-14s                     | |\n", VERSION);
	fprintf(outputfile,"%s| |    IRIS Standard for Exchange of Ear",com_strt);
	fprintf(outputfile,"thquake Data (SEED) data reader     | |\n");
	fprintf(outputfile,"%s| +-------------------------------------",com_strt);
	fprintf(outputfile,"------------------------------------+ |\n");
	fprintf(outputfile,"%s+---------------------------------------",com_strt);
	fprintf(outputfile,"--------------------------------------+\n");
	fprintf(outputfile,"%s\n%s\n",com_strt,com_strt);
}
