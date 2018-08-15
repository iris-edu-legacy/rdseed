/*===========================================================================*/
/* SEED reader     |             print_channel             |  station header */
/*===========================================================================*/
/*
	Name:		print_channel
	Purpose:	print all channel information for the current station to
				standard output
	Usage:		void print_channel ();
				print_channel ();
	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type52_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	print_stnh
	Calls to:	print_type52 - print a channel id
				print_response - print a response
				print_type59 - print a channel comment
	Algorithm:	print the contents of the structure to the standard output.
	Notes:		The structure is a linked list; type52_head is the first member
				of the list
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

void print_channel ()
{
	struct type52 *type52;

	for (type52 = current_station->type52_head; type52 != NULL; type52=type52->next)
	{
		current_channel = type52;

		print_type52 (type52);

		if (type52->response_head != NULL) 
			print_response (type52->response_head);

		if (type52->type59_head != NULL) 
			print_type59(type52->type59_head, 0);

	}
}
