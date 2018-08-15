#include "rdseed.h"

#define MANTISSA_MASK 0xfff0					/* mask for mantissa */
#define GAINRANGE_MASK 0x000f					/* mask for gainrange factor */
#define SHIFT  4								/* # bits in mantissa */
float grf[15] = {1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1,
			0.5, 0.25, 0.125, 0.0625};

void decode_graef (data_ptr, nsamples, index)
char *data_ptr;
int nsamples;
int index;
{
	short *temp;								/* recovers sample from SEED */
	int mantissa;								/* mantissa from SEED data */
	int gainrange;								/* gain range factor */
	int mult;									/* multiplier for terms above */
	int i;										/* counter */

	unsigned int tim1 , tim2;
                   /*=====================================*/
/*=================|     define constants for decoding     |=================*/
                   /*=====================================*/

                   /*=====================================*/
/*=================|      calculate samples for CDSN       |=================*/
                   /*=====================================*/

	for (i = index; i < index + nsamples; i++, data_ptr += 2)
	{
		/* recover mantissa and gain range factor */
		temp = (short *) data_ptr;
		if (byteswap == TRUE) *temp = swap_2byte (*temp);
		mantissa = ((int) *temp & MANTISSA_MASK) >> SHIFT;
		gainrange = ((int) *temp & GAINRANGE_MASK);

		/* determine multiplier from gain range factor and format definition */
		/* because shift operator is used later, these are powers of two */
		if ( mantissa >= 2048 ) mantissa -= 4096;

		/* save sample in seismic data array */
		*seismic_data_ptr  = (double) (mantissa *grf[gainrange]);

		seismic_data_ptr += 1;
	}
}
