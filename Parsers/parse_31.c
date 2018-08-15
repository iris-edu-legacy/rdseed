/*===========================================================================*/
/* SEED reader     |             parse_type31              |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		parse_type31
	Purpose:	parse a comment description dictionary (type 31) blockette
				into a globally-available table
	Usage:		void parse_type31 ();
				char *blockette;
				parse_type31 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type31_head - 1st entry in table; defined in structures.h,
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_abbrevdic
	Calls to:	parse_int - get an integer from a blockette 
				parse_nchar - get a fixed-length string from a blockette 
				parse_varlstr - get a variable-length string from a blockette
	Algorithm:	parse the blockette
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
struct type31 *type31_tail = NULL;					/* end of linked list */

void parse_type31 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type31 *type31; 							/* genl member of table */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================| allocate space and initialize a type31|=================*/
/*                 +=======================================+                 */

	type31 = (struct type31 *) alloc_linklist_element (sizeof (struct type31),
		"parse_type31");	

/*                 +=======================================+                 */
/*=================|  recover information from the type31  |=================*/
/*                 +=======================================+                 */

	/* recover comment code id */
	type31->code = parse_int (&blockette_ptr, 4);

	/* recover comment class code */
	type31->class = parse_nchar (&blockette_ptr, 1);

	/* recover comment text */
	type31->comment = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover units of comment */
	type31->units = parse_int (&blockette_ptr, 3);

/*                 +=======================================+                 */
/*=================| link the type31 into the type31 table |=================*/
/*                 +=======================================+                 */

	append_linklist_element (type31, type31_head, type31_tail);

}
