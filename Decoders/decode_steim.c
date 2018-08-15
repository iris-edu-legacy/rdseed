/*===========================================================================*/
/* SEED reader     |             decode_steim              |     decode data */
/*===========================================================================*/
/*
	Name:		decode_steim
	Purpose:	translate SEED tape data for IRIS stations (Steim format) to
				numbers
	Usage:		void decode_steim ();
				char *data_ptr;
				int nsamples;
				decode_steim (data_ptr, nsamples, index);
	Input:		data_ptr - pointer to beginning of seismic data in a SEED file
				nsamples - number of samples
				index - index of next entry into output array
	Output:		an array of numbers (seismic data)
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	process_data
	Calls to:	none
	Algorithm:	See Steim 1986, Halbert 1988.
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
				Steim, J. M. (1986).  The Very-Broad-Band Seismograph.  Doctoral
					thesis, Department of Geological Sciences, Harvard 
					University, Cambridge, Massachusetts.  184 pp.
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
			11/10/88  Dennis O'Neill  added smart byte swapping
			11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

#define MASK 0x00000003		/* mask for bottom 2 bits */
#define SHIFT 2			/* # bits per decompres flag */
#define MAX8  0x7f		/* maximum  8 bit positive # */
#define MAX16 0x7fff		/* maximum 16 bit positive # */
#define MAX32 0x7fffffff	/* maximum 32 bit positive # */
#define ELEMENTS_FRAME 16	/* # elements per data frame */

static int samp_cnter;

extern struct stn_list *stn_listhead;

void decode_steim (data_ptr, nsamples, index)
char *data_ptr;
int nsamples;
int index;

{
	unsigned short int *temp_u2byte;/* temp for byte swapping */
	short int *temp_2byte;		/* temp for byte swapping */
	int *temp_4byte;		/* temp for byte swapping */
	char *temp_byte;		/* temp for decompression */
	int temp;			/* temp for decompression */
	double sample;			/* temp for decompression */
	double last_value;		/* last value, previous blk */
	int initial_value;		/* first value, this blk */
	int final_value;		/* last value, this blk */

	int i;				/* counter */
	int j;				/* counter */
	int k;				/* counter */
	int compression_int;		/* coded compression flags */
	int compression_flag[ELEMENTS_FRAME];	/* compression flags */
	int num_frames;			/* number of frames in block */
	int counter;			/* counter */

	FILE *fp;
	double *sav_ptr;

// int samp;
// static int sample_cnter = 0;


/*                 +=======================================+                 */
/*=================| get first, last values, # of frames   |=================*/
/*                 +=======================================+                 */

	/* find last value from current seismogram DOESN'T WORK YET */
	if (index != 0) last_value = (double) *(seismic_data_ptr - 1);
	else last_value = 0;

	/* recover this block's intial and final sample values from 1st frame */
	temp_4byte = (int *) (data_ptr + 4);
	if (byteswap == TRUE) *temp_4byte = swap_4byte (*temp_4byte);
	initial_value = (int) *temp_4byte;

	temp_4byte = (int *) (data_ptr + 8);
	if (byteswap == TRUE) *temp_4byte = swap_4byte (*temp_4byte);

	final_value = (int) *temp_4byte;

	num_frames = (LRECL - data_hdr->bod) / (ELEMENTS_FRAME * 4); 

/*                 +=======================================+                 */
/*=================|           process each frame          |=================*/
/*                 +=======================================+                 */

	counter = 0;
	sav_ptr = seismic_data_ptr;

/*
fp = fopen("steim.out","a+");

if (fp == NULL)
{
	fprintf(stderr, "bad open\n");

}
*/

// fprintf(fp, "Initial = %d\n", initial_value);



	/* process each frame */
	for (i = 0; i < num_frames; i++)
	{

		/* get decompression flags */
		temp_4byte = (int *) data_ptr;
		if (byteswap == TRUE) *temp_4byte = swap_4byte (*temp_4byte);
		compression_int = (int) *temp_4byte;

		for (j = 15; j >= 0; j--)
		{
			compression_flag[j] = (compression_int & MASK);
			compression_int >>= SHIFT;
		}

		data_ptr += 4;

/*                 +=======================================+                 */
/*=================|         decompress each frame         |=================*/
/*                 +=======================================+                 */

		/* decompress this frame of data */
		for (j = 1; j < 16; j++)
		{
			switch (compression_flag[j])
			{
			case 1:		/* type 1, 4 differences */
				for (k = 0; k < 4; k++, data_ptr += 1)
				{
					temp_byte = (char *) data_ptr;
					temp = (int) *temp_byte;

					if (temp > MAX8) 
						temp -= 2 * (MAX8 + 1);

					if ((i == 0) && (j == 3) && (k == 0))
                            			last_value = 	
							initial_value - temp;

					sample = (double) temp + last_value;

/*
samp= sample;

fprintf(fp, "samp[%d]=%d\n", sample_cnter, samp);

sample_cnter++;
*/


					if (counter < nsamples)
					{
						*seismic_data_ptr = sample;
						seismic_data_ptr += 1;
					}
					last_value = sample;
					counter++;
				}

				break;

			case 2:		/* type 2, 2 differences */
				for (k = 0; k < 2; k++, data_ptr += 2)
				{
					temp_2byte = (short *) data_ptr;
					if (byteswap == TRUE)
						*temp_2byte = swap_2byte (*temp_2byte);

					temp = (int) *temp_2byte;

					if (temp > MAX16) 
						temp -= 2 * (MAX16 + 1);


					if ((i == 0) && (j == 3) && (k == 0))
                            			last_value = 
						  initial_value - temp;

					sample = (double) temp + last_value;
/*
samp= sample;
fprintf(fp, "samp[%d]=%d\n", sample_cnter, samp);
sample_cnter++;
*/

					if (counter < nsamples)
					{
						*seismic_data_ptr = sample;

						seismic_data_ptr += 1;
					}

					last_value = sample;
					counter++;

				}

				break;

			case 3:		/* type 3, 1 differences */
				for (k = 0; k < 1; k++, data_ptr += 4)
				{

					temp_4byte = (int *) data_ptr;

					if (byteswap == TRUE)
						*temp_4byte = swap_4byte (*temp_4byte);

					temp = (int) *temp_4byte;

					if (temp > MAX32) 
						temp -= 2 * (((unsigned)MAX32) + 1);

					if ((i == 0) && (j == 3) && (k == 0))
                            			last_value = 
							initial_value - temp;

					sample = (double) temp + last_value;
/*
samp=sample;
fprintf(fp, "samp[%d]=%d\n", sample_cnter, samp);
sample_cnter++;
*/

					if (counter < nsamples)
					{
						*seismic_data_ptr = sample;
						seismic_data_ptr += 1;
					}

					last_value = sample;
					counter++;

				}
				break;

			case 0:		/* type 0, not data */
				default:
					data_ptr += 4;
					break;

			}	/* switch loop */

		}	/* single frame for loop */


	}		/* all frames for loop */

	if (nsamples < counter)
	{
#if 0

		fprintf(stderr, "!!! WARNING !!!  Steim Decompression Sample Count Error at Record: %d, Sample: %d\n", input.recordnumber, index+nsamples);
		fprintf(stderr, "Expected %d, found %d samples... Extra values will be Truncated\n", nsamples, counter); 

#endif

	}
	else if (nsamples > counter)
	{

		fprintf(stderr, "!!! WARNING !!!  Steim Decompression Sample Count Error at Sample: %d\n",
index+nsamples);
		fprintf(stderr, "LRECL = %d,Expected %d, found %d samples... Lost values will be Padded with Zeros\n", LRECL,nsamples, counter);

		for (i=0;i<(nsamples-counter); i++) *seismic_data_ptr++ = 0.0; 
	}
/*
	fp = fopen("steim.out","a+");

    	for (i = 0; i < counter; i++)
    	{
		fprintf(fp,"Sample #%d=%d\n", samp_cnter, (int) *sav_ptr);
		sav_ptr += 1;
		samp_cnter++;

	}

	fclose(fp); 


*/
}				/* subprocedure */

