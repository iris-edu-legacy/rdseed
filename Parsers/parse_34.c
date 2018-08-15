/*===========================================================================*/
/* SEED reader     |               parse_type34               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		parse_type34
	Purpose:	parse a units abbreviation (type 34) blockette into a 
				globally-available structure
	Usage:		void parse_type34 ();
				char *blockette;
				parse_type34 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char - allocated in globals.h
				type34_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_abbrevdic
	Calls to:	parse_int - get an integer from a blockette 
				parse_varlstr - get a variable-length string from a blockette
	Algorithm:	parse the blockette; report the results
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/04/88  Dennis O'Neill  replaced "~" by STRINGTERM in
				                          parse_varlstr calls
				11/21/88  Dennis O'Neill  Production release 1.0
				08/19/89  Dennis O'Neill  streamlined linked list construction
*/

#include "rdseed.h"

/* this needs to be allocated here so it keeps its value between calls */
struct type34 *type34_tail = NULL;					/* end of linked list */

void parse_type34 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type34 *type34;							/* genl entry in table */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================| allocate space and initialize a type31  |=================*/
/*                 +=======================================+                 */

	type34 = (struct type34 *) alloc_linklist_element (sizeof (struct type34),
		"parse_type34");	

/*                 +=======================================+                 */
/*=================|   recover information from the type34    |=================*/
/*                 +=======================================+                 */

	memset((char *)type34, 0, sizeof(struct type34));

	/* recover unit code */
	type34->code = parse_int (&blockette_ptr, 3);

	/* recover unit name */
	type34->name = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover unit description, allowing for zero length */
	if (*blockette_ptr != '~')
		type34->description = parse_varlstr (&blockette_ptr, STRINGTERM);
	else {
		type34->description = NULL;
		blockette_ptr += 1;
	}

/*                 +=======================================+                 */
/*=================|    link the type34 into the type34 table    |=================*/
/*                 +=======================================+                 */

	append_linklist_element (type34, type34_head, type34_tail);

}
