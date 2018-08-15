/*===========================================================================*/
/* SEED reader     |               swap_4byte              |    subprocedure */
/*===========================================================================*/
/*
	Name:		swap_4byte
	Purpose:	reorder a 4-byte word from 3210 to 0123 (MSB-first to MSB-last)
				or from 0123 to 3210
	Usage:		unsigned long int swap_4byte ();
				unsigned long int word4;
				unsigned long int result;
				result = swap_4byte (word4);
	Input:		a 4-byte word in order 3210
	Output:		a 4-byte word in order 0123
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	anything
	Calls to:	none
	Algorithm:	Using a union between an unsigned long int and 4 chars,
				shuffle the bytes around to achieve the reverse word order.
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	11/09/88  Dennis O'Neill  original version
				11/21/88  Dennis O'Neill  Production release 1.0
*/

unsigned int swap_4byte (word4)
unsigned int word4;
{
	union
	{
		unsigned char character[4];
		unsigned int integer;
	} swap4byte;	/* holds 4-byte word */

	char temp0;
	char temp1;

	swap4byte.integer = word4;

	temp0 = swap4byte.character[0];
	temp1 = swap4byte.character[1];
	swap4byte.character[0] = swap4byte.character[3];
	swap4byte.character[1] = swap4byte.character[2];
	swap4byte.character[2] = temp1;
	swap4byte.character[3] = temp0;

	return (swap4byte.integer);
}
