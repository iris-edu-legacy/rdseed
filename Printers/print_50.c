/*===========================================================================*/
/* SEED reader     |              print_type50               |  station header */
/*===========================================================================*/
/*
	Name:		print_type50
	Purpose:	print the contents of a station identifier entry to the standard
				output
	Usage:		void print_type50 ();
				print_type50 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	current_station - ptr to the current station header
	Warnings:	none
	Errors:		none
	Called by:	print_stationh
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type50_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				09/17/89  Dennis O'Neill  removed number_channel,
				                          number_comments lines
				04/26/93  Allen Nance     added field 16 for v2.3
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type50 ()
{
	char *blkt_id="B050";						/* blockette id string */

	fprintf(outputfile,"%s+---------------------------------------",com_strt);
	fprintf(outputfile,"--------------------------------------+\n");
	fprintf(outputfile,"%s| Station header   |             Station %5s",
			com_strt,current_station->station);
	fprintf(outputfile,"             |        SEED tape |\n");
	fprintf(outputfile,"%s+---------------------------------------",com_strt);
	fprintf(outputfile,"--------------------------------------+\n");
	fprintf(outputfile,"%s\n",com_strt);

	fprintf(outputfile,"%s%s%2.2d         Station code:                          %s\n",
			blkt_id,fld_pref,3,current_station->station ? current_station->station : "(null)");
	fprintf(outputfile,"%s%s%2.2d         Latitude:                              %f\n",
			blkt_id,fld_pref,4,current_station->latitude);
	fprintf(outputfile,"%s%s%2.2d         Longitude:                             %f\n",
			blkt_id,fld_pref,5,current_station->longitude);
	fprintf(outputfile,"%s%s%2.2d         Elevation:                             %f\n",
			blkt_id,fld_pref,6,current_station->elevation);
/*
	- old spec, replaced with reserved fields -
	fprintf(outputfile,"Number of channels:                    %d\n",
		current_station->number_channels);
	fprintf(outputfile,"Number comments:                       %d\n",
		current_station->number_comments);
*/
	fprintf(outputfile,"%s%s%2.2d         Name:                                  %s\n",
			blkt_id,fld_pref,9,current_station->name ? current_station->name : "(null)");
	fprintf(outputfile,"%s%s%2.2d         Owner Code Lookup:  %d                ",
			blkt_id,fld_pref,10,current_station->owner_code);
	find_type33(outputfile, current_station->owner_code);
	fprintf(outputfile,"%s%s%2.2d         32-bit word order:                     %04d\n",
			blkt_id,fld_pref,11,current_station->longword_order);
	fprintf(outputfile,"%s%s%2.2d         16-bit word order:                     %02d\n",
			blkt_id,fld_pref,12,current_station->word_order);
	fprintf(outputfile,"%s%s%2.2d         Starting date:                         %s\n",
			blkt_id,fld_pref,13,current_station->start ? current_station->start : "(null)");
	fprintf(outputfile,"%s%s%2.2d         Ending date:                           %s\n",
			blkt_id,fld_pref,14,current_station->end ? current_station->end : "(null)");
	fprintf(outputfile,"%s%s%2.2d         Update flag:                           %s\n",
			blkt_id,fld_pref,15,current_station->update ? current_station->update : "(null)");
	/* if (current_station->network_code != NULL) */
		fprintf(outputfile,"%s%s%2.2d         Network Code:                          %s\n",
				blkt_id,fld_pref,16,
				current_station->network_code ? current_station->network_code : "??");
	fprintf(outputfile,"%s\n",com_strt);
}
