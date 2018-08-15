/*===========================================================================*/
/* SEED reader     |             parse_type43              |  station header */
/*===========================================================================*/
/*
	Name:		parse_type43
	Purpose:	parse a response (poles and zeroes) (type 53) blockette
				into a globally-available table
	Usage:		void parse_type43 ();
				char *blockette;
				parse_type43 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type43_head - 1st entry in table; defined in structures.h,
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
				05/11/94  CL - took out malloc if zero bytes needed
*/

#include "rdseed.h"


void parse_type43 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type43 *type43; 							/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */
	static struct type43 *type43_tail = NULL;   /* end of linked list */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|allocate space and initialize a type43 |=================*/
/*                 +=======================================+                 */

	type43 = (struct type43 *) alloc_linklist_element (sizeof (struct type43),
		"parse_type43");	

/*                 +=======================================+                 */
/*=================|  recover information from the type43  |=================*/
/*                 +=======================================+                 */

	/* recover response lookup code */
	type43->response_code = parse_int (&blockette_ptr, 4);

	/* recover response name */
	type43->name = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover response type */
	type43->response_type = *(parse_nchar(&blockette_ptr, 1 ));

	/* recover response in units (0 if none) */
	type43->input_units_code = parse_int (&blockette_ptr, 3);

	/* recover response out units */
	type43->output_units_code = parse_int (&blockette_ptr, 3);

	/* recover AO normalization factor (1.0 if none) */
	type43->ao_norm = parse_double (&blockette_ptr, 12);

	/* recover normalization frequency */
	type43->norm_freq = parse_double (&blockette_ptr, 12);

	/* recover number of complex zeroes */
	type43->number_zeroes = parse_int (&blockette_ptr, 3);

	/* allocate space for complex zeroes */
	if (type43->number_zeroes != 0)	
		type43->zero = (struct type43sub *) alloc_linklist_element 
							(sizeof (struct type43sub) * 
									type43->number_zeroes, 
							"parse_type43->zero");

	/* recover complex zeroes */
	for (i = 0; i < type43->number_zeroes; i++)
	{
		type43->zero[i].real = parse_double (&blockette_ptr, 12);
		type43->zero[i].imag = parse_double (&blockette_ptr, 12);
		type43->zero[i].real_error = parse_double (&blockette_ptr, 12);
		type43->zero[i].imag_error = parse_double (&blockette_ptr, 12);
	}

	/* recover number of complex poles */
	type43->number_poles = parse_int (&blockette_ptr, 3);

	/* allocate space for complex poles */
	if (type43->number_poles != 0)
		type43->pole = (struct type43sub *) alloc_linklist_element
						(sizeof (struct type43sub) * 
							type43->number_poles, 
						"parse_type43->pole");

	/* recover complex poles */
	for (i = 0; i < type43->number_poles; i++)
	{
		type43->pole[i].real = parse_double (&blockette_ptr, 12);
		type43->pole[i].imag = parse_double (&blockette_ptr, 12);
		type43->pole[i].real_error = parse_double (&blockette_ptr, 12);
		type43->pole[i].imag_error = parse_double (&blockette_ptr, 12);
	}

/*                 +=======================================+                 */
/*=================| link the type43 into the type43 table |=================*/
/*                 +=======================================+                 */

	append_linklist_element (type43, type43_head, type43_tail);
}
