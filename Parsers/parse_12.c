/*===========================================================================*/
/* SEED reader     |             parse_type12               |   volume header */
/*===========================================================================*/
/*
	Name:		parse_type12
	Purpose:	parse the volume time span index (type 12) blockette into a 
				globally-available structure
	Usage:		void parse_type12 ();
				char *blockette;
				parse_type12 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char - allocated in globals.h
				type12 - defined in structures.h, allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_volh
	Calls to:	parse_int - get an integer from a blockette 
				parse_long - get a long integer from a blockette 
				parse_varlstr - get a variable-length string from a blockette
	Algorithm:	skip the blockette type indicator and length; read the number of
				time spans; for each time span, read the starting time, ending 
				time, and sequence number.
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
				05/11/94  CL 		      took out call to malloc if 0 bytes
*/

#include "rdseed.h"

void parse_type12 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	int i;										/* counter */
	
	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

	/* read the number of time spans in the index table */
	type12.number_spans = parse_int (&blockette_ptr, 4);

	/* allocate space for the beginning and end of span, sequence number */
	if (type12.number_spans != 0)
		type12.timespan = (struct type12sub *) alloc_linklist_element
			(sizeof (struct type12sub) * type12.number_spans, "parse_type12");

	/* read the time spans and sequence numbers, print result */
	for (i = 0; i < type12.number_spans; i++)
	{
		type12.timespan[i].bos = parse_varlstr (&blockette_ptr, STRINGTERM);
		type12.timespan[i].eos = parse_varlstr (&blockette_ptr, STRINGTERM);
		type12.timespan[i].sequence_number = parse_long(&blockette_ptr, 6);
	}

}
