/*===========================================================================*/
/* SEED reader     |             process_volh              |   volume header */
/*===========================================================================*/
/*
	Name:		process_volh
	Purpose:	process a volume header from a SEED format file
	Usage:		void process_volh ();
				if (record type is volume header)
					process_volh ();
	Input:		none
	Output:		none
	Externals:	temp_char - allocated in globals.h
				input - defined in structures.h, allocated in globals.h
	Warnings:	if unexpected blockette type is encountered, print message
	Errors:		none
	Called by:	main
	Calls to:	read_blockette - local fcn; read a blockette
				parse_type10 - local fcn; parse the volume id blockette
				parse_type11 - local fcn; parse the volume station header index
				parse_type12 - local fcn; parse the volume time span index
	Algorithm:	receive a logical record of type V from the main program.  Parse
				each blockette (volume id, volumne station header index, and
				volume time span index) in the appropriate subprocedure.  If a 
				blockette of unknown type is encountered, print a message and 
				continue.  Stop when a blockette of type 0 is encountered.
	Notes:		Structure of the volume header list:
				volume id
				volume station header index
				volume time span index
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

void process_volh ()
{
	char *input_data_ptr, *blockette_ptr;						/* ptr into input.data */
	int i, l;

	input_data_ptr = input.data;

/*                 +=======================================+                 */
/*=================|    Read and process volume header     |=================*/
/*                 +=======================================+                 */

	blockette_length = 0;
	blockette_type = -1;

	while ((blockette_type != 0))
	{
		input_data_ptr += blockette_length;

		read_blockette (&input_data_ptr, blockette, &blockette_type, 
			&blockette_length);

		if (blockette_type == 10)
		{
			/* offset gets incr at end of "main" loop */
			/* offset -= LRECL; */

			blockette_ptr = &blockette[11];
			l = parse_int (&blockette_ptr, 2);			/* read logical record length */
			LRECL = 1<<l;

			if (LRECL < 256 || LRECL > 32768)
				{
				fprintf(stderr, "ERROR (process_volh):  ");
				fprintf(stderr, "Invalid Logical Record Length - Fatal ERROR\n");
				exit(-1);
				}

			read_logical_record (lrecord_ptr);

			found_lrecl_flag = TRUE;

			parse_type10 (blockette);

		}
		else if (at_volume && blockette_type == 11) parse_type11 (blockette);
		else if (at_volume && blockette_type == 12) parse_type12 (blockette);
		else if (at_volume && blockette_type != 0)
		{
			fprintf (stderr, "WARNING (process_volh):  ");
			fprintf (stderr, "unknown blockette type %d found in record %ld.\n",
				blockette_type, input.recordnumber);
			fprintf (stderr, "\tExecution continuing.\n");
		}
	}

/*                 +=======================================+                 */
/*=================|                cleanup                |=================*/
/*                 +=======================================+                 */

}
