/*===========================================================================*/
/* SEED reader     |             parse_type46              |  station header */
/*===========================================================================*/
/*
	Name:		parse_type46
	Purpose:	parse a generic response (type 56) blockette into a 
				globally-available table
	Usage:		void parse_type46 ();
				char *blockette;
				parse_type46 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type46_head - 1st entry in table; defined in structures.h,
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
				05/11/94  CL		  	  took out malloc if 0 bytes
*/

#include "rdseed.h"

void parse_type46 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type46 *type46; 							/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */
    static struct type46 *type46_tail = NULL;   /* end of linked list */


	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|allocate space and initialize a type46 |=================*/
/*                 +=======================================+                 */

	type46 = (struct type46 *) alloc_linklist_element (sizeof (struct type46),
		"parse_type46");	

/*                 +=======================================+                 */
/*=================|  recover information from the type46  |=================*/
/*                 +=======================================+                 */

	/* recover response lookup code */
	type46->response_code = parse_int (&blockette_ptr, 4);

	/* recover response name */
	type46->name = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover response in units */
	type46->input_units_code = parse_int (&blockette_ptr, 3);

	/* recover response out units */
	type46->output_units_code = parse_int (&blockette_ptr, 3);

	/* recover number of numerators */
	type46->number_corners = parse_int (&blockette_ptr, 4);

	/* allocate space for corners */
	if (type46->number_corners != 0)
		type46->corner = (struct type46sub *) 
							alloc_linklist_element
								(sizeof (struct type46sub) * 
									type46->number_corners, 
								"parse_type46->corner");

	/* recover corners */
	for (i = 0; i < type46->number_corners; i++)
	{
		type46->corner[i].frequency = parse_double (&blockette_ptr, 12);
		type46->corner[i].slope = parse_double (&blockette_ptr, 12);
	}

/*                 +=======================================+                 */
/*=================| link the type46 into the type46 table |=================*/
/*                 +=======================================+                 */

    append_linklist_element (type46, type46_head, type46_tail);

}
