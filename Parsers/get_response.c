/*===========================================================================*/
/* SEED reader     |             get_response              |  station header */
/*===========================================================================*/
/*
	Name:		get_response
	Purpose:	recover an instrument response from the input data, link it
				into the list for the current channel
	Usage:		struct response *get_response ();
				char type;
				struct response *response;
				response = get_response (type);
	Input:		type = a code letter indicating the type of response to recover
	Output:		response = a pointer to an instrument response
	Externals:	current_channel = the current channel description being built
	Warnings:	none
	Errors:		none
	Called by:	parse_type52
	Calls to:	none
	Algorithm:	allocate space for the response; link it into the list
	Notes:		none
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
				08/19/89  Dennis O'Neill  streamlined linked list construction

*/

#include "rdseed.h"

struct response *get_response (type)
char type;
{
	struct response *response;

	response = (struct response *)
		alloc_linklist_element (sizeof (struct response), "get_response");	
	
	response->type = type;

	append_linklist_element (response, current_channel->response_head,
		current_channel->response_tail);

	return (response);
}
