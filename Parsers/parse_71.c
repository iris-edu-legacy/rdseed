
/*===========================================================================*/
/* SEED reader     |               parse_type71               |     time header */
/*===========================================================================*/
/*
	Name:		parse_type71
	Purpose:	parse a hypocenter information (type 71) blockette
				into a globally-available table
	Usage:		void parse_type71 ();
				char *blockette;
				parse_type71 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type71_head - 1st entry in table; defined in structures.h,
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_timeh
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
				11/04/88  Dennis O'Neill  fixed bad call to parse_int (reported
				                          by L. Hwang, CalTech, 11/01/88)
				11/04/88  Dennis O'Neill  replaced "~" by STRINGTERM
				11/21/88  Dennis O'Neill  Production release 1.0
				08/19/89  Dennis O'Neill  streamlined linked list construction
				07/10/92  Allen Nance     Added fields 12,13,14 for Seed ver 2.2
				04/26/93  Allen Nance     Chenged fields 12,13,14 to ver 2.3
*/

#include "rdseed.h"

extern int Seed_flag;

/* this needs to be allocated here so it keeps its value between calls */
struct type71 *type71_tail = NULL;					/* end of linked list */

void parse_type71 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type71 *type71; 							/* genl member of table */
	int i;										/*counter */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

	if (Seed_flag)
		update_type71(blockette);
		

/*                 +========================================+                 */
/*=================|  allocate space and initialize a type71|=================*/
/*                 +========================================+                 */

	type71 = (struct type71 *) alloc_linklist_element (sizeof (struct type71),
		"parse_type71");	

/*                 +=======================================+                 */
/*=================|   recover information from the type71 |=================*/
/*                 +=======================================+                 */

	/* recover origin time of event */
	type71->origin_time = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover hypocenter source id code for lookup table */
	type71->source_code = parse_int (&blockette_ptr, 2);

	/* recover event latitude */
	type71->latitude = parse_double (&blockette_ptr, 10);

	/* recover event longitude */
	type71->longitude = parse_double (&blockette_ptr, 11);

	/* recover event depth */
	type71->depth = parse_double (&blockette_ptr, 7);

	/* recover number of magnitudes */
	type71->number_magnitudes = parse_int (&blockette_ptr, 2);

	/* allocate space for magnitudes */
	type71->magnitude = (struct type71sub *) alloc_linklist_element
		(sizeof (struct type71sub) * type71->number_magnitudes,
		"parse_type71->magnitude");

	/* recover magnitudes */
	for (i = 0; i < type71->number_magnitudes; i++)
	{
		type71->magnitude[i].magnitude = parse_double (&blockette_ptr, 5);
		type71->magnitude[i].type = parse_varlstr (&blockette_ptr, STRINGTERM);
		type71->magnitude[i].source_code = parse_int (&blockette_ptr, 2);
	}

	/* recover region and location info for SEED version 2.3 */
	if (type10.version >= 2.3)
	{
		type71->seismic_region   = parse_int (&blockette_ptr, 3);
		type71->seismic_location = parse_int (&blockette_ptr, 4);
		type71->region_name      = parse_varlstr (&blockette_ptr, STRINGTERM);
	}
	else
	{
		type71->seismic_region   = 0;
		type71->seismic_location = 0;
		type71->region_name      = NULL;
	}

	type71->type72_head = NULL;
	type71->type72_tail = NULL;
	current_origin = type71;
	

/*                 +=========================================+                 */
/*=================|   link the type71 into the type31 table |=================*/
/*                 +=========================================+                 */

	append_linklist_element (type71, type71_head, type71_tail);

}
