/*===========================================================================*/
/* SEED reader     |               swap_2byte              |    subprocedure */
/*===========================================================================*/
/*
	Name:		swap_2byte
	Purpose:	reorder a 2-byte word from 10 to 01 (MSB-first to MSB-last)
				or from 01 to 10
	Usage:		unsigned short int swap_2byte ();
				unsigned short int word2;
				unsigned short int result;
				result = swap_2byte (word2);
	Input:		a 2-byte word in order 10
	Output:		a 2-byte word in order 01
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	anything
	Calls to:	none
	Algorithm:	Using a union between an unsigned short int and 2 chars,
				shuffle the bytes around to achieve the reverse word order.
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	11/09/88  Dennis O'Neill  original version
				11/21/88  Dennis O'Neill  Production release 1.0
*/

unsigned short int swap_2byte (word2)
unsigned short int word2;
{
	union
	{
		unsigned char character[2];
		unsigned short int integer;
	} swap2byte;								/* holds 2-byte word */
	char temp0;

	swap2byte.integer = word2;

	temp0 = swap2byte.character[0];
	swap2byte.character[0] = swap2byte.character[1];
	swap2byte.character[1] = temp0;

	return (swap2byte.integer);
}
