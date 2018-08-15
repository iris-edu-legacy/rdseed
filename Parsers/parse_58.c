/*===========================================================================*/
/* SEED reader     |               parse_type58               |  station header */
/*===========================================================================*/
/*
	Name:		parse_type58
	Purpose:	parse a channel sensitivity (type 58) blockette
				into a globally-available table
	Usage:		void parse_type58 ();
				char *blockette;
				parse_type58 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type58_head - 1st entry in table; defined in structures.h,
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
				05/11/94  Chris Laughbon  took out the malloc if 0 bytes
*/

#include "rdseed.h"

void parse_type58 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type58 *type58; 							/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|  allocate space and initialize a type58  |=================*/
/*                 +=======================================+                 */

	type58 = (struct type58 *) alloc_linklist_element (sizeof (struct type58),
		"parse_type58");	

/*                 +=======================================+                 */
/*=================|    recover information from the type58   |=================*/
/*                 +=======================================+                 */

	/* recover cascade sequence number */
	type58->stage = parse_int (&blockette_ptr, 2);

	/* recover sensitivity in counts/unit (units defined in type 52 (chl id) */
	type58->sensitivity = parse_double (&blockette_ptr, 12);

	/* recover frequency of sensitivity factor in Hz */
	type58->frequency = parse_double (&blockette_ptr, 12);

	/* recover number of standardized calibrations */
	type58->number_calibrations = parse_int (&blockette_ptr, 2);

	/* allocate space for calibrations */
	if (type58->number_calibrations != 0)
		type58->calibration = (struct type58sub *) alloc_linklist_element
			(sizeof (struct type58sub) * type58->number_calibrations,
				"parse_type58->calibration");

	/* recover calibrations */
	for (i = 0; i < type58->number_calibrations; i++)
	{
		type58->calibration[i].sensitivity = parse_double (&blockette_ptr, 12);
		type58->calibration[i].frequency = parse_double (&blockette_ptr, 12);
		type58->calibration[i].time = parse_varlstr (&blockette_ptr, STRINGTERM);
	}

/*                 +=======================================+                 */
/*=================|    link the type58 into the type31 table   |=================*/
/*                 +=======================================+                 */

	/* initialize the pointer to the next record as end-of-list marker */
	type58->next = NULL;

	response = get_response ('S');
	response->ptr.type58 = type58;
}
