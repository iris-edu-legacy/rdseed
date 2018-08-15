/*===========================================================================*/
/* SEED reader     |             process_timeh             |     time header */
/*===========================================================================*/
/*
	Name:		process_timeh
	Purpose:	process the time span control records into a globally-available
				table
	Usage:		void process_timeh ();
				process_timeh ();
	Input:		none
	Output:		none
	Externals:	input - defined in structures.h, allocated in globals.h
	Warnings:	unknown blockette type encountered
	Errors:		none
	Called by:	main program
	Calls to:	Routines to parse the various blockette types:
				parse_type70 - Time Span Id Blockette parser
				parse_type71 - Hypocenter Information Blockette parser
				parse_type72 - Event Phases Blockette parser
				parse_type73 - Time Span Data Start Index Blockette parser
	Algorithm:	determine which type of blockette is encountered; parse it into
				the appropriate table
	Notes:		some of the parsing routines and their respective print routines
				are untested as of the initial release date; there were no
				blockettes of those types available as of the cited date.
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				08/29/89  Dennis O'Neill  added fcn parse_tsindex
*/

#include "rdseed.h"

void process_timeh ()
{
	char *input_data_ptr;					/* pointer into input data */

	input_data_ptr = input.data;

/*                 +=======================================+                 */
/*=================|  Process abbreviation dictionary hdr  |=================*/
/*                 +=======================================+                 */

	blockette_length = 0;
	blockette_type = -1;

	while (blockette_type != 0)
	{
		input_data_ptr += blockette_length;
		read_blockette (&input_data_ptr, blockette, &blockette_type, 
			&blockette_length);

		if (blockette_type == 70) parse_type70 (blockette);
		else if (blockette_type == 71) 
			parse_type71 (blockette);

		else if (blockette_type == 72) 
				parse_type72 (blockette);
		else if (blockette_type == 73) parse_type73 (blockette);
		else if (blockette_type == 74) parse_type74 (blockette);
		else if (blockette_type != 0) 
		{
			fprintf (stderr, "WARNING (process_timeh):  ");
			fprintf (stderr, "unknown blockette type %d found in record %ld.\n",
				blockette_type, input.recordnumber);
			fprintf (stderr, "\tExecution continuing.\n");
		}
	}

/*                 +=======================================+                 */
/*=================|                cleanup                |=================*/
/*                 +=======================================+                 */

}
