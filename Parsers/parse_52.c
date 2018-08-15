/*===========================================================================*/
/* SEED reader     |               parse_type52              |  station header */
/*===========================================================================*/
/*
	Name:		parse_type52
	Purpose:	parse a channel id (type 52) blockette into a globally-available
				table
	Usage:		void parse_type52 ();
				char *blockette;
				parse_type52 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type52_head - 1st entry in table; defined in structures.h,
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_stationh
	Calls to:	parse_int - get an integer from a blockette 
			parse_double - get a double from a blockette 
			parse_nchar - get a fixed-length string from a blockette 
			parse_varlstr - get a variable-length string from a blockette
	Algorithm:	parse the blockette into a globally-available table
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
			11/04/88  Dennis O'Neill  replaced "~" by STRINGTERM in
				                          parse_varlstr calls
			11/21/88  Dennis O'Neill  Production release 1.0
			08/19/89  Dennis O'Neill  streamlined linked list construction
			09/17/89  Dennis O'Neill  removed number_ch_comments
			04/12/95  Cl - changed allocation and saving of struct 
				  	till the end, only save if selected 
					channel.
			02/08/96  CL - added orientation code to channel name if
				   	multiplexed data
*/

#include "rdseed.h"

extern int ignore_net_codes;    /* set in "main" -rdseed.c */

static struct type52 type52;

int parse_type52 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;		/* ptr to inside blockette */
	struct type52 *type52_ptr;	/* genl member of table */

	struct time chn_start_struct;
        struct time chn_end_struct;
	
	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|   recover information from the type52   |=================*/
/*                 +=======================================+                 */

	/* recover location identifier */
	type52.location = parse_nchar (&blockette_ptr, 2);

	/* recover channel identifier */
	type52.channel = parse_nchar (&blockette_ptr, 3);

	/* recover subchannel identifier (for multiplexing) */
	type52.subchannel = parse_int (&blockette_ptr, 4);


	/* recover index for instrument ident in dictionary */
	type52.instrument_code = parse_int (&blockette_ptr, 3);

	/* recover optional instrument comment */
	if (*blockette_ptr != '~')
		type52.inst_comment = parse_varlstr (&blockette_ptr, STRINGTERM);
	else {
		type52.inst_comment = NULL;
		blockette_ptr += 1;
	}

	/* recover index for signal units in dictionary */
	type52.signal_units_code = parse_int (&blockette_ptr, 3);

	/* recover index for calibration units in dictionary */
	type52.calib_units_code = parse_int (&blockette_ptr, 3);

	/* recover latitude */
	type52.latitude = parse_double (&blockette_ptr, 10);

	/* recover longitude */
	type52.longitude = parse_double (&blockette_ptr, 11);

	/* recover elevation */
	type52.elevation = parse_double (&blockette_ptr, 7);

	/* recover local depth */
	type52.local_depth = parse_double (&blockette_ptr, 5);

	/* recover azimuth */
	type52.azimuth = parse_double (&blockette_ptr, 5);

	/* recover dip */
	type52.dip = parse_double (&blockette_ptr, 5);

	/* recover data format id lookup code */
	type52.format_code = parse_int (&blockette_ptr, 4);

	/* recover log2 of data record length */
	type52.log2drecl = parse_int (&blockette_ptr, 2);

	/* recover sample rate */
	type52.samplerate = parse_double (&blockette_ptr, 10);

	/* recover maximum clock drift tolerance */
	type52.clock_tolerance = parse_double (&blockette_ptr, 10);

	/* recover number of channel comment blockettes - old spec, reserved */
	/* type52.number_ch_comments = parse_int (&blockette_ptr, 4); */
	type52.reserved1 = parse_int (&blockette_ptr, 4);

	/* recover channel flags, if any */
	if (*blockette_ptr != '~')
		type52.channel_flag = parse_varlstr (&blockette_ptr, STRINGTERM);
	else {
		type52.channel_flag = NULL;
		blockette_ptr += 1;
	}

	/* recover effective start date */
	type52.start = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover effective end date, if any */
	if (*blockette_ptr != '~')
		type52.end = parse_varlstr (&blockette_ptr, STRINGTERM);
	else {
		type52.end = NULL;
		blockette_ptr += 1;
	}

        /* check start/end effective if entered by user */
        timecvt(&chn_start_struct, type52.start);

	if (type52.end == NULL)
	{
		chn_end_struct.year = 9999;
	}
	else
        	timecvt(&chn_end_struct, type52.end);

       	if (!chk_time(chn_start_struct, chn_end_struct))
                return 0;

	/* recover update flag */
	type52.update = parse_nchar (&blockette_ptr, 1);

	/* initialize the pointer for the first of each of several elements */
	type52.response_head = NULL;
	type52.response_tail = NULL;
	type52.type59_head = NULL;
	type52.type59_tail = NULL;
	type52.channel_update = NULL;

/*                 +=======================================+                 */
/*=================|link the type52 into the type50 table  |=================*/
/*                 +=======================================+                 */

/* 1st, if multiplexed data, add the orientation code */
	if (type52.subchannel > 0)
	{
		type52.channel[2] = determine_orient_code(&type52); 
		type52.channel[3] = 0;
	}

	type52_ptr = (struct type52 *)alloc_linklist_element(sizeof(type52), "parse_type52");

        memcpy((char *)type52_ptr, (char *)&type52, sizeof(struct type52));

        append_linklist_element(type52_ptr,
                    current_station->type52_head,
                    current_station->type52_tail);

        current_channel = type52_ptr;

	return 1;

}

/* ------------------------------------------------------------------- */
#define WITHIN(t, s, e) ((timecmp(t, s) >= 0) && (timecmp(t, e) <= 0))

int tag_channel_rec(stn, net, b52)
char *stn;
char *net;
struct type52 *b52;

{
	struct type50 *p;
        struct time stn_start, stn_end;   /* response effective times */

	struct time chn_start;
	struct time chn_end;

	struct type52 *c_ptr, *sav_ptr;

       	timecvt(&chn_start, type52.start);
	timecvt(&chn_end, type52.end);


	if(type52.end == 0)
		chn_end.year = 9999;
	
	sav_ptr = 0;

	for (p = type50_head; p != NULL;p = p->next)
        {
                if (strcmp(p->station, stn) != 0)
                        continue;

                if (type10.version >= 2.3)
                {
                        /* check to make sure there is network code,
                         * as blank network code on disk, turns out as a NULL string
                         */
                        if (p->network_code && (!ignore_net_codes))
                                if (strcmp( p->network_code, net) != 0)
                                        continue;
                }

                /* convert station time (YYYY,DDD,....) to struct */
                timecvt(&stn_start, p->start);
                timecvt(&stn_end, p->end);

                if (p->end == 0)
                        stn_end.year = 9999;
 
                /* do time check */
                if (WITHIN(chn_start, stn_start, stn_end) ||
		 	WITHIN(chn_end, stn_start, stn_end))
		{
        
			c_ptr = (struct type52 *)alloc_linklist_element(sizeof(type52), "parse_type52");

        		memcpy((char *)c_ptr, (char *)b52, 
						sizeof(struct type52));
	
        		append_linklist_element(c_ptr,
                    		p->type52_head,
                    		p->type52_tail);

			sav_ptr = c_ptr;

			current_channel = c_ptr;

		}
                          
    	}	/* tag onto all station records if appropiate */

	if (sav_ptr == 0)
	{
		fprintf(stderr,
"WARNING - parse_type52: unable to find channel %s's station record! Check effective times.\n", type52.channel);

		fprintf(stderr, "Current station/net: %s/%s\n",
                                        current_station->station,
                                        current_station->network_code ?
                                        current_station->network_code : "N/A");
   /* dump effective times to screen for visual feedback */

		dump_station_effective(current_station->station, 
				current_station->network_code);

               	fprintf(stderr, "\tchannel %s effective start/stop times:\n\t\t%s / %s\n",
                                b52->channel,
                                b52->start,
                                b52->end);


		fprintf(stderr, "Ignoring effective times\n");
 
		c_ptr = (struct type52 *)alloc_linklist_element(sizeof(type52), "parse_type52");

        	memcpy((char *)c_ptr, (char *)b52, sizeof(struct type52));
	
        	append_linklist_element(c_ptr,
                    			current_station->type52_head,
                    			current_station->type52_tail);
 
        	current_channel = c_ptr;
	
	}

}


/* ------------------------------------------------------------------- */
int dump_stn_chn_recs()


{
        struct type50 *p;
        struct type52 *c;

printf("----------------------\n");

        for (p = type50_head; p != NULL;p = p->next)
        {
		printf("Station start=%s  ---- End=%s\n",
			p->start, p->end);


		for (c = p->type52_head; c != NULL; c = c->next)
			printf("\tChannel %s start=%s ---- End=%s\n\n",
				c->channel, c->start, c->end);


		printf("\n\n");
		
	}
	
printf("------------------------\n");


}

