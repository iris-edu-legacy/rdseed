/*===========================================================================*/
/* SEED reader     |            decode_ieeefloat           |    process data */
/*===========================================================================*/
/*
	Name:		decode_ieeefloat
	Purpose:	translate SEED IEEE float tape data from stations to numbers
	Usage:		void decode_ieeefloat ();
				char *data_ptr;
				int nsamples;
				decode_ieeefloat (data_ptr, nsamples, index);
	Input:		data_ptr - pointer to beginning of seismic data in a SEED file
				nsamples - number of samples
				index - index of next entry into output array
	Output:		an array of numbers (seismic data)
	Externals:	
	Warnings:	none
	Errors:		none
	Called by:	process_data
	Calls to:	none
	Algorithm:	IEEE 32 bit float data are decoeded.
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	Sun OS 3.2 C
	Revisions:	09/29/92  Allen Nance  Initial version
*/

#include "rdseed.h"

void decode_ieeefloat (data_ptr, nsamples, index)
char *data_ptr;
int nsamples;
int index;
{
	char *temp;		/* recovers sample from SEED */
	float *float_ptr;
	int i;			/* counter */

	char p1, p2;




/*                +=========================================+                 */
/*================| calculate samples for IEEE 32 bit float |=================*/
/*                +=========================================+                 */

	for (i = index; i < index + nsamples; i++, data_ptr += 4)
	{

		/* recover mantissa and gain range factor */
		if (byteswap == TRUE)
		{
			temp = (char *) data_ptr;

			p1 = *temp;
			p2 = *(temp + 1);
			*temp = *(temp + 3);
			*(temp + 1) = *(temp + 2);
			*(temp + 2) = p2;
			*(temp + 3) = p1;

		}


		float_ptr = (float *)data_ptr;

		*seismic_data_ptr = (double) *float_ptr;

		seismic_data_ptr += 1;
	}

}
