/*===========================================================================*/
/* SEED reader     |            decode_geoscope           |    process data */
/*===========================================================================*/
/*
	Name:	decode_geoscope
	Purpose:translate SEED tape data for GEOSCOPE stations to numbers
	Usage:	void decode_geoscope ();
		char *data_ptr;
		int nsamples;
		decode_geoscope (data_ptr, nsamples, index);
	Input:	data_ptr - pointer to beginning of seismic data in a SEED file
		nsamples - number of samples
		index - index of next entry into output array
	Output:	an array of numbers (seismic data)
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	process_data
	Calls to:	none
	Algorithm:	Translate geoscope-compressed data to numbers
	Notes:		geoscope data are compressed according to the formula

			sample = M * (b exp {[m * (G + agr)] + ar})

			where 
				sample = seismic data sample 
				M      = mantissa, written to tape
				G      = gain range factor, written to tape
				b      = base to be exponentiated = 2 for SRO
				m      = multiplier  = -1 for SRO
				agr    = term to be added to gain range factor = 0 for SRO
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
	Revisions:	07/15/88  Dennis O'Neill  Initial release
				11/10/88  Dennis O'Neill  added smart byte swapping
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

#define MANTISSA_MASK 0x0fff		/* mask for mantissa */
#define GAIN3_MASK 0x7000		/* mask for gainrange factor */
#define GAIN4_MASK 0xf000		/* mask for gainrange factor */
#define SHIFT 12			/* # bits in mantissa */
#define MAX12 0x7ff			/* maximum 12 bit positive # */
#define MAX24 0x7fffff			/* maximum 24 bit positive # */

void decode_geoscope (data_ptr, nsamples, index, name)
char *data_ptr;
int nsamples;
int index;
char  *name;
{

        // double  *sav_ptr;


	short *temp;		/* recovers sample from SEED */
	int mantissa;		/* mantissa from SEED data */
	int gainrange;		/* gain range factor */
	int base;		/* base for exponentiation */
	int add2gr;		/* added to gainrange factor */
	int mult;		/* multiplier for terms above */
	int add2result;		/* added to result above */
	int exponent;		/* total exponent */
	int demux;		/* demux pointer */
	int i, j, k;		/* counter */
	union   {		/* byte */
		char c[4];
		int  i;
		} data;

        int code;

/*                 +=======================================+                 */
/*=================|     define constants for decoding     |=================*/
/*                 +=======================================+                 */

	base = 2;
	add2gr = 0;
	mult = -1;
	add2result = 0;

        // sav_ptr = seismic_data_ptr;

 	data_hdr->num_mux_chan = 3; 

/*	data_hdr->num_mux_chan = 1; */

/*                 +=======================================+                 */
/*=================|      calculate samples for SRO        |=================*/
/*                 +=======================================+                 */

	if ( strncasecmp(name,"Geoscope gain-range on 3 bits",29) == 0)
     	code =1; 
	else if ( strncasecmp(name,"Geoscope-3 byte",15) == 0)
     	code =2; 
	else if ( strncasecmp(name,"Geoscope gain range on 4 bits",29) == 0)
     	code =3; 
	else 
	{
		fprintf(stderr, "Error: decode_geoscope > Unknown data type: %s\n",name);
       	exit(1);
	}

	if (strstr(name, "DEMUXED"))
		data_hdr->num_mux_chan = 1;
	else
		data_hdr->num_mux_chan = 3;
	
	demux = 0;

	for (i = index; i < index + nsamples; i++)
	{

		for (j = demux = 0; j < data_hdr->num_mux_chan; j++, demux += seis_buffer_length/3)
		{

		switch (code)
		{
			case 1:
				temp = (short *) data_ptr;
				if (byteswap == TRUE) *temp = swap_2byte (*temp);
				/* recover mantissa and gain range factor */
				mantissa = ((int) (*temp & MANTISSA_MASK));
				gainrange = ((unsigned int) (*temp & GAIN3_MASK)) >> SHIFT;
		
				/* calculate exponent from gain range factor and format definition */
				exponent = gainrange;

				/* calculate sample from mantissa and exponent */
				*(seismic_data_ptr+demux) = ((double) (mantissa-2048)) / pow((double) base, (double) exponent);

				break;
			case 2:
				data.i = 0;
				if (byteswap == TRUE)
					for (k=0;k<3;k++) 
						data.c[2-k] = data_ptr[k];
					else
					for (k=0;k<3;k++) 
						data.c[1+k] = data_ptr[k];

				mantissa = data.i;
				/* take 2's complement for mantissa */
				if (mantissa > MAX24) 
					mantissa -= 2 * (MAX24 + 1);

				/* calculate sample from mantissa and exponent */
				*(seismic_data_ptr+demux) = (double) mantissa;
				break;
			case 3:
				temp = (short *) data_ptr;
				if (byteswap == TRUE) 
					*temp = swap_2byte (*temp);

				/* recover mantissa and gain range factor */
				mantissa = ((int) (*temp & MANTISSA_MASK));

				gainrange = ((unsigned int) (*temp & GAIN4_MASK)) >> SHIFT;

				/* calculate exponent from gain range factor and format definition */
				exponent = gainrange ;

				/* take 2's complement for mantissa */
				/* if (mantissa > MAX12) mantissa -= 2 * (MAX12 + 1); */

				/* calculate sample from mantissa and exponent */
				*(seismic_data_ptr+demux) =((double) (mantissa-2048)) / pow ((double) base, (double) exponent);

				break;
		}

		/* save sample in seismic data array */
		// *(seismic_data_ptr+demux) = sample;


		switch(code)
		{
			case 1: case 3: data_ptr += 2; break;
			case 2: data_ptr += 3; break;
		} 
		}               

		seismic_data_ptr += 1;
	}

}
