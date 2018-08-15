/* UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED *** UNTESTED */
/*      Data of this type were unavailable when this process was written     */

/*===========================================================================*/
/* SEED reader     |            parse_type35               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		parse_type35
	Purpose:	parse a beam configuration (type 35) blockette into a 
				globally-available structure
	Usage:		void parse_type35 ();
				char *blockette;
				parse_type35 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char - allocated in globals.h
				type35_head - 1st entry in table; defined in structures.h,
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_abbrevdic
	Calls to:	parse_int - get an integer from a blockette 
				parse_double - get a double from a blockette 
				parse_nchar - get a fixed-length string from a blockette 
	Algorithm:	parse the blockette; report the results
	Notes:		number of type35->beam[i].channels is set by MAX_CPTS, 
				set at 1000 on 22 March 1988.
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				08/19/89  Dennis O'Neill  streamlined linked list construction
				09/17/89  Dennis O'Neill  increase #chars in type35->number_beams
				                          to 4; added subchannel_id field
				05/11/94  CL			  took out malloc call if 0 bytes
*/

#include "rdseed.h"

/* this needs to be allocated here so it keeps its value between calls */
struct type35 *type35_tail = NULL;					/* end of linked list */

void parse_type35 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type35 *type35;							/* genl member of table */
	int i;										/* counter */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|allocate space and initialize a type35 |=================*/
/*                 +=======================================+                 */

	/* allocate space for a type35 record */
	type35 = (struct type35 *) alloc_linklist_element (sizeof (struct type35),
		"parse_type35");	

/*                 +=======================================+                 */
/*=================|  recover information from the type35  |=================*/
/*                 +=======================================+                 */

	/* recover beam code */
	type35->code = parse_int (&blockette_ptr, 3);

	/* recover number of beam components */
	type35->number_beams = parse_int (&blockette_ptr, 4);

	/* allocate space for the beam entries */
	if (type35->number_beams != 0)
		type35->beam = (struct type35sub *)alloc_linklist_element(
							sizeof (struct type35sub)
								* type35->number_beams, 
							"parse_type35->beam");

	/* recover the beam entries */
	for (i = 0; i < type35->number_beams; i++)
	{
		/* recover station id */
		type35->beam[i].station = parse_nchar (&blockette_ptr, 5);

		/* recover location id */
		type35->beam[i].location = parse_nchar (&blockette_ptr, 2);

		/* recover channel id */
		type35->beam[i].channel = parse_nchar (&blockette_ptr, 3);

		/* recover subchannel identifier (for multiplexing) */
		type35->beam[i].subchannel = parse_int (&blockette_ptr, 4);

		/* recover channel weight */
		type35->beam[i].weight = parse_double (&blockette_ptr, 5);
	}

/*                 +=======================================+                 */
/*=================| link the type35 into the type35 table |=================*/
/*                 +=======================================+                 */

	append_linklist_element (type35, type35_head, type35_tail);

}
