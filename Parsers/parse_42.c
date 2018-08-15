/*===========================================================================*/
/* SEED reader     |             parse_type42              |  station header */
/*===========================================================================*/
/*
	Name:		parse_type42
	Purpose:	parse a response (polynomial) (type 42) blockette
				into a globally-available table
	Usage:		void parse_type42 ();
				char *blockette;
				parse_type42 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type42_head - 1st entry in table; defined in structures.h,
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
	Revisions:	03/23/99 adapted from parse_41() for SEED version 2.2
*/

#include "rdseed.h"

void parse_type42 (blockette)
char *blockette;				/* ptr to start of blockette */
{
	char *blockette_ptr;			/* ptr to inside blockette */
	struct type42 *type42; 			/* genl member of table */
	int i;					/* counter */
	struct response *response;		/* channel response ptr */
    static struct type42 *type42_tail = NULL;   /* end of linked list */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|allocate space and initialize a type42 |=================*/
/*                 +=======================================+                 */

	type42 = (struct type42 *) alloc_linklist_element (sizeof (struct type42),
		"parse_type42");	

/*                 +=======================================+                 */
/*=================|  recover information from the type42  |=================*/
/*                 +=======================================+                 */

	/* recover response lookup code */
	type42->response_code = parse_int (&blockette_ptr, 4);

	/* recover response name */
	type42->name = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover transfer function type */
	type42->transfer_fct_type = *(parse_nchar(&blockette_ptr, 1 ));

	/* recover response in units */
	type42->input_units_code = parse_int (&blockette_ptr, 3);

	/* recover response out units */
	type42->output_units_code = parse_int (&blockette_ptr, 3);

	/* recover polynomial approximation type */
	type42->poly_approx_type = *(parse_nchar(&blockette_ptr, 1 ));

	/* recover valid frequency units */
	type42->valid_freq_units = *(parse_nchar(&blockette_ptr, 1 ));

	/* recover lower valid frequency bound */
	type42->lower_valid_freq = parse_double(&blockette_ptr, 12 );

	/* recover upper valid frequency bound */
	type42->upper_valid_freq = parse_double(&blockette_ptr, 12 );

	/* recover lower bound of approximation */
	type42->lower_bound_approx = parse_double(&blockette_ptr, 12 );

	/* recover upper bound of approximation */
	type42->upper_bound_approx = parse_double(&blockette_ptr, 12 );

	/* recover maximum absolute error */
	type42->max_abs_error = parse_double(&blockette_ptr, 12 );

	/* recover number of coefficients */
	type42->number_coefficients = parse_int (&blockette_ptr, 3);

	/* allocate space for coefficients */
	type42->coefficient = (struct type42sub *) alloc_linklist_element
		(sizeof (struct type42sub) * type42->number_coefficients,
		"parse_type42->coefficient");

	/* recover coefficients */
	for (i = 0; i < type42->number_coefficients; i++)
	{
		type42->coefficient[i].coefficient = parse_double (&blockette_ptr, 12);
		type42->coefficient[i].error = parse_double (&blockette_ptr, 12);
	}



/*                 +=======================================+                 */
/*=================| link the type42 into the type42 table |=================*/
/*                 +=======================================+                 */

    append_linklist_element (type42, type42_head, type42_tail);
}
