/*===========================================================================*/
/* SEED reader     |            parse_type59               |  station header */
/*===========================================================================*/
/*
	Name:		parse_type59
	Purpose:	parse a channel comment (type 59) blockette
	Usage:		void parse_type59 ();
				char *blockette;
				parse_type59 (blockette);
	Input:		blockette = pointer to a station comment blockette
	Output:		none
	Externals:	temp_char - allocated in globals.h
				type59_head - 1st entry in table; defined in structures.h, 
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

/* this needs to be allocated here so it keeps its value between calls */
struct type59 *type59_tail = NULL;					/* end of linked list */

void parse_type59 (blockette)
char *blockette;								/* ptr to start of blockette */
{
FILE *fptr;
int size;
char wrkstr[200];

	char *blockette_ptr;		/* ptr to inside blockette */
	struct type59 *type59; 		/* genl member of table */

        struct time chn_comm_start_struct;
        struct time chn_comm_end_struct;

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================| allocate space and initialize a type59|=================*/
/*                 +=======================================+                 */

	type59 = (struct type59 *) alloc_linklist_element (sizeof (struct type59),
		"parse_type59");	

	memset((char *)type59, 0, sizeof(struct type59));


/*                 +=======================================+                 */
/*=================|  recover information from the type59  |=================*/
/*                 +=======================================+                 */

	/* recover beginning effective time */
	type59->start = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover ending effective time */
	if (*blockette_ptr != '~')
		type59->end = parse_varlstr (&blockette_ptr, STRINGTERM);
	else {
		type59->end = NULL;
		blockette_ptr += 1;
	}

	/* check start/end effective if entered by user */
        timecvt(&chn_comm_start_struct, type59->start);

	if (type59->end == NULL)
	{
		chn_comm_end_struct.year = 9999;
	}
	else
        	timecvt(&chn_comm_end_struct, type59->end);

        if (!chk_time(chn_comm_start_struct, chn_comm_end_struct))
	{
		free(type59);
                return;
	}

	/* recover index code for comment in comment dictionary */
	type59->comment_code = parse_int (&blockette_ptr, 4);

	/* recover index code for level in comment dictionary */
	type59->level_code = parse_long(&blockette_ptr, 6);

/*                 +=======================================+                 */
/*=================| link the type59 into the type59 table |=================*/
/*                 +=======================================+                 */

	append_linklist_element (type59, current_channel->type59_head,
		current_channel->type59_tail);

}
