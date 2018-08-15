/*===========================================================================*/
/* SEED reader     |              decode_cdsn              |    process data */
/*===========================================================================*/
/*
	Name:		decode_cdsn
	Purpose:	translate SEED tape data for CDSN stations to numbers
	Usage:		void decode_cdsn ();
				char *data_ptr;
				int nsamples;
				decode_cdsn (data_ptr, nsamples, index);
	Input:		data_ptr - pointer to beginning of seismic data in a SEED file
				nsamples - number of samples
				index - index of next entry into output array
	Output:		an array of numbers (seismic data)
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	process_data
	Calls to:	none
	Algorithm:	translate CDSN-compressed data to numbers
	Notes:		CDSN data are compressed according to the formula

					sample = M * (2 exp G)

				where 
					sample = seismic data sample 
					M      = mantissa; biased mantissa B is written to tape
					G      = exponent of multiplier (i.e. gain range factor); 
							 key K is written to tape
					exp    = exponentiation operation
					B      = M + 8191, biased mantissa, written to tape
					K      = key to multiplier exponent, written to tape
							 K may have any of the values 0 - 3, as follows:
							 0 => G = 0, multiplier = 2 exp 0 = 1
							 1 => G = 2, multiplier = 2 exp 2 = 4
							 2 => G = 4, multiplier = 2 exp 4 = 16
							 3 => G = 7, multiplier = 2 exp 7 = 128
				Data are stored on tape in two bytes as follows:	
					fedc ba98 7654 3210 = bit number, power of two
					KKBB BBBB BBBB BBBB = form of SEED data
					where K = key to multiplier exponent and B = biased mantissa

				Masks to recover key to multiplier exponent and biased mantissa
				from tape are:
					fedc ba98 7654 3210 = bit number = power of two
					0011 1111 1111 1111 = 0x3fff     = mask for biased mantissa
					1100 0000 0000 0000 = 0xc000     = mask for gain range key

				Where does the bias number 8191 come from?  I think it's 
				derived as follows.  Assume a 21 bit recording system.  Such
				a system has a range of
					(2**(21-1)) - 1 >= x >= -2**(21-1), or
					1048575 >= x >= -1048576.
				The dynamic range of such a system can be closely 
				approximated in a 16 bit number, where 2 bits of the 16 bit
				number are a coded multiplier and 14 bits represent an
				unsigned number biased from its true value by a constant.
				In this case, the range of the 16 bit system is
					(((2**14) - 1) - (2**(14-1) - 1)) * (2**7) >= x
					   |              |                  ^- maximum multiplier
					   |              max signed 14 bit number
					   max unsigned 14 bit number
					((16384 - 1) - (8192 - 1)) * (128) >= x 
					(16383 - 8191) * 128 >= x
					         ^- here it is!!
					1048576 >= x

					((0) - (2**(14-1) - 1)) * (2**7) <= x
					(0 - 8191) * 128 <= x
					-1048448 <= x
				So the range of this gain-ranged 16-bit system, i.e.,
					1048576 >= x >= -1048448
				is a very close approximation to the range of a 21 bit 
				system.  Clever folks, these seismology types.

				My apologies to those to whom this was obvious.  I wanted
				to understand where that magic number came from.
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	Sun OS 3.2 C
	Revisions:	07/15/88  Dennis O'Neill  Preliminary initial release 0.9
				11/10/88  Dennis O'Neill  added smart byte swapping
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

#define MANTISSA_MASK 0x3fff					/* mask for mantissa */
#define GAINRANGE_MASK 0xc000					/* mask for gainrange factor */
#define SHIFT 14								/* # bits in mantissa */
#define MAX14 0x1fff							/* maximum 14 bit positive # */
/*
#define BIAS 0x1fff = 8191                      -- note that this is MAX14 --
*/

void decode_cdsn (data_ptr, nsamples, index)
char *data_ptr;
int nsamples;
int index;
{
	short *temp;								/* recovers sample from SEED */
	int mantissa;								/* mantissa from SEED data */
	int gainrange;								/* gain range factor */
	int mult;									/* multiplier for terms above */
	double sample;								/* sample value */
	int i;										/* counter */
int ix = 0;

double *sav_ptr = seismic_data_ptr;

/*                 +=======================================+                 */
/*=================|     define constants for decoding     |=================*/
/*                 +=======================================+                 */

/*                 +=======================================+                 */
/*=================|      calculate samples for CDSN       |=================*/
/*                 +=======================================+                 */

	for (i = index; i < index + nsamples; i++, data_ptr += 2)
	{
		/* recover mantissa and gain range factor */
		temp = (short *) data_ptr;
// printf("data[0,1]=%d, %d\n", data_ptr[0], data_ptr[1]);

// printf("temp=%d\n", *temp);

		if (byteswap == TRUE) *temp = swap_2byte (*temp);
		mantissa = ((int) *temp & MANTISSA_MASK);
		gainrange = ((int) *temp & GAINRANGE_MASK) >> SHIFT;

// printf("man/gain=%d, %d\n", mantissa, gainrange);

		/* determine multiplier from gain range factor and format definition */
		/* because shift operator is used later, these are powers of two */
		if (gainrange == 0) mult = 0;
		else if (gainrange == 1) mult = 2;
		else if (gainrange == 2) mult = 4;
		else if (gainrange == 3) mult = 7;

		/* unbias the mantissa */
// printf("mantissa=%d, MAX14=%d\n", mantissa, MAX14);

		mantissa -= MAX14;

// printf("After man -= MAX14, mantissa=%d\n", mantissa);

		/* calculate sample from mantissa and multiplier using left shift */
		/* mantissa << mult is equivalent to mantissa * (2 exp (mult)) */
		/* save sample in seismic data array */
		*seismic_data_ptr = (double) (mantissa << mult);

		seismic_data_ptr += 1;
	}
}
