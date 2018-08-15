/*===========================================================================*/
/* SEED reader     |               parse_type61               |  station header */
/*===========================================================================*/
/*
	Name:		parse_type61
	Purpose:	parse a response (coefficients) (type 61) blockette
				into a globally-available table
	Usage:		void parse_type61 ();
				char *blockette;
				parse_type61 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type61_head - 1st entry in table; defined in structures.h,
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_stationh
	Calls to:	parse_int - get an integer from a blockette 
				parse_double - get a double from a blockette 
				parse_nchar - get a fixed-length string from a blockette 
	Algorithm:	parse the blockette into a globally-available table
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Allen Nance
	Revisions:	07/10/92  Allen Nance
				adapted from parse_54 for SEED version 2.2
*/

#include "rdseed.h"

void parse_type61 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type61 *type61; 							/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|  allocate space and initialize a type61  |=================*/
/*                 +=======================================+                 */

	type61 = (struct type61 *) alloc_linklist_element (sizeof (struct type61),
		"parse_type61");	

/*                 +=======================================+                 */
/*=================|   recover information from the type61    |=================*/
/*                 +=======================================+                 */

	/* recover cascade sequence number */
	type61->stage = parse_int (&blockette_ptr, 2);

	/* recover cascade sequence number */
	type61->name = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover response type */
	type61->symmetry_code = *(parse_nchar(&blockette_ptr, 1 ));

	/* recover response in units */
	type61->input_units_code = parse_int (&blockette_ptr, 3);

	/* recover response out units */
	type61->output_units_code = parse_int (&blockette_ptr, 3);

	/* recover number of numerators */
	type61->number_numerators = parse_int (&blockette_ptr, 4);

	/* allocate space for numerator coefficients */
	type61->numerator = (double *) alloc_linklist_element
		(sizeof (double) * type61->number_numerators,
		"parse_type61->numerator");

	/* recover numerator coefficients */
	for (i = 0; i < type61->number_numerators; i++)
	{
		type61->numerator[i] = parse_double (&blockette_ptr, 14);
	}

/*                 +=======================================+                 */
/*=================|    link the type61 into the type61 table    |=================*/
/*                 +=======================================+                 */

	/* initialize the pointer to the next record as end-of-list marker */
	type61->next = NULL;

	response = get_response ('F');
	response->ptr.type61 = type61;
}
