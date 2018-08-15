/*===========================================================================*/
/* SEED reader     |             find_wordorder            |    subprocedure */
/*===========================================================================*/
/*
	Name:		find_wordorder
	Purpose:	determine the word order of the machine on which it's running
	Usage:		int find_wordorder ();
			int result;
			result = find_wordorder ();
	Input:		none
	Output:		FALSE = word order is most-significant-byte first, i.e., 3210
			TRUE = word order is least-significant-byte first, i.e., 0123
	Externals:	none
	Warnings:	none
	Errors:		word order not recognized
	Called by:	main
	Calls to:	none
	Algorithm:	using a union construct, place known numbers into a 4-byte
			word.  Compare the integer represented by that word to its
			known value in MSB-first and MSB-last schema.
	Notes:		Typical usage would be:
			int BYTESWAP;
			BYTESWAP = find_wordorder ();
			where BYTESWAP may be TRUE or FALSE.
			Because this program was written on a Sun computer with word
			order MSB-first, it returns FALSE if no byte-swapping is
			necessary, TRUE otherwise.  That is, FALSE indicates that
the machine is a MSB-first machine.
			MSB-first is commonly described as "big-endian", and
			MSB-last is commonly described as "little-endian".
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	11/09/88  Dennis O'Neill  Initial version
				11/21/88  Dennis O'Neill  Production release 1.0
*/
#include "rdseed.h"

#define MSB_FIRST 0x76543210			/* MC680x0 word order */
#define MSB_LAST  0x10325476			/* VAX, 80x86 word order */

int find_wordorder (mini_hdr)
struct data_blk_1000 *mini_hdr;

{
	union
	{
		unsigned char character[4];
		unsigned int integer;
	} test4;	/* holds test 4-byte word */

/*  not used

	union
	{
		unsigned char character[2];
		unsigned short int integer;
	} test2; */	/* holds test 2-byte w



	char wordorder[4+1];			/* string for word order */

	/* Construct a 4-byte word of known contents - 0x76543210 */
	test4.character[0] = 0x76;
	test4.character[1] = 0x54;
	test4.character[2] = 0x32;
	test4.character[3] = 0x10;

	/* Construct a 2-byte word of known contents - 0xDCBA */
	/* not used
	test2.character[0] = 0xDC;
	test2.character[1] = 0xBA;
	*/

	/* make check for valid word order entry in mseed */
	if (mini_hdr)
	{

		if ((mini_hdr->word_order > 1))
		{
			fprintf(stderr, "Error - bad miniseed word order detected! Defaulting to station information.\nStation: %s:channel%s\n",
					current_station->station,
					current_channel->channel);

			mini_hdr = 0;

		}
	}

	/* determine the 4-byte word order of this machine */
	if      (test4.integer == MSB_FIRST)
      	{

		/* mseed overrides the station info */
		if (mini_hdr)
		{

			/* 0 == mseed in INTEL order */
			if (mini_hdr->word_order == 0)
				/* byteswapping needed */
				return TRUE;
			else 
				return FALSE;
		}

      		if (current_station->word_order == 10) 
			return (FALSE);

		return(TRUE);
      	}
	else 
	if (test4.integer == MSB_LAST)
      	{

		if (mini_hdr) 
			/* 1 == miniseed in SPARC word order */
                        if (mini_hdr->word_order == 1) 
				/* byteswapping needed */
                                return TRUE; 
                        else  
                                return FALSE; 

      		if (current_station->word_order == 1) 
			return (FALSE);

      		return (TRUE);
      	}
	else
	{
		fprintf (stderr, "ERROR (find_wordorder):  ");

		fprintf (stderr, 
			"machine word order, %d, not treated by byte swappers.\n",
			test4.integer);

		fprintf (stderr, "Execution terminating.\n");
		exit (-1);
	}
}

/* ---------------------------------------------------------------------- */

/* see SEED manual for blockette 1000 word order */
#define REAL_HARDWARE 1
#define INTEL_VAX 0


int get_word_order()
{
	union
	{
		unsigned char character[4];
		unsigned int integer;
	} test4;	/* holds test 4-byte word */

	char wordorder[4+1];			/* string for word order */

	/* Construct a 4-byte word of known contents - 0x76543210 */
	test4.character[0] = 0x76;
	test4.character[1] = 0x54;
	test4.character[2] = 0x32;
	test4.character[3] = 0x10;

	/* determine the 4-byte word order of this machine */
	if      (test4.integer == MSB_FIRST)
      	{
		return(REAL_HARDWARE);
      	}
	else 
	if (test4.integer == MSB_LAST)
      	{
      		return (INTEL_VAX);
      	}


	fprintf (stderr, "ERROR get_word_order(): Unable to determine the machine's word order. Assuming 68000\n ");

	return(REAL_HARDWARE);

}


