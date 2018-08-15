/*===========================================================================*/
/* SEED reader     |             parse_type51                 |  station header */
/*===========================================================================*/
/*
	Name:		parse_type51
	Purpose:	parse a station comment (type 51) blockette into a
				globally-available table
	Usage:		void parse_type51 ();
				char *blockette;
				parse_type51 (blockette);
	Input:		blockette = pointer to a station comment blockette
	Output:		none
	Externals:	temp_char - allocated in globals.h
				type51_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_stationh
	Calls to:	parse_int - get an integer from a blockette 
				parse_long - get a long integer from a blockette 
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

void parse_type51 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;		/* ptr to inside blockette */
	struct type51 *type51; 		/* genl member of table */

	struct time stn_comm_start_struct;
        struct time stn_comm_end_struct;

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|  allocate space and initialize a type51  |=================*/
/*                 +=======================================+                 */

	type51 = (struct type51 *) alloc_linklist_element (sizeof (struct type51),
		"parse_type51");	

/*                 +=======================================+                 */
/*=================|   recover information from the type51    |=================*/
/*                 +=======================================+                 */

	memset((char *)type51, 0, sizeof(struct type51));

	/* recover beginning effective time */
	type51->start = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover ending effective time */
	if (*blockette_ptr != '~')
		type51->end = parse_varlstr (&blockette_ptr, STRINGTERM);
	else {
		type51->end = NULL;
		blockette_ptr += 1;
	}

        /* check start/end effective if entered by user */
        timecvt(&stn_comm_start_struct, type51->start);

	if (type51->end == NULL)
	{
		/* make essentially infinate endpoint */
		stn_comm_end_struct.year = 99999;
	}
	else
        	timecvt(&stn_comm_end_struct, type51->end);

        if (!chk_time(stn_comm_start_struct, stn_comm_end_struct))
	{
		free(type51);

                return;
	}

	/* recover index code for comment in comment dictionary */
	type51->comment_code = parse_int (&blockette_ptr, 4);

	/* recover index code for level in comment dictionary */
	type51->level_code = parse_long(&blockette_ptr, 6);

/*                 +=======================================+                 */
/*=================|    link the type51 into the type51 table    |=================*/
/*                 +=======================================+                 */


	append_linklist_element (type51, current_station->type51_head,
		current_station->type51_tail);

}
