/*===========================================================================*/
/* SEED reader     |             free_type73               |     time header */
/*===========================================================================*/
/*
	Name:		free_type73
*/

#include "rdseed.h"

void free_type73 ()
{
	struct type73 *type73, *t;
	int i;

	type73 = type73_head;
	while (type73 != NULL)
	{
		for (i = 0; i < type73->number_datapieces; i++)
		{
			if (type73->datapiece[i].station)
				free(type73->datapiece[i].station);

			if (type73->datapiece[i].location)
				free(type73->datapiece[i].location);

			if (type73->datapiece[i].channel)
				free(type73->datapiece[i].channel);

			if (type73->datapiece[i].time != NULL) 
				free(type73->datapiece[i].time);

		}

		if (type73->datapiece)
			free((char *)(type73->datapiece));

		t = type73->next;

		free((char *)type73);

		type73 = t;

	}
	type73_head = NULL;
}
