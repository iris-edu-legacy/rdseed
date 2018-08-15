/*===========================================================================*/
/* SEED reader     |             parse_type41              |  station header */
/*===========================================================================*/
/*
	Name:		parse_type41
	Purpose:	parse a response (coefficients) (type 54) blockette
				into a globally-available table
	Usage:		void parse_type41 ();
				char *blockette;
				parse_type41 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type41_head - 1st entry in table; defined in structures.h,
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
	Revisions:	07/10/92 adapted from parse_44() for SEED version 2.2
				02/16/94 CL - bug fix, changed field width of # of numerator
				05/11/94 CL - took out malloc if zero bytes needed
*/

#include "rdseed.h"

void parse_type41 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type41 *type41; 							/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */
    static struct type41 *type41_tail = NULL;   /* end of linked list */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|allocate space and initialize a type41 |=================*/
/*                 +=======================================+                 */

	type41 = (struct type41 *) alloc_linklist_element (sizeof (struct type41),
		"parse_type41");	

/*                 +=======================================+                 */
/*=================|  recover information from the type41  |=================*/
/*                 +=======================================+                 */

	/* recover response lookup code */
	type41->response_code = parse_int (&blockette_ptr, 4);

	/* recover response name */
	type41->name = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover response type */
	type41->symmetry_code = *(parse_nchar(&blockette_ptr, 1 ));

	/* recover response in units */
	type41->input_units_code = parse_int (&blockette_ptr, 3);

	/* recover response out units */
	type41->output_units_code = parse_int (&blockette_ptr, 3);

	/* recover number of numerators */
	type41->number_numerators = parse_int (&blockette_ptr, 4);

	/* allocate space for numerator coefficients */
	if (type41->number_numerators != 0)
		type41->numerator = (double *) alloc_linklist_element
			(sizeof (double) * type41->number_numerators,
				"parse_type41->numerator");

	/* recover numerator coefficients */
	for (i = 0; i < type41->number_numerators; i++)
	{
		type41->numerator[i] = parse_double (&blockette_ptr, 14);
	}

/*                 +=======================================+                 */
/*=================| link the type41 into the type41 table |=================*/
/*                 +=======================================+                 */

    append_linklist_element (type41, type41_head, type41_tail);
}
