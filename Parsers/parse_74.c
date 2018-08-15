/*===========================================================================*/
/* SEED reader     |            parse_type74             |     time header */
/*===========================================================================*/
/*
	Name:		parse_type74
	Purpose:	parse a time series index (type 74) blockette into a
				globally-available table
	Usage:		void parse_type74 ();
				char *blockette;
				parse_type74 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type74_head - 1st entry in table; defined in structures.h,
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
	Revisions:	08/29/89  Dennis O'Neill  Initial version
				04/26/93  Allen Nance     added field 16 for v2.3
				09/01/93  CL 			  fixed minor bug in parsing for
										  number of accels.
				05/11/94  CL			  took out call to malloc if 0 bytes

*/

#include "rdseed.h"

/* this needs to be allocated here so it keeps its value between calls */
struct type74 *type74_tail = NULL;			/* end of linked list */

void parse_type74 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type74 *type74; 					/* genl member of table */
	int i;										/*counter */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================| allocate space and initialize a type73|=================*/
/*                 +=======================================+                 */

	type74 = (struct type74 *) alloc_linklist_element
		(sizeof (struct type74), "parse_type74");	

	memset((char *)type74, 0, sizeof(struct type74));


/*                 +=======================================+                 */
/*=================|  recover information from the type73  |=================*/
/*                 +=======================================+                 */

	type74->station = parse_nchar (&blockette_ptr, 5);
	type74->location = parse_nchar (&blockette_ptr, 2);
	type74->channel = parse_nchar (&blockette_ptr, 3);

	if (*blockette_ptr != '~')
		type74->starttime = parse_varlstr (&blockette_ptr, STRINGTERM);
	else {
		type74->starttime = NULL;
		blockette_ptr += 1;
	}
	type74->start_index = parse_long(&blockette_ptr, 6);
	type74->start_subindex = parse_int (&blockette_ptr, 2);

	if (*blockette_ptr != '~')
		type74->endtime = parse_varlstr (&blockette_ptr, STRINGTERM);
	else {
		type74->endtime = NULL;
		blockette_ptr += 1;
	}
	type74->end_index = parse_long(&blockette_ptr, 6);
	type74->end_subindex = parse_int (&blockette_ptr, 2);

	/* recover number of accelerators */
	type74->number_accelerators = parse_int (&blockette_ptr, 3);

	/* allocate space for data pieces */
	if (type74->number_accelerators != 0)
		type74->accelerator = (struct type74sub *) 
			alloc_linklist_element(sizeof(struct type74sub) *
						type74->number_accelerators,
						"parse_type74->datapiece");

	/* recover accelerators */
	for (i = 0; i < type74->number_accelerators; i++)
	{
		if (*blockette_ptr != '~')
			type74->accelerator[i].time =
				parse_varlstr (&blockette_ptr, STRINGTERM);
		else {
			type74->accelerator[i].time = NULL;
			blockette_ptr += 1;
		}
		type74->accelerator[i].index = parse_long(&blockette_ptr, 6);
		type74->accelerator[i].subindex = parse_int (&blockette_ptr, 2);
	}

	/* recover network info for SEED version 2.3 */
	if (type10.version >= 2.3)
	{
		type74->network_code = parse_nchar (&blockette_ptr, 2);
	}
	else
	{
		type74->network_code = NULL;
	}

/*                 +=======================================+                 */
/*=================| link the type74 into the type74 table |=================*/
/*                 +=======================================+                 */

	append_linklist_element (type74, type74_head, type74_tail);

}
