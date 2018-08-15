
/*===========================================================================*/
/* SEED reader     |               free_type72             |     time header */
/*===========================================================================*/
/*
	Name:		free_type72
*/

#include "rdseed.h"

void free_type72 ()
{
	struct type72 *type72, *t;

	type72 = type72_head;
	while (type72 != NULL)
	{	
		if (type72->station)
			free(type72->station);

		if (type72->location)
			free(type72->location);

		if (type72->channel)
			free(type72->channel);

		if (type72->arrival_time)
			free(type72->arrival_time);

		if (type72->phasename)
			free(type72->phasename);

		if (type72->network_code != NULL) 
			free(type72->network_code);

		t = type72->next;

		free((char *) type72);

		type72 = t;
	}
	type72_head = NULL;
}
