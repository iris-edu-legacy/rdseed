/*===========================================================================*/
/* SEED reader     |               parse_type54               |  station header */
/*===========================================================================*/
/*
	Name:		parse_type54
	Purpose:	parse a response (coefficients) (type 54) blockette
				into a globally-available table
	Usage:		void parse_type54 ();
				char *blockette;
				parse_type54 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type54_head - 1st entry in table; defined in structures.h,
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
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				08/19/89  Dennis O'Neill  streamlined linked list construction
				05/11/94  CL 			  took out malloc if bytes to malloc are
										  zero
*/

#include "rdseed.h"

void parse_type54 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type54 *type54; 							/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|  allocate space and initialize a type54  |=================*/
/*                 +=======================================+                 */

	type54 = (struct type54 *) alloc_linklist_element (sizeof (struct type54),
		"parse_type54");	

/*                 +=======================================+                 */
/*=================|   recover information from the type54    |=================*/
/*                 +=======================================+                 */

	/* recover transfer function type */
	type54->transfer = parse_nchar (&blockette_ptr, 1);

	/* recover cascade sequence number */
	type54->stage = parse_int (&blockette_ptr, 2);

	/* recover response in units */
	type54->input_units_code = parse_int (&blockette_ptr, 3);

	/* recover response out units */
	type54->output_units_code = parse_int (&blockette_ptr, 3);

	/* recover number of numerators */
	type54->number_numerators = parse_int (&blockette_ptr, 4);

	/* allocate space for numerator coefficients */
	if (type54->number_numerators != 0)
	type54->numerator = (struct type54sub *) alloc_linklist_element
		(sizeof (struct type54sub) * type54->number_numerators,
		"parse_type54->numerator");

	/* recover numerator coefficients */
	for (i = 0; i < type54->number_numerators; i++)
	{
		type54->numerator[i].coefficient = parse_double (&blockette_ptr, 12);
		type54->numerator[i].error = parse_double (&blockette_ptr, 12);
	}

	/* recover number of denominator coefficients */
	type54->number_denominators = parse_int (&blockette_ptr, 4);

	/* allocate space for denominator coefficients */
	if (type54->number_denominators != 0)
		type54->denominator = (struct type54sub *) alloc_linklist_element
				(sizeof (struct type54sub) * type54->number_denominators,
					"parse_type54->denominator");

	/* recover denominator coefficients */
	for (i = 0; i < type54->number_denominators; i++)
	{
		type54->denominator[i].coefficient = parse_double (&blockette_ptr, 12);
		type54->denominator[i].error = parse_double (&blockette_ptr, 12);
	}

/*                 +=======================================+                 */
/*=================|    link the type54 into the type54 table    |=================*/
/*                 +=======================================+                 */

	/* initialize the pointer to the next record as end-of-list marker */
	type54->next = NULL;

	response = get_response ('C');
	response->ptr.type54 = type54;
}
