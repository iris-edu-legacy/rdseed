/*===========================================================================*/
/* SEED reader     |              decode_sro               |    process data */
/*===========================================================================*/
/*
	Name:		decode_sro
	Purpose:	translate SEED tape data for SRO stations to numbers
	Usage:		void decode_sro ();
				char *data_ptr;
				int nsamples;
				decode_sro (data_ptr, nsamples, index);
	Input:		data_ptr - pointer to beginning of seismic data in a SEED file
				nsamples - number of samples
				index - index of next entry into output array
	Output:		an array of numbers (seismic data)
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	process_data
	Calls to:	none
	Algorithm:	
	Notes:		SRO data are compressed according to the formula

					sample = M * (b exp {[m * (G + agr)] + ar})

				where 
					sample = seismic data sample 
					M      = mantissa, written to tape
					G      = gain range factor, written to tape
					b      = base to be exponentiated               =  2 for SRO
					m      = multiplier                             = -1 for SRO
					agr    = term to be added to gain range factor  =  0 for SRO
					ar     = term to be added to [m * (gr + agr)]   = 10 for SRO
					exp    = exponentiation operation
				Data are stored on tape in two bytes as follows:	
					fedc ba98 7654 3210 = bit number, power of two
					GGGG MMMM MMMM MMMM = form of SEED data
					where G = gain range factor and M = mantissa
				Masks to recover gain range and mantissa from tape are:
					fedc ba98 7654 3210 = bit number = power of two
					0000 1111 1111 1111 = 0x0fff     = mask for mantissa
					1111 0000 0000 0000 = 0xf000     = mask for gain range
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	Sun OS 3.2 C
	Revisions:	07/15/88  Dennis O'Neill  Preliminary initial release 0.9
				11/10/88  Dennis O'Neill  added smart byte swapping
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

#define MANTISSA_MASK 0x3fff					/* mask for mantissa */
#define GAINRANGE_MASK 0xd000					/* mask for gainrange factor */
#define SHIFT 14								/* # bits in mantissa */
#define MAX12 0x7ff								/* maximum 12 bit positive # */

float rstn_gain[] = {1,4,16,128};

void decode_rstn (data_ptr, nsamples, index)
char *data_ptr;
int nsamples;
int index;
{
	short *temp;								/* recovers sample from SEED */
	int mantissa;								/* mantissa from SEED data */
	int gainrange;								/* gain range factor */
	int base;									/* base for exponentiation */
	int add2gr;									/* added to gainrange factor */
	int mult;									/* multiplier for terms above */
	int add2result;								/* added to result above */
	int exponent;								/* total exponent */
	int i;										/* counter */

/*                 +=======================================+                 */
/*=================|     define constants for decoding     |=================*/
/*                 +=======================================+                 */

	base = 2;
	add2gr = 0;
	mult = -1;
	add2result = 10;

/*                 +=======================================+                 */
/*=================|      calculate samples for SRO        |=================*/
/*                 +=======================================+                 */

	for (i = index; i < index + nsamples; i++, data_ptr += 2)
	{
		/* recover mantissa and gain range factor */
		temp = (short *) data_ptr;
		if (byteswap == TRUE) *temp = swap_2byte (*temp);
		mantissa = ((int) *temp & MANTISSA_MASK);
		gainrange = ((int) *temp & GAINRANGE_MASK) >> SHIFT;

		/* calculate exponent from gain range factor and format definition */
		exponent = (mult * (gainrange + add2gr)) + add2result;

		if (mantissa > MAX12) mantissa -= 2 * (MAX12 + 1);

		/* calculate sample from mantissa and exponent */
		/* save sample in seismic data array */
		*seismic_data_ptr = (double) (mantissa-8191) * rstn_gain[gainrange];

		seismic_data_ptr += 1;
	}
}
