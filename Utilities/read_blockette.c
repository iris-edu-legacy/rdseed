/*===========================================================================*/
/* SEED reader     |           read_blockette              |         utility */
/*===========================================================================*/
/*
	Name:		read_blockette
	Purpose:	return a complete blockette to caller, even if spanning occured
	Usage:		void read_blockette ();
				char *input_data_ptr;
				char *blockette;
				int blockette_type;
				int blockette_length;
				read_blockette (input_data_ptr, blockette, &blockette_type, 
					&blockette_length);
	Input:		input_data_ptr = the record read from the input file
	Output:		blockette = an array containing the blockette data
				blockette_type = a number indicating the blockette type
				blockette_length = number of bytes in the blockette
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	anything
	Calls to:	memncpy
	Algorithm:	retrieve the blockette type; retrieve the length in bytes of the
				blockette; attempt to recover the appropriate number of bytes.
				Check the actual number of bytes copied; if not enough bytes
				were copied, read more logical input records, knocking off the
				introductory bytes, until sufficient bytes to complete the 
				blockette are obtained..
	Notes:		This routine will read blockettes which span logical records.
				It returns the proper offset into the current logical record
				at which the next blockette will be found, even if a spanning
				has occurred (the	value returned in "blockette_length").
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"
#define BLOCKETTE_INCREMENT 1024

void read_blockette (input_data_ptr, blockette, blockette_type, blockette_length)
char **input_data_ptr;
char *blockette;
int *blockette_type;
int *blockette_length;
{
	extern FILE *inputfile;					/* ptr to an open input file */
	int bytes_copied;					/* number of bytes copied */
	int original_blockette_length;				/* # bytes in blockette */
	int total_bytes_copied;					/* counter */

	int eor = 0;
	struct input_data_hdr *hdr;

	/* recover the blockette type */
	memncpy (blockette, *input_data_ptr, 3);

	*blockette_type = atoi (blockette);

	/* recover the length of the blockette */
	memncpy (blockette, (*input_data_ptr)+3, 4);
	*blockette_length = atoi (blockette);

	original_blockette_length = *blockette_length;

	/* if necessary, allocate more space for the blockette */

	/* don't read too much  REPLACE WITH SECTION ABOVE */
	if (*blockette_length > MAX_BLKT_LENGTH)
	{
		fprintf (stderr, "ERROR (read_blockette):  ");
		fprintf (stderr, "attempted to read a blockette of length %d;\n",
			*blockette_length);
		fprintf (stderr, "\tmaximum allowed length is %d.\n", 
			MAX_BLKT_LENGTH);
		fprintf (stderr, "\tTry changing MAX_BLKT_LENGTH in rdseed.h");
		fprintf (stderr, " and recompiling after a make clean.\n");
		fprintf (stderr, "\tExecution terminating.\n");
		exit (-1);
	}

	/* recover the blockette */
	bytes_copied = memncpy (blockette, *input_data_ptr, *blockette_length);


	/* if not enough bytes were read, read more logical records */
	total_bytes_copied = bytes_copied;

	if (total_bytes_copied < original_blockette_length)
	{

		while (total_bytes_copied < original_blockette_length)
		{
			
			if (offset + LRECL >= num_bytes_read) 
				/* read a physical record */
			{
				memset(precord, ' ', PRECL);

				/*num_bytes_read = read (fileno (inputfile), precord, PRECL);*/

				num_bytes_read = fread(precord, 1, PRECL, inputfile);	

				if (num_bytes_read == 0)
				{
					fprintf (stderr, "ERROR (read_blockette):  ");
					fprintf (stderr, "blockette was incomplete, but EOF found");
					fprintf (stderr, " in input file.\n");
					fprintf (stderr, "\tExecution terminating.\n");
					exit (-1);
				}


				precord_ptr = (char *)precord;
				offset = 0;
				lrecord_ptr = precord_ptr;

			}
			else
			{
				/* point to beginning of next logical record */
				/* lrecord_ptr = precord_ptr + offset + LRECL; */

				lrecord_ptr += LRECL;

				/* set up offset for next time around */
				offset += LRECL;
		
				if (offset >= num_bytes_read) 
					offset = 0;
			}
 
			/* extract a logical record from the physical record */
			read_logical_record (lrecord_ptr);

			*input_data_ptr = input.data;
			// new block len read in.

		 	*blockette_length = *blockette_length - bytes_copied;

			bytes_copied = memncpy (temp_char, input.data, *blockette_length);

			total_bytes_copied += bytes_copied;
			strcat (blockette, temp_char);

		}

		/* trap error if complete blockette not read */
		if (strlen (blockette) < original_blockette_length)
		{
			fprintf (stderr, "ERROR (read_blockette):  ");
			fprintf (stderr, "unable to read a complete blockette.\n");
			fprintf (stderr, "\tLength wanted, %d; length acquired, %d\n",
						original_blockette_length, strlen (blockette));
			fprintf (stderr, "\tExecution terminating.\n");
			exit (-1);
		}
	}
}
