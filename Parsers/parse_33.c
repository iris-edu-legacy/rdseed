/*===========================================================================*/
/* SEED reader     |               parse_type33               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		parse_type33
	Purpose:	parse a generic abbreviation (type 33) blockette into a
				globally-available structure
	Usage:		void parse_type33 ();
				char *blockette;
				parse_type33 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char - allocated in globals.h
				type33_head - 1st entry in table; defined in structures.h,
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_abbrevdic
	Calls to:	parse_int - get an integer from a blockette 
				parse_varlstr - get a variable-length string from a blockette
	Algorithm:	parse the blockette; save the result in a table
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
struct type33 *type33_tail = NULL;					/* end of linked list */

void parse_type33 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type33 *type33;							/* genl member of table */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================| allocate space and initialize a type31  |=================*/
/*                 +=======================================+                 */

	type33 = (struct type33 *) alloc_linklist_element (sizeof (struct type33),
		"parse_type33");	

/*                 +=======================================+                 */
/*=================|   recover information from the type33    |=================*/
/*                 +=======================================+                 */

	/* recover description key code */
	type33->code = parse_int (&blockette_ptr, 3);

	/* recover abbreviation description */
	type33->abbreviation = parse_varlstr (&blockette_ptr, STRINGTERM);

/*                 +=======================================+                 */
/*=================|    link the type33 into the type33 table    |=================*/
/*                 +=======================================+                 */

	append_linklist_element (type33, type33_head, type33_tail);

}
