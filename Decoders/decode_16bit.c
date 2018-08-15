/*==========================================================================*/
/* SEED reader     |             decode_16_bit.c          |     decode data */
/*==========================================================================*/
/*
	Name:		decode_1 6_bit
	Purpose:	translate SEED tape data for IRIS stations (Steim format) to
				numbers
	Usage:		void decode_16_bit();
				char *data_ptr;
				int nsamples;
				decode_16_bit(data_ptr, nsamples, index);
	Input:		data_ptr - pointer to beginning of seismic data in a SEED file
				nsamples - number of samples
				index - index of next entry into output array
	Output:		an array of numbers (seismic data)
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	process_data
	Calls to:	none
	Algorithm:	none.
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

void decode_16bit(data_ptr, nsamples, index)
char *data_ptr;
int nsamples;
int index;

{
	unsigned short int *temp_u2byte;/* temp for byte swapping */
	short int *temp_2byte;		/* temp for byte swapping */

	int i;

/*
FILE *fptr;

fptr = fopen("16bit.out", "a+");
*/



	for (i = 0; i < nsamples; data_ptr += 2, i++)
	{

		temp_2byte = (short *) data_ptr;

		if (byteswap == TRUE)
			*temp_2byte = swap_2byte (*temp_2byte);

		*seismic_data_ptr = (double) *temp_2byte;

/* fprintf(fptr, "Sample [%d]=%6.2f\n", i, *seismic_data_ptr); */

		seismic_data_ptr += 1;


	}

/* fclose(fptr);  */

}				/* subprocedure */

