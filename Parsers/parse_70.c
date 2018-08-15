/*===========================================================================*/
/* SEED reader     |             parse_type70                |     time header */
/*===========================================================================*/
/*
	Name:		parse_type70
	Purpose:	parse a time span id (type 70) blockette into a
				globally-available table
	Usage:		void parse_type70 ();
				char *blockette;
				parse_type70 (blockette);
	Input:		blockette = pointer to a station comment blockette
	Output:		none
	Externals:	temp_char - allocated in globals.h
				type70_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_timeh
	Calls to:	parse_nchar - get a fixed-length string from a blockette
				parse_varlstr - get a variable-length string from a blockette
	Algorithm:	break the blockette into its parts, put them into a structure
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
struct type70 *type70_tail = NULL;					/* end of linked list */

void parse_type70 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type70 *type70; 							/* genl member of table */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================| allocate space and initialize a type70  |=================*/
/*                 +=======================================+                 */

	type70 = (struct type70 *) alloc_linklist_element (sizeof (struct type70),
		"parse_type70");	

/*                 +=======================================+                 */
/*=================|   recover information from the type70   |=================*/
/*                 +=======================================+                 */

	/* recover time span flag */
	type70->flag = parse_nchar (&blockette_ptr, 1);

	/* recover start-of-data time */
	type70->start_of_data = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover end-of-data time */
	type70->end_of_data = parse_varlstr (&blockette_ptr, STRINGTERM);

/*                 +=======================================+                 */
/*=================|   link the type70 into the type70 table   |=================*/
/*                 +=======================================+                 */

	append_linklist_element (type70, type70_head, type70_tail);

}
