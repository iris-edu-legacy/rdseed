/*===========================================================================*/
/* SEED reader     |             parse_type11               |   volume header */
/*===========================================================================*/
/*
	Name:		parse_type11
	Purpose:	parse a volume station header index (type 11) blockette into a 
				globally-available structure
	Usage:		void parse_type11 ();
				char *blockette;
				parse_type11 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char - allocated in globals.h
				type11 - defined in structures.h, allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_volh
	Calls to:	parse_int - get an integer from a blockette 
				parse_int - get an integer from a blockette 
				parse_nchar - get a fixed-length string from a blockette 
	Algorithm:	skip blockette type and length; read number of stations; for
				each station, read station ident and sequence number
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				08/19/89  Dennis O'Neill  streamlined linked list construction
				05/11/94  CL			  took out the malloc if 0 bytes
*/

#include "rdseed.h"

/* this needs to be allocated here so it keeps its value between calls */
struct type11 *type11_tail = NULL;                      /* end of linked list */ 


void parse_type11 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	int i;										/* counter */
	
	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

	struct type11 *type11;

	type11 = (struct type11 *) alloc_linklist_element(sizeof(struct type11), "parse_type11");

	/* read the number of stations in the index table */
	type11->number_stations = parse_int(&blockette_ptr, 3);

	/* allocate space for the station ids and sequence numbers */
	if (type11->number_stations != 0)
		type11->station = (struct type11sub *) alloc_linklist_element
							(sizeof (struct type11sub) * 
								type11->number_stations, 
							"parse_type11");

	/* read the stations and sequence numbers */
	for (i = 0; i < type11->number_stations; i++)
	{
		type11->station[i].station_id = parse_nchar (&blockette_ptr, 5);

		type11->station[i].sequence_number = parse_long(&blockette_ptr, 6);

	}

	append_linklist_element (type11, type11_head, type11_tail);
}
