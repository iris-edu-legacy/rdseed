/* UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED */
/*      Data of this type were unavailable when this process was written     */

/*===========================================================================*/
/* SEED reader     |               parse_type55               |  station header */
/*===========================================================================*/
/*
	Name:		parse_type55
	Purpose:	parse a response (list) (type 55) blockette into a 
				globally-available table
	Usage:		void parse_type55 ();
				char *blockette;
				parse_type55 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type55_head - 1st entry in table; defined in structures.h,
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
				05/11/94  CL			  took out the malloc if bytes==0
*/

#include "rdseed.h"

void parse_type55 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type55 *type55; 							/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|  allocate space and initialize a type55  |=================*/
/*                 +=======================================+                 */

	type55 = (struct type55 *) alloc_linklist_element (sizeof (struct type55),
		"parse_type55");	

/*                 +=======================================+                 */
/*=================|   recover information from the type55    |=================*/
/*                 +=======================================+                 */

	/* recover cascade sequence number */
	type55->stage = parse_int (&blockette_ptr, 2);

	/* recover response in units */
	type55->input_units_code = parse_int (&blockette_ptr, 3);

	/* recover response out units */
	type55->output_units_code = parse_int (&blockette_ptr, 3);

	/* recover number of numerators */
	type55->number_responses = parse_int (&blockette_ptr, 4);

	/* allocate space for responses */
	if (type55->number_responses != 0)
		type55->response = (struct type55sub *) alloc_linklist_element
		(sizeof (struct type55sub) * type55->number_responses, "parse_type55->response");

	/* recover responses */
	for (i = 0; i < type55->number_responses; i++)
	{
		type55->response[i].frequency = parse_double (&blockette_ptr, 12);
		type55->response[i].amplitude = parse_double (&blockette_ptr, 12);
		type55->response[i].amplitude_error = parse_double (&blockette_ptr, 12);
		type55->response[i].phase = parse_double (&blockette_ptr, 12);
		type55->response[i].phase_error = parse_double (&blockette_ptr, 12);
	}

/*                 +=======================================+                 */
/*=================|    link the type55 into the type55 table    |=================*/
/*                 +=======================================+                 */

	/* initialize the pointer to the next record as end-of-list marker */
	type55->next = NULL;

	response = get_response ('L');
	response->ptr.type55 = type55;
}
