/*===========================================================================*/
/* SEED reader     |               parse_type72               |     time header */
/*===========================================================================*/
/*
	Name:		parse_type72
	Purpose:	parse an event phases (type 72) blockette
				into a globally-available table
	Usage:		void parse_type72 ();
				char *blockette;
				parse_type72 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type72_head - 1st entry in table; defined in structures.h,
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_stationh
	Calls to:	parse_int - get an integer from a blockette 
				parse_double - get a double from a blockette 
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
				04/26/93  Allen Nance     added fields 11,12 for v2.3
*/

#include "rdseed.h"

/* this needs to be allocated here so it keeps its value between calls */
struct type72 *type72_tail = NULL;				/* end of linked list */

void parse_type72 (blockette)
char *blockette;						/* ptr to start of blockette */
{
	char *blockette_ptr;					/* ptr to inside blockette */
	struct type72 *type72; 					/* genl member of table */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|  allocate space and initialize a type72  |=================*/
/*                 +=======================================+                 */

	type72 = (struct type72 *) alloc_linklist_element (sizeof (struct type72),
		"parse_type72");	

/*                 +=======================================+                 */
/*=================|   recover information from the type72    |=================*/
/*                 +=======================================+                 */

	/* recover station id */
	type72->station = parse_nchar (&blockette_ptr, 5);

	/* recover location id */
	type72->location = parse_nchar (&blockette_ptr, 2);

	/* recover channel id */
	type72->channel = parse_nchar (&blockette_ptr, 3);

	/* recover arrival time of phase */
	type72->arrival_time = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover signal amplitude (see channel id for units) */
	type72->amplitude = parse_double (&blockette_ptr, 10);

	/* recover signal period */
	type72->period = parse_double (&blockette_ptr, 10);

	/* recover signal to noise ratio */
	type72->sig2noise = parse_double (&blockette_ptr, 10);

	/* recover name of phase */
	type72->phasename = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover source and network info for SEED version 2.3 */
	if (type10.version >= 2.3)
	{
		type72->source_code  = parse_int (&blockette_ptr, 2);
		type72->network_code = parse_nchar (&blockette_ptr, 2);
	}
	else
	{
		type72->source_code = 0;
		type72->network_code = NULL;
	}


/*                 +=========================================+                 */
/*=================|   link the type72 into the type71 table |=================*/
/*                 +=========================================+                 */

	if (current_origin == NULL)
	{
		append_linklist_element (type72, type72_head, type72_tail);
	}
	else
		append_linklist_element (type72, current_origin->type72_head, current_origin->type72_tail);

}
