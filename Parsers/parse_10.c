/*===========================================================================*/
/* SEED reader     |              parse_type10               |   volume header */
/*===========================================================================*/
/*
	Name:		parse_type10
	Purpose:	parse the volume identification (type 10) blockette into a 
				globally-available structure
	Usage:		void parse_type10 ();
				char *blockette;
				parse_type10 (blockette);
	Input:		blockette = a pointer to the start of the volume id blockette
	Output:		none
	Externals:	temp_char - allocated in globals.h
				type10 - defined in structures.h, allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	process_volh
	Calls to:	parse_int - get an integer from a blockette 
				parse_double - get a double from a blockette 
				parse_varlstr - get a variable-length string from a blockette
	Algorithm:	parse the blockette; put result into a structure
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/04/88  Dennis O'Neill  replaced "~" by STRINGTERM in
				                          parse_varlstr calls
				11/21/88  Dennis O'Neill  Production release 1.0
				07/10/92  Allen Nance     Added fields 7 and 8 for Seed ver 2.2
				04/26/93  Allen Nance     Seed ver 2.3 compatibility fields 7,8
						                  are really for v2.3 not v2.2
				09/01/93  CL			  added parsing of volume label 

*/

#include "rdseed.h"

void parse_type10 (blockette)
char *blockette;								/* ptr to start of blockette */
{
	char *blockette_ptr;						/* ptr to inside blockette */
	
	/* point to beginning of information, past type and length */
	blockette_ptr = blockette;
	blockette_ptr += 7;
	
	/* read the version number */
	type10.version = parse_double (&blockette_ptr, 4);

	/* read log(2) of logical record length */
	type10.log2lrecl = parse_int (&blockette_ptr, 2);

	/* read beginning- and end-of-volume dates */
	type10.bov = parse_varlstr (&blockette_ptr, STRINGTERM);
	type10.eov = parse_varlstr (&blockette_ptr, STRINGTERM);

	if (type10.version >= 2.3)
	{
		type10.volume_time     = parse_varlstr(&blockette_ptr, STRINGTERM);
		type10.organization = parse_varlstr(&blockette_ptr, STRINGTERM);
		type10.volume_label = parse_varlstr(&blockette_ptr, STRINGTERM);

	}
	else
	{
		type10.volume_time     = NULL;
		type10.organization = NULL;
	}		
}
