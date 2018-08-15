/*===========================================================================*/
/* SEED reader     |              decode_32bit             |    process data */
/*===========================================================================*/
/*
	Name:		decode_32bit
	Purpose:	translate SEED tape data from UCSD stations to numbers
	Usage:		void decode_32bit ();
				char *data_ptr;
				int nsamples;
				decode_32bit (data_ptr, nsamples, index);
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
	Notes:		32 bit integer data are compressed according to the formula

				sample = M

			where 
				sample = seismic data sample 
				M      = mantissa, written to tape
			Data are stored on tape in four bytes as follows:	
                 1111 1111 1111 1111 0000 0000 0000 0000
		fedc ba98 7654 3210 fedc ba98 7654 3210 = bit #, pwr of two
		MMMM MMMM MMMM MMMM MMMM MMMM MMMM MMMM = form of SEED data
		where G = gain range factor and M = mantissa
		No masks are needed to recover 32 bit integer mantissae from
		tape.
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	Sun OS 3.2 C
	Revisions:	02/06/89  Dennis O'Neill  Initial version
			12/03/95  CL - changed a cast from unsigned to signed 

*/

#include "rdseed.h"

#define MAX32 0x7fffffff						/* maximum 32 bit positive # */

void decode_32bit (data_ptr, nsamples, index)
char *data_ptr;
int nsamples;
int index;
{
	int *temp;		/* recovers sample from SEED */
	int mantissa;		/* mantissa from SEED data */
	int i;			/* counter */

double *sav_ptr = seismic_data_ptr;

/*                 +=======================================+                 */
/*=================|     define constants for decoding     |=================*/
/*                 +=======================================+                 */

/*                 +=======================================+                 */
/*=================| calculate samples for 32 bit integers |=================*/
/*                 +=======================================+                 */

	for (i = index; i < index + nsamples; i++, data_ptr += 4)
	{
		/* recover mantissa and gain range factor */
		temp = (int *) data_ptr;

		if (byteswap == TRUE) 
			*temp = swap_4byte (*temp);

		mantissa = (int) *temp;

		/* calculate sample from mantissa and multiplier */
		/* save sample in seismic data array */
		*seismic_data_ptr = (double) mantissa;

		seismic_data_ptr += 1;

	}
}
