/*===========================================================================*/
/* SEED reader     |               parse_type50              |  station header */
/*===========================================================================*/
/*
	Name:		parse_type50
	Purpose:	parse a station id (type 50) blockette into a 
				globally-available table
	Usage:		void parse_type50 ();
				char *blockette;
				parse_type50 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type50_head - 1st entry in table; defined in structures.h,
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_abbrevdic
	Calls to:	parse_int - get an integer from a blockette 
			parse_double - get a double from a blockette 
			parse_nchar - get a fixed-length string from a blockette 
			parse_varlstr - get a variable-length string from a blockette
	Algorithm:	parse the blockette
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
			11/04/88  Dennis O'Neill  replaced "~" by STRINGTERM
					parse_varlstr calls

			11/21/88  Dennis O'Neill  Production release 1.0
			08/19/89  Dennis O'Neill  streamlined linked list construction
			09/17/89  Dennis O'Neill  removed number_channels and 
		                          number_comments fields
			04/26/93  Allen Nance     Added network code for v2.3
*/

#include "rdseed.h"

/* this needs to be allocated here so it keeps its value between calls */
struct type50 *type50_tail = NULL;	/* end of linked list */

static struct type50 type50; 		/* genl member of table */

int parse_type50 (blockette)
char *blockette;			/* ptr to start of blockette */
{
	char *blockette_ptr;		/* ptr to inside blockette */
	struct type50 *type50_ptr;

	struct time stn_start_struct;
	struct time stn_end_struct;

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================| allocate space and initialize a type50  |=================*/
/*                 +=======================================+                 */

#if 0
	/* allocate space for a type50 record */
	type50 = (struct type50 *) alloc_linklist_element (sizeof (struct type50),
		"parse_type50");	

	/* set the global variable current_station to this record */
	current_station = type50;
#endif

/*                 +=======================================+                 */
/*=================|   recover information from the type50   |=================*/
/*                 +=======================================+                 */

	type50.next = NULL;

	/* recover station symbol */
	type50.station = parse_nchar (&blockette_ptr, 5);

	/* recover latitude */
	type50.latitude = parse_double (&blockette_ptr, 10);

	/* recover longitude */
	type50.longitude = parse_double (&blockette_ptr, 11);

	/* recover elevation */
	type50.elevation = parse_double (&blockette_ptr, 7);

	/* recover number of channels - old spec, removed */
	/* type50.number_channels = parse_int (&blockette_ptr, 4); */
	type50.reserved1 = parse_int (&blockette_ptr, 4);

	/* recover number of station comment blockettes - old spec, removed */
	/* type50->number_comments = parse_int (&blockette_ptr, 3); */
	type50.reserved2 = parse_int (&blockette_ptr, 3);

	/* recover site name */
	type50.name = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover network/station owner name code (see generic abbrev dict) */
	type50.owner_code = parse_int (&blockette_ptr, 3);

	/* recover 32-bit byte significance order */
	type50.longword_order = parse_int (&blockette_ptr, 4);

	/* recover 16-bit byte significance order */
	type50.word_order = parse_int (&blockette_ptr, 2);

	/* recover starting effective date */
	type50.start = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover ending effective date, allowing for zero length */
	if (*blockette_ptr != '~')
		type50.end = parse_varlstr (&blockette_ptr, STRINGTERM);
	else {
		type50.end = NULL;
		blockette_ptr += 1;
	}

	/* check start/end effective if entered by user */
	timecvt(&stn_start_struct, type50.start);

	if (type50.end == NULL)
	{
		/* make essentially infinite endpoint */
		stn_end_struct.year = 99999;
	}
	else
		timecvt(&stn_end_struct, type50.end);
	
	if (!chk_time(stn_start_struct, stn_end_struct))
		return 0;

	/* recover update flag */
	type50.update = parse_nchar (&blockette_ptr, 1);

	/* recover update flag */
	if (type10.version >= 2.3)
	{
		type50.network_code = parse_nchar (&blockette_ptr, 2);
	}
	else
	{
		type50.network_code = NULL;
	}

	/* initialize the pointer for the first of each of several elements */
	type50.type51_head = NULL;
	type50.type51_tail = NULL;
	type50.type52_head = NULL;
	type50.type52_tail = NULL;
	type50.station_update = NULL;

/*                 +=======================================+                 */
/*=================|   link the type50 into the type50 table   |=================*/
/*                 +=======================================+                 */

	/* allocate space for a type50 record */
    	type50_ptr = (struct type50 *)
		alloc_linklist_element(sizeof(struct type50), 
					"parse_type50");

	memcpy((char *)type50_ptr, (char *)&type50, sizeof(struct type50));
			
	append_linklist_element (type50_ptr, type50_head, type50_tail);

	current_station = type50_ptr;

	return 1;
}
