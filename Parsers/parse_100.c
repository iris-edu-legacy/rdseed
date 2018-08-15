/*===========================================================================*/
/* SEED reader     |            parse_type100              |     data block  */
/*===========================================================================*/
/*
	Name:		parse_type100
	Purpose:	parse a data record header for blockette 100s.
	Usage:		void parse_type100 ();
				char *input_data_ptr;
				double *sample_rate;
				parse_type100 (input_data_ptr, &sample_rate);
	Input:		pointer to beginning of data record header
	Output:		none
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	process_data
	Calls to:	none
	Algorithm:	search through data record blockettes for blockette 100 sample rate
	Notes:		none
	Problems:	none known
	References:	none
	Language:	C, hopefully ANSI standard
	Author:		Allen Nance
	Revisions:	09/18/92  Allen Nance  Initial version
*/

#include "rdseed.h"

// already been swapped in process_data

void parse_type100alt(char *, double *);


extern int byteswap;

void parse_type100 (input_data_ptr, sample_rate)
char *input_data_ptr;	/* ptr to start of data record */
double *sample_rate;	/* ptr to returned sample rate - don't touch if 
			 * block 100 not found 
			 */


{
	struct input_data_hdr *input_data_hdr;	/* fixed data header */
	struct data_blk_100 *blk_100;		/* blockette 100 pointer */

	/* point to beginning data header structure */
	input_data_hdr = (struct input_data_hdr *) input_data_ptr;

	if (input_data_hdr->bofb == 0)
		return;

	parse_type100alt(input_data_ptr + (input_data_hdr->bofb - 8), sample_rate);

}

void parse_type100alt(char *blks, double *sample_rate)

{

	struct data_blk_100 *blk_100;		/* blockette 100 pointer */

	blk_100 = (struct data_blk_100 *)blks;

	do 
	{

		if (blk_100->hdr.type == 100)
		{
			*sample_rate = blk_100->sample_rate;

			return;
		}

		if (blk_100->hdr.next_blk_byte == 0)
			return;

		if ((blk_100->hdr.next_blk_byte%4)!=0)
		{
			fprintf (stderr, "WARNING (parse_type100):  ");
			fprintf (stderr, "invalid byte pointer = %d\n", 
					blk_100->hdr.next_blk_byte);
			fprintf (stderr, "\tData blockette parsing aborted.\n");
			break;
		}

		/* move to next blk */

		blk_100 = (struct data_blk_100 *)(blks + (blk_100->hdr.next_blk_byte-48));

	} while (1);

	return;

}
