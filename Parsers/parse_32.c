/* UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED */
/*      Data of this type were unavailable when this process was written     */

/*===========================================================================*/
/* SEED reader     |               parse_type32              |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		parse_type32
	Purpose:	parse a cited source dictionary (type 32) blockette into a 
				globally-available structure
	Usage:		void parse_type32 ();
				char *blockette;
				parse_cdsb (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char - allocated in globals.h
				type32_head - 1st entry in table; defined in structures.h, 
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
struct type32 *type32_tail = NULL;					/* end of linked list */

void parse_type32 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type32 *type32;							/* genl member of table */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================| allocate space and initialize a type32  |=================*/
/*                 +=======================================+                 */

	type32 = (struct type32 *) alloc_linklist_element (sizeof (struct type32),
		"parse_type32");	

/*                 +=======================================+                 */
/*=================|   recover information from the type32   |=================*/
/*                 +=======================================+                 */

	/* recover source id */
	type32->code = parse_int (&blockette_ptr, 2);

	/* recover type32->author */
	type32->author = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover type32->catalog information */
	type32->catalog = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover type32->publisher */
	type32->publisher = parse_varlstr (&blockette_ptr, STRINGTERM);

/*                 +=======================================+                 */
/*=================|   link the type32 into the type32 table   |=================*/
/*                 +=======================================+                 */

	append_linklist_element (type32, type32_head, type32_tail);

}
