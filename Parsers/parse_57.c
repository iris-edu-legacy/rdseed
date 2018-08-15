/* UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED */
/*      Data of this type were unavailable when this process was written     */

/*===========================================================================*/
/* SEED reader     |            parse_type57               |  station header */
/*===========================================================================*/
/*
	Name:		parse_type57
	Purpose:	parse a decimation (type 57) blockette into a 
				globally-available table
	Usage:		void parse_type57 ();
				char *blockette;
				parse_type57 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type57_head - 1st entry in table; defined in structures.h,
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_stationh
	Calls to:	parse_int - get an integer from a blockette 
				parse_double - get a double from a blockette 
	Algorithm:	parse the blockette into a globally-available table
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	04/30/90  Dennis O'Neill  Initial release
*/

#include "rdseed.h"

void parse_type57 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type57 *type57; 						/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================| allocate space and initialize a type57|=================*/
/*                 +=======================================+                 */

	type57 = (struct type57 *) alloc_linklist_element (sizeof (struct type57),
		"parse_type57");	

/*                 +=======================================+                 */
/*=================|  recover information from the type57  |=================*/
/*                 +=======================================+                 */

	/* recover cascade sequence number */
	type57->stage = parse_int (&blockette_ptr, 2);

	/* recover input sample rate */
	type57->input_sample_rate = parse_double (&blockette_ptr, 10);

	/* recover decimation factor */
	type57->decimation_factor = parse_int (&blockette_ptr, 5);

	/* recover decimation offset */
	type57->decimation_offset = parse_int (&blockette_ptr, 5);

	/* recover delay */
	type57->delay = parse_double (&blockette_ptr, 11);

	/* recover delay */
	type57->correction = parse_double (&blockette_ptr, 11);

/*                 +=======================================+                 */
/*=================| link the type57 into the type57 table |=================*/
/*                 +=======================================+                 */

	/* initialize the pointer to the next record as end-of-list marker */
	type57->next = NULL;

	response = get_response ('D');
	response->ptr.type57 = type57;
}
