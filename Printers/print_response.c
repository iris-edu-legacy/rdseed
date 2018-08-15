/*===========================================================================*/
/* SEED reader     |            print_response             |  station header */
/*===========================================================================*/
/*
	Name:		print_response
	Purpose:	print the response information for the current channel to
				standard output
	Usage:		void print_response ();
				print_response ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	current_station
				response_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_channel
	Calls to:	print_type53
				print_type54
				print_type55
				print_type56
				print_type58
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; response_head is the first
				member of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				12/12/88  Dennis O'Neill  added ".type53" etc to response->ptr
				          to make this work on a Sun 4/OS 4.0.
				03/25/99  Stephane Zuzlewski	Added support for blockette 62.
*/

#include "rdseed.h"

void print_response (response_head)
struct response *response_head;
{
	struct response *response;					/* looping vbl */

	for (response = response_head; response != NULL; response = response->next)
	{
		if (response->type == 'P') print_type53 (outputfile,response->ptr.type53);
		else if (response->type == 'C') print_type54 (outputfile,response->ptr.type54);
		else if (response->type == 'L') print_type55 (outputfile,response->ptr.type55);
		else if (response->type == 'G') print_type56 (outputfile,response->ptr.type56);
		else if (response->type == 'D') print_type57 (outputfile,response->ptr.type57);
		else if (response->type == 'S') print_type58 (outputfile,response->ptr.type58);
		else if (response->type == 'R') print_type60 (outputfile,response->ptr.type60);
		else if (response->type == 'F') print_type61 (outputfile,response->ptr.type61);
		else if (response->type == 'O') print_type62 (outputfile,response->ptr.type62);
		else 
		{
			fprintf (stderr, "WARNING [print_response]:  ");
			fprintf (stderr, "unknown response type %c encountered.\n", 
				response->type);
			fprintf (stderr, "\tExecution continuing.\n");
			fprintf(outputfile,"\tWARNING [print_response]:  ");
			fprintf(outputfile,"unknown response type %c encountered.\n\n",
				response->type);
		}
	}
}

