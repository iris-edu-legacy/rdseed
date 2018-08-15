/*===========================================================================*/
/* SEED reader     |              parse_type45             |  station header */
/*===========================================================================*/
/*
	Name:		parse_type45
	Purpose:	parse a response (list) (type 55) blockette into a 
				globally-available table
	Usage:		void parse_type45 ();
				char *blockette;
				parse_type45 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type45_head - 1st entry in table; defined in structures.h,
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
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				08/19/89  Dennis O'Neill  streamlined linked list construction
				05/11/94  CL			  took out malloc if 0 bytes needed
*/

#include "rdseed.h"

void parse_type45 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type45 *type45; 							/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */
    static struct type45 *type45_tail = NULL;   /* end of linked list */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|allocate space and initialize a type45 |=================*/
/*                 +=======================================+                 */

	type45 = (struct type45 *) alloc_linklist_element (sizeof (struct type45),
		"parse_type45");	

/*                 +=======================================+                 */
/*=================|  recover information from the type45  |=================*/
/*                 +=======================================+                 */

	/* recover response lookup code */
	type45->response_code = parse_int (&blockette_ptr, 4);

	/* recover response name */
	type45->name = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover response out units */
	type45->input_units_code = parse_int (&blockette_ptr, 3);

	/* recover response out units */
	type45->output_units_code = parse_int (&blockette_ptr, 3);

	/* recover number of numerators */
	type45->number_responses = parse_int (&blockette_ptr, 4);

	/* allocate space for responses */
	if (type45->number_responses != 0)
		type45->response = (struct type45sub *) alloc_linklist_element
								(sizeof (struct type45sub) * 
										type45->number_responses, 
							"parse_type45->response");

	/* recover responses */
	for (i = 0; i < type45->number_responses; i++)
	{
		type45->response[i].frequency = parse_double (&blockette_ptr, 12);
		type45->response[i].amplitude = parse_double (&blockette_ptr, 12);
		type45->response[i].amplitude_error = parse_double (&blockette_ptr, 12);
		type45->response[i].phase = parse_double (&blockette_ptr, 12);
		type45->response[i].phase_error = parse_double (&blockette_ptr, 12);
	}

/*                 +=======================================+                 */
/*=================|link the type45 into the type45 table  |=================*/
/*                 +=======================================+                 */

    append_linklist_element (type45, type45_head, type45_tail);

}
