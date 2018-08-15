/*===========================================================================*/
/* SEED reader     |            free_response             |  station header */
/*===========================================================================*/
/*
	Name:		free_response
*/

#include "rdseed.h"

void free_response (response_head)
struct response *response_head;
{
	struct response *response, *t;

	response = response_head;	
	while (response != NULL)
	{
		if (response->type == 'P')      free_type53 (response->ptr.type53);
		else if (response->type == 'C') free_type54 (response->ptr.type54);
		else if (response->type == 'L') free_type55 (response->ptr.type55);
		else if (response->type == 'G') free_type56 (response->ptr.type56);
		else if (response->type == 'D') free_type57 (response->ptr.type57);
		else if (response->type == 'S') free_type58 (response->ptr.type58);
		else if (response->type == 'R') free_type60 (response->ptr.type60);
		else if (response->type == 'F') free_type61 (response->ptr.type61);
		else if (response->type == 'O') free_type62 (response->ptr.type62);

		t = response->next;
		free((char *)response);
		response = t;
	}
}

