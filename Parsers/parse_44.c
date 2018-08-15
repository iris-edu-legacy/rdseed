/*===========================================================================*/
/* SEED reader     |             parse_type44              |  station header */
/*===========================================================================*/
/*
	Name:		parse_type44
	Purpose:	parse a response (coefficients) (type 54) blockette
				into a globally-available table
	Usage:		void parse_type44 ();
				char *blockette;
				parse_type44 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type44_head - 1st entry in table; defined in structures.h,
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
				06/14/90  Sue Schoch      added response type field
				05/11/94  CL 			  took out malloc if 0 bytes needed
*/

#include "rdseed.h"

void parse_type44 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type44 *type44; 							/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */
    static struct type44 *type44_tail = NULL;   /* end of linked list */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|allocate space and initialize a type44 |=================*/
/*                 +=======================================+                 */

	type44 = (struct type44 *) alloc_linklist_element (sizeof (struct type44),
		"parse_type44");	

/*                 +=======================================+                 */
/*=================|  recover information from the type44  |=================*/
/*                 +=======================================+                 */

	/* recover response lookup code */
	type44->response_code = parse_int (&blockette_ptr, 4);

	/* recover response name */
	type44->name = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover response type */
	type44->response_type = *(parse_nchar(&blockette_ptr, 1 ));

	/* recover response in units */
	type44->input_units_code = parse_int (&blockette_ptr, 3);

	/* recover response out units */
	type44->output_units_code = parse_int (&blockette_ptr, 3);

	/* recover number of numerators */
	type44->number_numerators = parse_int (&blockette_ptr, 4);

	/* allocate space for numerator coefficients */
	if (type44->number_numerators != 0)
		type44->numerator = (struct type44sub *) alloc_linklist_element
							(sizeof (struct type44sub) * 
								type44->number_numerators,
							"parse_type44->numerator");

	/* recover numerator coefficients */
	for (i = 0; i < type44->number_numerators; i++)
	{
		type44->numerator[i].coefficient = parse_double (&blockette_ptr, 12);
		type44->numerator[i].error = parse_double (&blockette_ptr, 12);
	}

	/* recover number of denominator coefficients */
	type44->number_denominators = parse_int (&blockette_ptr, 4);

	/* allocate space for denominator coefficients */
	if (type44->number_denominators != 0)
		type44->denominator = (struct type44sub *) alloc_linklist_element
								(sizeof (struct type44sub) * 
										type44->number_denominators,
								"parse_type44->denominator");

	/* recover denominator coefficients */
	for (i = 0; i < type44->number_denominators; i++)
	{
		type44->denominator[i].coefficient = parse_double (&blockette_ptr, 12);
		type44->denominator[i].error = parse_double (&blockette_ptr, 12);
	}

/*                 +=======================================+                 */
/*=================| link the type44 into the type44 table |=================*/
/*                 +=======================================+                 */

    append_linklist_element (type44, type44_head, type44_tail);
}
