/*===========================================================================*/
/* SEED reader     |             parse_type73              |     time header */
/*===========================================================================*/
/*
	Name:		parse_type73
	Purpose:	parse a time span data start index (type 73) blockette into a
				globally-available table
	Usage:		void parse_type73 ();
				char *blockette;
				parse_type73 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type73_head - 1st entry in table; defined in structures.h,
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_stationh
	Calls to:	parse_int - get an integer from a blockette 
				parse_double - get a double from a blockette 
				parse_varlstr - get a variable-length string from a blockette
	Algorithm:	parse the blockette into a globally-available table
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
struct type73 *type73_tail = NULL;				/* end of linked list */

void parse_type73 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type73 *type73; 						/* genl member of table */
	int i;										/*counter */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================| allocate space and initialize a type73|=================*/
/*                 +=======================================+                 */

	type73 = (struct type73 *) alloc_linklist_element (sizeof (struct type73),
		"parse_type73");	

/*                 +=======================================+                 */
/*=================|  recover information from the type73  |=================*/
/*                 +=======================================+                 */

	/* recover number of data pieces */
	type73->number_datapieces = parse_int(&blockette_ptr, 4);

	/* allocate space for data pieces */
	type73->datapiece = (struct type73sub *) alloc_linklist_element
		(sizeof (struct type73sub) * type73->number_datapieces,
		"parse_type73->datapiece");

	/* recover data pieces */
	for (i = 0; i < type73->number_datapieces; i++)
	{
		type73->datapiece[i].station = parse_nchar (&blockette_ptr, 5);
		type73->datapiece[i].location = parse_nchar (&blockette_ptr, 2);
		type73->datapiece[i].channel = parse_nchar (&blockette_ptr, 3);
		if (*blockette_ptr != '~')
			type73->datapiece[i].time = parse_varlstr (&blockette_ptr, STRINGTERM);
		else {
			type73->datapiece[i].time = NULL;
			blockette_ptr += 1;
		}
		type73->datapiece[i].sequence_number = parse_long(&blockette_ptr, 6);
		type73->datapiece[i].subsequence = parse_int (&blockette_ptr, 2);
/*printf("%d - %s.%s %d\n", input.recordnumber, type73->datapiece[i].station,type73->datapiece[i].channel,type73->datapiece[i].sequence_number);*/

	} /*printf("End 73\n");*/

/*                 +=======================================+                 */
/*=================| link the type73 into the type73 table |=================*/
/*                 +=======================================+                 */

	append_linklist_element (type73, type73_head, type73_tail);

}


flush_type73()
{
	while (type73_head != NULL)
	{
		free (type73_head);
		type73_head = type73_head->next;
	}
	type73_tail = type73_head;
}
