/*===========================================================================*/
/* SEED reader     |             process_abrvd             |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		process_abrvd
	Purpose:	recover abbreviation dictionary data and put into 
				globally-available storage
	Usage:		void process_abrvd ();
				process_abrvd ();
	Input:		none
	Output:		none
	Externals:	input - defined in structures.h, allocated in globals.h
	Warnings:	unknown blockette type encountered
	Errors:		none
	Called by:	main program
	Calls to:	Routines to parse the various blockette types:
				parse_type30 - Data Format Dictionary Blockette parser
				parse_type31 - Comment Description Dictionary Blockette parser
				parse_type32 - Cited Source Dictionary Blockette parser
				parse_type33 - Generic Abbreviation Blockette parser
				parse_type34 - Units Abbreviation Blockette parser
				parse_type35 - Beam Configuration Blockette parser

				parse_type43 - Response (Poles & Zeros) Blockette parser
				parse_type44 - Response (Coefficients) Blockette parser
				parse_type45 - Response List Blockette parser
				parse_type46 - Generic Response Blockette parser
				parse_type47 - Decimation Blockette parser
				parse_type48 - Channel Sensitivitty/Gain Blockette parser

	Algorithm:	determine which type of blockette is encountered; parse it into
				the appropriate table
	Notes:		parse_csdb and parse_type35 and their respective print routines are
	Notes:		parse_type32 and parse_type35 and their respective print routines are
				untested as of initial release date; there were no blockettes of
				those types available as of the cited date.
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				05/08/90  Sue Schoch      Production release 2.0
                                   001    Added support for 4? blockettes
				03/25/99  Stephane Zuzlewski	Added support for blockette 42
*/

#include "rdseed.h"

void process_abrvd ()
{
	char *input_data_ptr;					/* pointer to input data */

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

		if (blockette_type == 30) parse_type30 (blockette);
		else if (blockette_type == 31) parse_type31 (blockette);
		else if (blockette_type == 32) parse_type32 (blockette);
		else if (blockette_type == 33) parse_type33 (blockette);
		else if (blockette_type == 34) parse_type34 (blockette);
		else if (blockette_type == 35) parse_type35 (blockette);

		else if (blockette_type == 41) parse_type41 (blockette);  /* 001 */
		else if (blockette_type == 42) parse_type42 (blockette);  /* 001 */
		else if (blockette_type == 43) parse_type43 (blockette);  /* 001 */
		else if (blockette_type == 44) parse_type44 (blockette);  /* 001 */
		else if (blockette_type == 45) parse_type45 (blockette);  /* 001 */
		else if (blockette_type == 46) parse_type46 (blockette);  /* 001 */
		else if (blockette_type == 47) parse_type47 (blockette);  /* 001 */
		else if (blockette_type == 48) parse_type48 (blockette);  /* 001 */
		else if (blockette_type != 0) 
		{
			fprintf (stderr, "WARNING (process_abbrevdic):  ");
			fprintf (stderr, "unknown blockette type %d found in record %ld.\n",
				blockette_type, input.recordnumber);
			fprintf (stderr, "\tExecution continuing.\n");
		}
	}

/*                 +=======================================+                 */
/*=================|                cleanup                |=================*/
/*                 +=======================================+                 */

}
