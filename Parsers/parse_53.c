/*===========================================================================*/
/* SEED reader     |               parse_type53              |  station header */
/*===========================================================================*/
/*
	Name:		parse_type53
	Purpose:	parse a response (poles and zeroes) (type 53) blockette
				into a globally-available table
	Usage:		void parse_type53 ();
				char *blockette;
				parse_type53 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type53_head - 1st entry in table; defined in structures.h,
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
				05/11/94  CL			  took out the malloc if num==0
*/

#include "rdseed.h"

void parse_type53 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type53 *type53; 							/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================| allocate space and initialize a type53  |=================*/
/*                 +=======================================+                 */

	type53 = (struct type53 *) alloc_linklist_element (sizeof (struct type53),
		"parse_type53");	

/*                 +=======================================+                 */
/*=================|   recover information from the type53   |=================*/
/*                 +=======================================+                 */

	/* recover transfer function type */
	type53->transfer = parse_nchar (&blockette_ptr, 1);

	/* recover cascade sequence number */
	type53->stage = parse_int (&blockette_ptr, 2);

	/* recover response in units (0 if none) */
	type53->input_units_code = parse_int (&blockette_ptr, 3);

	/* recover response out units */
	type53->output_units_code = parse_int (&blockette_ptr, 3);

	/* recover AO normalization factor (1.0 if none) */
	type53->ao_norm = parse_double (&blockette_ptr, 12);

	/* recover normalization frequency */
	type53->norm_freq = parse_double (&blockette_ptr, 12);

	/* recover number of complex zeroes */
	type53->number_zeroes = parse_int (&blockette_ptr, 3);

	/* allocate space for complex zeroes */
	if (type53->number_zeroes != 0)
		type53->zero = (struct type53sub *) alloc_linklist_element 
			(sizeof (struct type53sub) * type53->number_zeroes, "parse_type53->zero");

	/* recover complex zeroes */
	for (i = 0; i < type53->number_zeroes; i++)
	{
		type53->zero[i].real = parse_double (&blockette_ptr, 12);
		type53->zero[i].imag = parse_double (&blockette_ptr, 12);
		type53->zero[i].real_error = parse_double (&blockette_ptr, 12);
		type53->zero[i].imag_error = parse_double (&blockette_ptr, 12);
	}

	/* recover number of complex poles */
	type53->number_poles = parse_int (&blockette_ptr, 3);

	/* allocate space for complex poles */
	if (type53->number_poles != 0)
		type53->pole = (struct type53sub *) alloc_linklist_element
			(sizeof (struct type53sub) * type53->number_poles, "parse_type53->pole");

	/* recover complex poles */
	for (i = 0; i < type53->number_poles; i++)
	{
		type53->pole[i].real = parse_double (&blockette_ptr, 12);

		type53->pole[i].imag = parse_double (&blockette_ptr, 12);
		type53->pole[i].real_error = parse_double (&blockette_ptr, 12);
		type53->pole[i].imag_error = parse_double (&blockette_ptr, 12);

		if ((!strcmp(type53->transfer,"A") || !strcmp(type53->transfer,"B")) &&
		     type53->pole[i].real > 0.0) 
			fprintf(stderr, "Warning! Station/channel: %s/%s has a pole with a real value > 0.0: real value=%6.2f\n", 
				current_station->station, 
				current_channel->channel,
				type53->pole[i].real);

	}

/*                 +=======================================+                 */
/*=================|   link the type53 into the type53 table   |=================*/
/*                 +=======================================+                 */

	/* initialize the pointer to the next record as end-of-list marker */
	type53->next = NULL;

	response = get_response ('P');
	response->ptr.type53 = type53;	
}
