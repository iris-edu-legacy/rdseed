/*===========================================================================*/
/* SEED reader     |            print_type60               |  station header */
/*===========================================================================*/
/*
	Name:		print_type60
	Purpose:	print the response reference table to standard output
	Usage:		void print_type60 ();
				print_type60 ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type60_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_response
	Calls to:	none
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type60_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Sue Schoch
	Revisions:	06/05/90  Sue Schoch      Initial preliminary release in 2.0
			02/25/99  Stephane Zuzlewski	Added support for blockette 42.
*/

#include "rdseed.h"
#include "resp_defs.h"

void print_type60 (fp,type60_head)
FILE *fp;
struct type60 *type60_head;
{
	struct type60 *type60;						/* looping vbl */
	int i,j;									/* counter */
	char *blkt_id="B060";						/* blockette id string */


	for (type60 = type60_head; type60 != NULL; type60 = type60->next)
	{
		for( i = 0; i < type60->number_stages; i++ )
		{
			fprintf (fp,"%s+            +--------------------------",com_strt);
			fprintf (fp,"------------------------+             +\n");
			fprintf (fp,"%s+            |   Response Reference Information, %5s ch %3s",
					 com_strt, current_station->station, current_channel->channel);
			fprintf (fp,"   |             +\n");
			fprintf (fp,"%s+            +--------------------------",com_strt);
			fprintf (fp,"------------------------+             +\n");
			fprintf (fp,"%s\n",com_strt);
			fprintf (fp,"%s%s%2.2d     Number of Stages:                      %d\n",
					 blkt_id,fld_pref,3,type60->number_stages);
			fprintf (fp,"%s%s%2.2d     Stage number:                          %d\n",
					 blkt_id,fld_pref,4,type60->stage[i].value );
			fprintf (fp,"%s%s%2.2d     Number of Responses:                   %d\n",
					 blkt_id,fld_pref,5,type60->stage[i].number_responses);
			fprintf (fp,"%s\n",com_strt);

			for( j = 0; j < type60->stage[i].number_responses; j++)
			{
				if (find_type41(fp,type60->stage[i].response[j].reference));
				else if (find_type42(fp,type60->stage[i].response[j].reference));
				else if (find_type43(fp,type60->stage[i].response[j].reference));
				else if (find_type44(fp,type60->stage[i].response[j].reference));
				else if (find_type45(fp,type60->stage[i].response[j].reference));
				else if (find_type46(fp,type60->stage[i].response[j].reference));
				else if (find_type47(fp,type60->stage[i].response[j].reference));
				else if (find_type46(fp,type60->stage[i].response[j].reference));
				else if (find_type48(fp,type60->stage[i].response[j].reference));
				else fprintf (fp,"!!! ERROR - Response Description Code %d  NOT FOUND !!!\n", type60->stage[i].response[j].reference);
			}
		}
		fprintf (fp,"%s\n",com_strt);
	}
}

void old_print_type60 (fp,type60_head)
FILE *fp;
struct type60 *type60_head;
{
	struct type60 *type60;							/* looping vbl */
	int i,j;										/* counter */

	fprintf (fp,"+                  +--------------------");
	fprintf (fp,"-------------------+                  +\n");
	fprintf (fp,"+                  |   Instrument response, %5s ch %3s",
		current_station->station, current_channel->channel);
	fprintf (fp,"   |                  +\n");
	fprintf (fp,"+                  +--------------------");
	fprintf (fp,"-------------------+                  +\n");
	fprintf (fp,"\n");

	fprintf(fp, "\nBlockette 60\n\n");

	for (type60 = type60_head; type60 != NULL; type60 = type60->next)
	{
/*
			fprintf (fp,"Number of Stages:                      %d\n",
			type60->number_stages);
*/

		for( i = 0; i < type60->number_stages; i++ )
		{
			fprintf (fp,"\nStage number:                          %d\n",
				type60->stage[i].value );
/*
			fprintf (fp,"Number of Responses:                   %d\n",
				type60->stage[i].number_responses);
*/
			for( j = 0; j < type60->stage[i].number_responses; j++)
			{
				if (old_find_type41(fp,type60->stage[i].response[j].reference));
				else if (old_find_type42(fp,type60->stage[i].response[j].reference));
				else if (old_find_type43(fp,type60->stage[i].response[j].reference));
				else if (old_find_type44(fp,type60->stage[i].response[j].reference));
				else if (old_find_type45(fp,type60->stage[i].response[j].reference));
				else if (old_find_type46(fp,type60->stage[i].response[j].reference));
				else if (old_find_type47(fp,type60->stage[i].response[j].reference));
				else if (old_find_type46(fp,type60->stage[i].response[j].reference));
				else if (old_find_type48(fp,type60->stage[i].response[j].reference));
				else fprintf (fp,"!!! ERROR - Response Description Code %d  NOT FOUND !!!\n", type60->stage[i].response[j].reference);
			}
		}
		fprintf (fp,"\n");
	}
}
