/*===========================================================================*/
/* SEED reader     |               free_type52             |  station header */
/*===========================================================================*/
/*
	Name:		free_type52
*/

#include "rdseed.h"

void free_type52 (type52)
struct type52 *type52;
{
	struct type52 *t;


	while (type52 != NULL)
	{	
		if (type52->location)
			free(type52->location);

		if (type52->channel)
			free(type52->channel);

		if (type52->inst_comment != NULL) 
			free(type52->inst_comment);

		if (type52->channel_flag != NULL) 
			free(type52->channel_flag);

		if (type52->start)
			free(type52->start);

		if (type52->end != NULL) 
			free(type52->end);

		if (type52->update)
			free(type52->update);

		free_response(type52->response_head);
		free_type59(type52->type59_head);

		t = type52->next;

		free((char *) type52);

		type52 = t;
	}

}
