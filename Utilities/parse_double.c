/*===========================================================================*/
/* SEED reader     |              parse_double             |         utility */
/*===========================================================================*/
/*
	Name:		parse_double
	Purpose:	parse a double (float) field from a blockette
	Usage:		double parse_double ();
				char *blockette_ptr;
				int number_of_bytes;
				double result;
				result = parse_double (&blockette_ptr, number_of_bytes);
	Input:		blockette_ptr = ptr to the blockette_ptr
				number_of_bytes = number of bytes from blockette to process
	Output:		blockette_ptr = points to beginning of next string
				result = the value of the double field
	Externals:	temp_char - allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	any parsing procedure
	Calls to:	memncpy - does what strncpy is alleged to do
	Algorithm:	get number_of_bytes bytes from the blockette; increment the
				blockette pointer; return the double value of the string 
				to the caller
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				04/26/93  Allen Nance     Had added some code to compensate for
                                          space between E and exponent, bit it
                                          didn't work in all cases.
*/

#include "rdseed.h"
#include <stdlib.h>

#include <math.h>


double parse_double (blockette_ptr, number_of_bytes)
char **blockette_ptr;
int number_of_bytes;
{
	double mantissa, exp;

	memcpy (temp_char, *blockette_ptr, number_of_bytes);	/* no memNcpy in ANSI C */
	*blockette_ptr += number_of_bytes;
	temp_char[number_of_bytes] = 0;
	if (strstr(temp_char, "E ") != NULL)	/* catch 'funny' conditions 	JAL001 */
	{
		sscanf(temp_char, "%lfE %lf", &mantissa, &exp);
		return( mantissa * pow(10.0, exp));
	}
	else if (strstr(temp_char, "e ") != NULL)
	{
		sscanf(temp_char, "%lfe %lf", &mantissa, &exp);
		return( mantissa * pow(10.0, exp));
	}
	else
	{
		return (atof (temp_char));
	}

}
