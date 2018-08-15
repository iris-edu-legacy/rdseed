/* UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED */
/*      Data of this type were unavailable when this process was written     */

/*===========================================================================*/
/* SEED reader     |               parse_type56               |  station header */
/*===========================================================================*/
/*
	Name:		parse_type56
	Purpose:	parse a generic response (type 56) blockette into a 
				globally-available table
	Usage:		void parse_type56 ();
				char *blockette;
				parse_type56 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type56_head - 1st entry in table; defined in structures.h,
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
				05/11/94  CL			  took out malloc if zero bytes
*/

#include "rdseed.h"

void parse_type56 (blockette)
char *blockette;						/* ptr to start of blockette */
{
	char *blockette_ptr;					/* ptr to inside blockette */
	struct type56 *type56; 					/* genl member of table */
	int i;							/* counter */
	struct response *response;				/* channel response ptr */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|  allocate space and initialize a type56  |=================*/
/*                 +=======================================+                 */

	type56 = (struct type56 *) alloc_linklist_element (sizeof (struct type56),
		"parse_type56");	

/*                 +=======================================+                 */
/*=================|   recover information from the type56    |=================*/
/*                 +=======================================+                 */

	/* recover cascade sequence number */
	type56->stage = parse_int (&blockette_ptr, 2);

	/* recover response in units */
	type56->input_units_code = parse_int (&blockette_ptr, 3);

	/* recover response out units */
	type56->output_units_code = parse_int (&blockette_ptr, 3);

	/* recover number of numerators */
	type56->number_corners = parse_int (&blockette_ptr, 4);

	/* allocate space for corners */
	if (type56->number_corners != 0)
		type56->corner = (struct type56sub *) alloc_linklist_element
			(sizeof (struct type56sub) * type56->number_corners, "parse_type56->corner");

	/* recover corners */
	for (i = 0; i < type56->number_corners; i++)
	{
		type56->corner[i].frequency = parse_double (&blockette_ptr, 12);
		type56->corner[i].slope = parse_double (&blockette_ptr, 12);
	}

/*                 +=======================================+                 */
/*=================|    link the type56 into the type56 table    |=================*/
/*                 +=======================================+                 */

	/* initialize the pointer to the next record as end-of-list marker */
	type56->next = NULL;

	response = get_response ('G');
	response->ptr.type56 = type56;
}
