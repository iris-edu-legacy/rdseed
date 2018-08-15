/*===========================================================================*/
/* SEED reader     |              parse_type60             |  station header */
/*===========================================================================*/
/*
	Name:		parse_type60
	Purpose:	parse a response reference (type 60) blockette
				into a globally-available table
	Usage:		void parse_type60 ();
				char *blockette;
				parse_type60 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type60_head - 1st entry in table; defined in structures.h,
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
	Author:		Sue Schoch
	Revisions:	06/05/90  Sue Schoch      Initial preliminary release in 2.0
			    05/11/94  CL		took out malloc if zero bytes needed
*/

#include "rdseed.h"

void parse_type60 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type60 *type60;						/* genl member of table */
	int i,j;									/* counter */
	struct response *response;					/* channel response ptr */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|allocate space and initialize a type60 |=================*/
/*                 +=======================================+                 */

	type60 = (struct type60 *) alloc_linklist_element (sizeof (struct type60),
		"parse_type60");	

/*                +=======================================+                  */
/*================|  recover information from the type60  |==================*/
/*                +=======================================+                  */

	/* recover number of stages */
	type60->number_stages = parse_int (&blockette_ptr, 2);

	/* allocate space for stage values */
	if (type60->number_stages != 0)
		type60->stage = (struct type60sub1 *) alloc_linklist_element
			(sizeof (struct type60sub1) * type60->number_stages,
				"parse_type60->stage values");

	/* recover stage values */
	for (i = 0; i < type60->number_stages; i++)
	{
		type60->stage[i].value = parse_int( &blockette_ptr, 2);
	
		/* recover number of response references */
		type60->stage[i].number_responses = parse_int (&blockette_ptr, 2);
	
		/* allocate space for references */
		if (type60->stage[i].number_responses != 0)
			type60->stage[i].response = 
				(struct type60sub2 *) alloc_linklist_element
									(sizeof (struct type60sub2) * 
										type60->stage[i].number_responses,
									"parse_type60->response references");
	
		/* recover response references */
		for (j = 0; j < type60->stage[i].number_responses; j++)
		{
			type60->stage[i].response[j].reference= parse_int(&blockette_ptr,4);
		}
	}

/*                 +=======================================+                 */
/*=================| link the type60 into the type60 table |=================*/
/*                 +=======================================+                 */

	/* initialize the pointer to the next record as end-of-list marker */
	type60->next = NULL;

	response = get_response ('R');
	response->ptr.type60 = type60;
}
