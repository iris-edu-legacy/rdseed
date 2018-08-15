/*===========================================================================*/
/* SEED reader     |             decode_dwwssn             |    process data */
/*===========================================================================*/
/*
	Name:		decode_dwwssn
	Purpose:	translate SEED tape data for CDSN stations to numbers
	Usage:		void decode_dwwssn ();
				char *data_ptr;
				int nsamples;
				decode_dwwssn (data_ptr, nsamples, index);
	Input:		data_ptr - pointer to beginning of seismic data in a SEED file
				nsamples - number of samples
				index - index of next entry into output array
	Output:		an array of numbers (seismic data)
	Externals:	
	Warnings:	none
	Errors:		none
	Called by:	process_data
	Calls to:	none
	Algorithm:	
	Notes:		DWWSSN data are compressed according to the formula

					sample = M

				where 
					sample = seismic data sample 
					M      = mantissa, written to tape
				Data are stored on tape in two bytes as follows:	
					fedc ba98 7654 3210 = bit number, power of two
					MMMM MMMM MMMM MMMM = form of SEED data
					where G = gain range factor and M = mantissa
				No masks are needed to recover DWWSSN mantissae from tape.
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	Sun OS 3.2 C
	Revisions:	07/15/88  Dennis O'Neill  Preliminary initial release 0.9
				11/10/88  Dennis O'Neill  added smart byte swapping
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

#define MAX16 0x7fff							/* maximum 16 bit positive # */

void decode_dwwssn (data_ptr, nsamples, index)
char *data_ptr;
int nsamples;
int index;
{
	short *temp;								/* recovers sample from SEED */
	int mantissa;								/* mantissa from SEED data */
	int i;										/* counter */

double *sav_ptr = seismic_data_ptr;

/*                 +=======================================+                 */
/*=================|     define constants for decoding     |=================*/
/*                 +=======================================+                 */

/*                 +=======================================+                 */
/*=================|     calculate samples for DWWSSN      |=================*/
/*                 +=======================================+                 */

	for (i = index; i < index + nsamples; i++, data_ptr += 2)
	{
		/* recover mantissa and gain range factor */
		temp = (short *) data_ptr;
		if (byteswap == TRUE) *temp = swap_2byte (*temp);
		mantissa = (int) *temp;

		/* take 2's complement for mantissa */
		if (mantissa > MAX16) mantissa -= 2 * (MAX16 + 1);

		/* calculate sample from mantissa and multiplier */
		/* save sample in seismic data array */
		*seismic_data_ptr = (double) mantissa;

		seismic_data_ptr += 1;
	}
 
}
