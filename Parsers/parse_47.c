/*===========================================================================*/
/* SEED reader     |              parse_type47              | station header */
/*===========================================================================*/
/*
	Name:		parse_type47
	Purpose:	parse a decimation (47) dictionary  blockette
				into a globally-available table
	Usage:		void parse_type47 ();
				char *blockette;
				parse_type47 (blockette);
	Input:		blockette = pointer to beginning of blockette
	Output:		none
	Externals:	temp_char
				type47_head - 1st entry in table; defined in structures.h,
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
	Revisions:	05/08/90  Sue Schoch      Initial release 2.0
*/

#include "rdseed.h"

void parse_type47 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	struct type47 *type47; 							/* genl member of table */
	int i;										/* counter */
	struct response *response;					/* channel response ptr */
    static struct type47 *type47_tail = NULL;   /* end of linked list */

	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;

/*                 +========================================+                 */
/*=================| allocate space and initialize a type47 |=================*/
/*                 +========================================+                 */

	type47 = (struct type47 *) alloc_linklist_element (sizeof (struct type47),
		"parse_type47");	

/*                 +========================================+                 */
/*=================|   recover information from the type47  |=================*/
/*                 +========================================+                 */

	/* recover decimation lookup code */
	type47->response_code = parse_int (&blockette_ptr, 4);

	/* recover decimation name */
	type47->name = parse_varlstr (&blockette_ptr, STRINGTERM);

	/* recover incoming sample rate in samples per second */
	type47->input_sample_rate = parse_double (&blockette_ptr, 10);

	/* recover decimation factor (also is number of samples to read in) */ 
	/* 	input_sample_rate * decimation_factor = final sample rate       */
	type47->decimation_factor = parse_int(&blockette_ptr, 5);

	/* recover offset to sample to use (sample number - 1) */
	type47->decimation_offset = parse_int(&blockette_ptr, 5);

	/* recover estimated pure delay for the stage */
	type47->delay = parse_double (&blockette_ptr, 11);

	/* recover time shift applied to time tag due to digital delay */
	type47->correction = parse_double (&blockette_ptr, 11);

/*                 +========================================+                 */
/*=================|  link the type47 into the type47 table |=================*/
/*                 +========================================+                 */

	append_linklist_element (type47, type47_head, type47_tail);

}
