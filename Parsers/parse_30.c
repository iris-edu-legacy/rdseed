/*===========================================================================*/
/* SEED reader     |              parse_type30               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		parse_type30
	Purpose:	parse a data format dictionary (type 30) blockette into a 
				globally-available structure
	Usage:		void parse_type30 ();
				char *blockette;
				parse_type30 (blockette);
	Input:		blockette = pointer to beginning of blockette string
	Output:		none
	Externals:	temp_char - allocated in globals.h
				type30_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		terminate if unable to allocate sufficient memory
	Called by:	process_abbrevdic
	Calls to:	parse_int - get an integer from a blockette 
				parse_varlstr - get a variable-length string from a blockette
	Algorithm:	parse the blockette into its parts; report result
	Notes:		This process creates a linked list containing the entire data
					format dictionary.  For a usage example, see print_type30.
				The first element of the linked list is type30_head.
				type30_head and type30_tail should be initialized to NULL when 
					allocated.
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/04/88  Dennis O'Neill  replaced "~" by STRINGTERM in
				                          parse_varlstr calls
				11/21/88  Dennis O'Neill  Production release 1.0
				08/19/89  Dennis O'Neill  streamlined linked list construction
				05/11/94  CL			  took out the malloc if 0 bytes
*/

#include "rdseed.h"

struct prim_struct *parse_ddl_key();

/* this needs to be allocated here so it keeps its value between calls */
struct type30 *type30_tail = NULL;			/* end of linked list */

void parse_type30 (blockette)
char *blockette;					/* ptr to start of blockette */
{
	char *blockette_ptr;				/* ptr to inside blockette */
	int i;						/* counter */
	struct type30 *type30;				/* fmt dctnry blockette */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================| allocate space and initialize a type30  |=================*/
/*                 +=======================================+                 */

	type30 = (struct type30 *) alloc_linklist_element (sizeof (struct type30),
		"parse_type30");	

/*                 +=======================================+                 */
/*=================|   recover information from the type30   |=================*/
/*                 +=======================================+                 */

	/* recover format name */
	type30->name = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover format identification code */
	type30->code = parse_int (&blockette_ptr, 4);

	/* recover data family code */
	type30->family = parse_int (&blockette_ptr, 3);

	/* recover number of data decoder keys */
	type30->number_keys = parse_int (&blockette_ptr, 2);

	/* allocate space for decoder keys */
	/* this is space for an array of pointers to pointers to chars */
	/* this prepares for recovering the decoder keys of unknown size */
	if (type30->number_keys != 0)
		type30->decoder_key = (char **)alloc_linklist_element(sizeof(char *) *
								  type30->number_keys, 
								"parse_type30->decoder_key");

	if (type30->number_keys != 0)
		type30->decoder_key_prim = 
			(char **)alloc_linklist_element(sizeof (char *) * 
												type30->number_keys,
											"parse_type30->decoder_key_prim");

	/* recover data decoder keys */
	for (i = 0; i < type30->number_keys; i++)
	{
		type30->decoder_key[i] = parse_varlstr (&blockette_ptr, STRINGTERM);
		type30->decoder_key_prim[i] = (char *) parse_ddl_key(type30->decoder_key[i], type30->family);
	}

/*                 +=======================================+                 */
/*=================|   link the type30 into the type30 table   |=================*/
/*                 +=======================================+                 */

	append_linklist_element (type30, type30_head, type30_tail);

}
