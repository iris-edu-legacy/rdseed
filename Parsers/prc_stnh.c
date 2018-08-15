/*===========================================================================*/
/* SEED reader     |              process_stnh             |  station header */
/*===========================================================================*/
/*
	Name:		process_stnh
	Purpose:	recover station header data and put into 
				globally-available storage

	Usage:		void process_stnh ();
				process_stnh ();
	Input:		none
	Output:		none
	Externals:	input - defined in structures.h, allocated in globals.h
	Warnings:	unknown blockette type encountered
	Errors:		none
	Called by:	main program
	Calls to:	Routines to parse the various blockette types:
			parse_type50 - Station ID Blockette parser
			parse_type51 - Station Comment Blockette parser
			parse_type52 - Channel ID Blockette parser
			parse_type53 - Response (Poles and Zeroes) parser
			parse_type54 - Response (Coefficients) Blockette parser
			parse_type55 - Response (List) Blockette parser
			parse_type56 - Generic Response Blockette parser
			parse_type57 - Decimation Blockette parser
			parse_type58 - Channel Sensitivity Blockette parser
			parse_type59 - Channel Comment Blockette parser
			parse_type60 - Response Reference Blockette parser
	Algorithm:	
		determine which type of blockette is encountered; parse it into
		the appropriate table
	Notes:	some of the parsing routines and their respective print routines
		are untested as of the initial release date; there were no
		blockettes of those types available as of the cited date.

	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				05/02/90  Sue Schoch      Included blockette 57 support. Rel 2.0
                05/15/90  Sue Schoch      Included blockette 60 support.
				03/25/99  Included blockette 62 support.
*/

#include "rdseed.h"

extern int ignore_net_codes;

static int channel_wanted = FALSE;
static int found_station = FALSE;

static int found_channel = FALSE;

int process_stnh ()
{
	char wrkstr[10];


	char *input_data_ptr;		/* pointer to input data */

	char prev_stn[10];

	input_data_ptr = input.data;

/*                 +=======================================+                 */
/*=================|        Process station headers        |=================*/
/*                 +=======================================+                 */

	blockette_length = 0;
	blockette_type = -1;

	while (blockette_type != 0)
	{
		input_data_ptr += blockette_length;
		read_blockette (&input_data_ptr, blockette, &blockette_type, 
			&blockette_length);

		/* could be zero if blank to end of record */
		if (blockette_length == 0)
			return 1;

		/* check for garbage */
		if ((blockette_type < 50) || 
		    (blockette_type > 69))
			return 0;

		if (blockette_type == 50) 
		{
			/* if this station is new, ie doesn't match up with
			 * previous scanned station, reset flags
			 */
			strncpy(wrkstr, &blockette[7], 5);
			wrkstr[5] = 0;
			
			if (strcmp(wrkstr, prev_stn) != 0)
			{
				strcpy(prev_stn, wrkstr);

				reset_stn_chn_flags();
			}

			/* check the network code, if called from process_B11,
			 * the B11 does not have a network code, so we need to
			 * check here
			 */
			if (!ignore_net_codes)
			{
				char netcode[4];

				/* we can take advantage of the fact that the
				 * network code, as of 2.3, is at the end of this
				 * string - 2
				 */

				strcpy(netcode, 
					type10.version >= 2.3 ?
					  &blockette[strlen(blockette) - 2] :
					  "");

				if (!chk_network(netcode))
					return 0;

			}

			found_channel = FALSE;

			if (parse_type50(blockette))
				found_station = TRUE;

			continue;
		}

		/* loop back to continue parsing */
		if (!found_station)
			continue; 
	
		if (blockette_type == 51) 
		{
			parse_type51 (blockette);
			continue;
		}
 
		if (blockette_type == 52) 
		{
			/* check to see if it is a wanted channel */
			/* channel name starts at the 9th byte */
			/* past type, length, location code */
			strncpy(wrkstr, &blockette[9], 3);
			wrkstr[3] = 0;

			/* this checks if user requested this channel */
			if (chk_channel(wrkstr))
			{
				channel_wanted = TRUE;

				/* check location code */
                                strncpy(wrkstr, &blockette[7], 2);
                                wrkstr[2] = 0;

                                if (!chk_location(wrkstr))
					channel_wanted = FALSE;
				else
					/* if effective times are not what is 
				 	 * wanted, p_52 returns false
				 	 */

					if (!parse_type52(blockette))
						channel_wanted = FALSE;
					else
						found_channel = TRUE;
			}
			else
				channel_wanted = FALSE;

			continue;
		}


		if ((blockette_type == 59) && found_channel) 
		{
			/* found_channel is there because if the
			 * user requested say LHE for a specific
			 * time, and there are subsequent updates
			 * in the DATALESS say, the B059s get tagged
			 * onto the end if all updates. So we flag it
			 * earlier so we no to parse it
			 */
			parse_type59(blockette);
			continue;
                } 

		if (!channel_wanted)
			continue;

		if (blockette_type == 53) 
		{
			parse_type53(blockette);
			continue;
		}
		
		if (blockette_type == 54) 
		{
			parse_type54(blockette);
			continue;
		}
		
		if (blockette_type == 55) 
		{
			parse_type55(blockette);
			continue;
		}
		
		if (blockette_type == 56) 
		{
			parse_type56(blockette);
			continue;
		}
		
		if (blockette_type == 57) 
		{
			parse_type57(blockette);     
			continue;
		}
		
		if (blockette_type == 58)
		{
			parse_type58(blockette);
			continue;
		}
		

		if (blockette_type == 60) 
		{
			parse_type60(blockette);     
			continue;
                } 

		if (blockette_type == 61) 
		{
			parse_type61(blockette);
			continue;
		}

		if (blockette_type == 62) 
		{
			parse_type62(blockette);
			continue;
		}
		
		if (blockette_type != 0) 
		{
			fprintf (stderr, "WARNING (process_stationh):  ");
			fprintf (stderr, "unknown blockette type %d found in record %ld.\n",
				blockette_type, input.recordnumber);
			fprintf (stderr, "\tExecution continuing.\n");
	
			return 0;

		}
		
	}


	return 1;

/*                 +=======================================+                 */
/*=================|                cleanup                |=================*/
/*                 +=======================================+                 */

}

/*                 +=======================================+                 */
/*=================|                helper proc            |=================*/
/*                 +=======================================+                 */

int reset_stn_chn_flags()

{

	channel_wanted = FALSE;
	found_station = FALSE;
 
	found_channel = FALSE;
}
