/*===========================================================================*/
/* SEED reader     |               parse_type62               |  station header */
/*===========================================================================*/
/*
	Name:		parse_type62
	Purpose:	parse a response (polynomial) (type 62) blockette
				into a globally-available table
	Usage:		void parse_type62 ();
				char *blockette;
				parse_type62 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type62_head - 1st entry in table; defined in structures.h,
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
	Author:		Stephane Zuzlewski
	Revisions:	02/23/99  adapted from parse_61 for SEED version 2.2
*/

#include "rdseed.h"

void parse_type62 (blockette)
char *blockette;				/* ptr to start of blockette */
{
	char *blockette_ptr;			/* ptr to inside blockette */
	struct type62 *type62; 			/* genl member of table */
	int i;					/* counter */
	struct response *response;		/* channel response ptr */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|  allocate space and initialize a type62  |=================*/
/*                 +=======================================+                 */

	type62 = (struct type62 *) alloc_linklist_element (sizeof (struct type62),
		"parse_type62");	

/*                 +=======================================+                 */
/*=================|   recover information from the type62    |=================*/
/*                 +=======================================+                 */

	/* recover transfer function type */
	type62->transfer_fct_type = *(parse_nchar(&blockette_ptr, 1 ));

	/* recover cascade sequence number */
	type62->stage = parse_int (&blockette_ptr, 2);

	/* recover response in units */
	type62->input_units_code = parse_int (&blockette_ptr, 3);

	/* recover response out units */
	type62->output_units_code = parse_int (&blockette_ptr, 3);

	/* recover polynomial approximation type */
	type62->poly_approx_type = *(parse_nchar(&blockette_ptr, 1 ));

	/* recover valid frequency units */
	type62->valid_freq_units = *(parse_nchar(&blockette_ptr, 1 ));

	/* recover lower valid frequency bound */
	type62->lower_valid_freq = parse_double(&blockette_ptr, 12 );

	/* recover upper valid frequency bound */
	type62->upper_valid_freq = parse_double(&blockette_ptr, 12 );

	/* recover lower bound of approximation */
	type62->lower_bound_approx = parse_double(&blockette_ptr, 12 );

	/* recover upper bound of approximation */
	type62->upper_bound_approx = parse_double(&blockette_ptr, 12 );

	/* recover maximum absolute error */
	type62->max_abs_error = parse_double(&blockette_ptr, 12 );

	/* recover number of coefficients */
	type62->number_coefficients = parse_int (&blockette_ptr, 3);

	/* allocate space for coefficients */
	type62->coefficient = (struct type62sub *) alloc_linklist_element
		(sizeof (struct type62sub) * type62->number_coefficients,
		"parse_type62->coefficient");

	/* recover coefficients */
	for (i = 0; i < type62->number_coefficients; i++)
	{
		type62->coefficient[i].coefficient = parse_double (&blockette_ptr, 12);
		type62->coefficient[i].error = parse_double (&blockette_ptr, 12);
	}

/*                 +=======================================+                 */
/*=================|    link the type62 into the type62 table    |=================*/
/*                 +=======================================+                 */

        /* initialize the pointer to the next record as end-of-list marker */
        type62->next = NULL;

        response = get_response ('O');
        response->ptr.type62 = type62;

}
