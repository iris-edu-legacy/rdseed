/*===========================================================================*/
/* SEED reader     |             find_type30               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		find_type30
	Purpose:	print the contents of the units dictionary to standard output
	Usage:		void print_type34 ();
				print_type34 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type34_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_abbrevdic
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type34_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Tim Ahern
	
	Updates:	CL - null-pointer protect fprintf

	Revisions:	10/13/95  TJM   prefix strings added to make consistent with RESP files
*/

#include "rdseed.h"
#include "resp_defs.h"

void find_type30 (fp,code)
FILE *fp;
int code;
{
	int i;
	struct type30 *type30;							/* looping vbl */
	char *blkt_id="B030";						/* blockette id string */

	for (type30 = type30_head; type30 != NULL; type30 = type30->next)
		{
		if (type30->code == code){
			fprintf(fp,"Format Information Follows\n");
			fprintf(fp,"%s%s%2.2d          Format Name: %s\n",
					blkt_id,fld_pref,3,type30->name ? type30->name : "(null)");

			fprintf (fp,"%s%s%2.2d          Data family:  %3d          \n",
					 blkt_id,fld_pref,5,type30->family);

			fprintf(fp,"%s%s%2.2d          Number of Keys:  %3d        \n",
					blkt_id,fld_pref,6,type30->number_keys);

			for(i=0;i<type30->number_keys;i++){
				fprintf(fp,"%s%s%2.2d             Key %2d: %s \n",
							blkt_id,fld_pref,7,i+1, 
							type30->decoder_key[i] ? type30->decoder_key[i] : "(null)");
				}
			break;
 			}
		}
	if (type30 == NULL) fprintf(fp,"!!! ERROR - Format Code Not Found !!!\n");
}
