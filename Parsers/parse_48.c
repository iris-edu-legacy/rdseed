/*===========================================================================*/
/* SEED reader     |             parse_type48              |  station header */
/*===========================================================================*/
/*
	Name:		parse_type48
	Purpose:	parse a channel sensitivity (type 58) blockette
				into a globally-available table
	Usage:		void parse_type48 ();
				char *blockette;
				parse_type48 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type48_head - 1st entry in table; defined in structures.h,
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
	Author:		Sue Schoch
	Revisions:	06/05/90  Sue Schoch      Initial preliminary release in 2.0
				05/11/94  CL			  took out malloc if 0 bytes
*/

#include "rdseed.h"

void parse_type48 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type48 *type48; 							/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */
	static struct type48 *type48_tail = NULL;   /* end of linked list */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +=======================================+                 */
/*=================|allocate space and initialize a type48 |=================*/
/*                 +=======================================+                 */

	type48 = (struct type48 *) alloc_linklist_element (sizeof (struct type48),
		"parse_type48");	

/*                 +=======================================+                 */
/*=================|  recover information from the type48  |=================*/
/*                 +=======================================+                 */

	/* recover response lookup code */
	type48->response_code = parse_int (&blockette_ptr, 4);

	/* recover response name */
	type48->name = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover sensitivity in counts/unit (units defined in type 52 (chl id) */
	type48->sensitivity = parse_double (&blockette_ptr, 12);

	/* recover frequency of sensitivity factor in Hz */
	type48->frequency = parse_double (&blockette_ptr, 12);

	/* recover number of standardized calibrations */
	type48->number_calibrations = parse_int (&blockette_ptr, 2);

	/* allocate space for calibrations */
	if (type48->number_calibrations != 0)		/* added 05/11/94- CL */
		type48->calibration = (struct type48sub *) 
					alloc_linklist_element(sizeof (struct type48sub) *
											type48->number_calibrations,
										   "parse_type48->calibration");

	/* recover calibrations */
	for (i = 0; i < type48->number_calibrations; i++)
	{
		type48->calibration[i].sensitivity = parse_double (&blockette_ptr, 12);
		type48->calibration[i].frequency = parse_double (&blockette_ptr, 12);
		type48->calibration[i].time = parse_varlstr (&blockette_ptr, STRINGTERM);
	}

/*                 +=======================================+                 */
/*=================| link the type48 into the type31 table |=================*/
/*                 +=======================================+                 */

    append_linklist_element (type48, type48_head, type48_tail);

}
