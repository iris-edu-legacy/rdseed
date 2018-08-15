/*===========================================================================*/
/* SEED reader     |              print_type30               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		print_type30
	Purpose:	print the contents of the data format dictionary to standard 
				output
	Usage:		void print_type30 ();
				print_type30 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type30_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
				type30 - defined in structures.h, allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_abbrevdic
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type30_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
                02/17/93  CL    nullpointer protect fprintfs
				10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type30 ()
{
	struct type30 *type30;							/* looping vbl */
	int i;										/* counter */
	char *blkt_id="B030";						/* blockette id string */

	if (type30_head != NULL)
	{
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s+------------------|        Data format ",com_strt);
		fprintf(outputfile,"dictionary         |------------------+\n");
		fprintf(outputfile,"%s+                  +--------------------",com_strt);
		fprintf(outputfile,"-------------------+                  +\n");
		fprintf(outputfile,"%s\n",com_strt);

		for (type30 = type30_head; type30 != NULL; type30 = type30->next)
		{
			/* report the values found in the blockette */
			fprintf(outputfile,"%s%s%2.2d     Format name:                           %s\n",
					blkt_id,fld_pref,3,type30->name ? type30->name : "(null)");
			fprintf(outputfile,"%s%s%2.2d     Format code:                           %d\n",
					blkt_id,fld_pref,4,type30->code);
			fprintf(outputfile,"%s%s%2.2d     Data family:                           %d\n",
					blkt_id,fld_pref,5,type30->family);
			for (i = 0; i < type30->number_keys; i++) 
				fprintf(outputfile,"%s%s%2.2d     Decoder key %03d:                       %s\n",
						blkt_id,fld_pref,7,i+1,type30->decoder_key[i] ? type30->decoder_key[i] : "(null)");
			fprintf(outputfile,"%s\n",com_strt);
		}
	}
}
