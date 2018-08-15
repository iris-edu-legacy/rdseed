/*===========================================================================*/
/* SEED reader     |            decode_ieeedouble 	   |    process data */
/*===========================================================================*/
/*
	Name:		decode_ieeedouble
	Purpose:	translate SEED IEEE float tape data from stations to numbers
	Usage:		void decode_ieeedouble();
				char *data_ptr;
				int nsamples;
				decode_ieeedouble(data_ptr, nsamples, index);
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

void decode_ieeedouble (data_ptr, nsamples, index)
char *data_ptr;
int nsamples;
int index;
{

        union {
                double d;
                unsigned char a[8];
        } doub;

	double d;

	char *temp;		/* recovers sample from SEED */

	int i, ix;			/* counter */

	char p0, p1;
	char p2, p3;

/*                +=========================================+                 */
/*================| calculate samples for IEEE 32 bit float |=================*/
/*                +=========================================+                 */

	for (i = index; i < index + nsamples; i++, data_ptr += 8)
	{
		/* recover mantissa and gain range factor */

		if (byteswap == TRUE)
		{
			temp = (char *) data_ptr;

			p0 = *temp;
			p1 = *(temp + 1);
			p2 = *(temp + 2);
			p3 = *(temp + 3);

			*temp = *(temp + 7);
			*(temp + 1) = *(temp + 6);
			*(temp + 2) = *(temp + 5);
			*(temp + 3) = *(temp + 4);

			*(temp + 7) = p0;
			*(temp + 6) = p1;
			*(temp + 5) = p2;
			*(temp + 4) = p3;

		}

		for (ix = 0; ix < sizeof(double); ix++)
		{
			doub.a[ix] = (unsigned char)*(data_ptr + ix);
		}

		/* save sample in seismic data array */
		*seismic_data_ptr = doub.d;

		seismic_data_ptr += 1;
	}

}
