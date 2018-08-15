/*===========================================================================*/
/* SEED reader     |              print_type52               |  station header */
/*===========================================================================*/
/*
	Name:		print_type52
	Purpose:	print the contents of the channel identifier entry to standard 
				output
	Usage:		void print_type52 ();
				print_type52 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type52_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_abbrevdic
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type52_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				09/17/89  Dennis O'Neill  removed number_ch_comments
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type52 (type52)
struct type52 *type52;								/* current channel */
{
	char *blkt_id="B052";						/* blockette id string */

	fprintf(outputfile,"%s+                  +--------------------",com_strt);
	fprintf(outputfile,"-------------------+                  +\n");
	fprintf(outputfile,"%s+------------------|   Station %5s   |    Channel %3s",
		com_strt,current_station->station, type52->channel);
	fprintf(outputfile,"    |------------------+\n");
	fprintf(outputfile,"%s+                  +--------------------",com_strt);
	fprintf(outputfile,"-------------------+                  +\n");
	fprintf(outputfile,"%s\n",com_strt);

	fprintf(outputfile,"%s%s%2.2d     Channel:                               %s\n", 
			blkt_id,
			fld_pref, 4,
			type52->channel ? type52->channel : "(null)");

	fprintf(outputfile,"%s%s%2.2d     Location:                              %s\n", 
			blkt_id,
			fld_pref, 3,
			type52->location ? type52->location : "(null)");

	fprintf(outputfile,"%s%s%2.2d     Subchannel:                            %d\n",
			blkt_id,fld_pref,5,type52->subchannel);
	fprintf(outputfile,"%s%s%2.2d     Instrument lookup:    %4d             ",
			blkt_id,fld_pref,6,type52->instrument_code);
	find_type33(outputfile,type52->instrument_code);
	fprintf(outputfile,"%s%s%2.2d     Comment:                               %s\n",
			blkt_id,fld_pref,7,type52->inst_comment ? type52->inst_comment : "(null)");
	fprintf(outputfile,"%s%s%2.2d     Signal units lookup:  %4d             ",
			blkt_id,fld_pref,8,type52->signal_units_code);

	find_type34(outputfile,type52->signal_units_code);

	fprintf(outputfile,"%s%s%2.2d     Calibration units lookup:  %4d        ",
		blkt_id,fld_pref,9,type52->calib_units_code);
	find_type34(outputfile,type52->calib_units_code);
	fprintf(outputfile,"%s%s%2.2d     Latitude:                              %f\n",
			blkt_id,fld_pref,10,type52->latitude);
	fprintf(outputfile,"%s%s%2.2d     Longitude:                             %f\n",
			blkt_id,fld_pref,11,type52->longitude);
	fprintf(outputfile,"%s%s%2.2d     Elevation:                             %f\n",
			blkt_id,fld_pref,12,type52->elevation);
	fprintf(outputfile,"%s%s%2.2d     Local depth:                           %f\n",
			blkt_id,fld_pref,13,type52->local_depth);
	fprintf(outputfile,"%s%s%2.2d     Azimuth:                               %f\n",
			blkt_id,fld_pref,14,type52->azimuth);
	fprintf(outputfile,"%s%s%2.2d     Dip:                                   %f\n",
			blkt_id,fld_pref,15,type52->dip);
	fprintf(outputfile,"%s%s%2.2d     Format lookup:  %4d                   ",
			blkt_id,fld_pref,16,type52->format_code);
	find_type30(outputfile, type52->format_code);
	fprintf(outputfile,"%s%s%2.2d     Log2 of Data record length:            %d\n",
			blkt_id,fld_pref,17,type52->log2drecl);
	fprintf(outputfile,"%s%s%2.2d     Sample rate:                           %G\n",
			blkt_id,fld_pref,18,type52->samplerate);
	fprintf(outputfile,"%s%s%2.2d     Clock tolerance:                       %G\n",
			blkt_id,fld_pref,19,type52->clock_tolerance);
/*
	- old spec, removed -
	fprintf(outputfile,"Number of channel comments:            %d\n",
		type52->number_ch_comments);
*/
	fprintf(outputfile,"%s%s%2.2d     Channel flags:                         %s\n",
			blkt_id,fld_pref,21,type52->channel_flag ? type52->channel_flag : "(null)");

	fprintf(outputfile,"%s%s%2.2d     Start date:                            %s\n", 
			blkt_id,fld_pref,22,type52->start ? type52->start: "(null)");
	fprintf(outputfile,"%s%s%2.2d     End date:                              %s\n", 
			blkt_id,fld_pref,23,type52->end ? type52->end: "(null)");

	fprintf(outputfile,"%s%s%2.2d     Update flag:                           %s\n", 
			blkt_id,fld_pref,24,type52->update ? type52->update : "(null)");
	fprintf(outputfile,"%s\n",com_strt);
}
