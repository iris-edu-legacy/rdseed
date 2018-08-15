/*===========================================================================*/
/* SEED reader     |              free_type74              |     time header */
/*===========================================================================*/
/*
	Name:		free_type74
*/

#include "rdseed.h"

void free_type74 ()
{
	struct type74 *type74, *t;
	int i;

	type74 = type74_head;
	while (type74 != NULL)
	{

		if (type74->station)
			free(type74->station);
		if (type74->location)	
			free(type74->location);

		if (type74->channel)
			free(type74->channel);

		if (type74->starttime != NULL) 
			free(type74->starttime);

		if (type74->endtime != NULL) 
			free(type74->endtime);

		for (i = 0; i < type74->number_accelerators; i++)
		{
			if (type74->accelerator[i].time != NULL) 
					free(type74->accelerator[i].time);
		}

		if (type74->accelerator)
			free((char *)(type74->accelerator));

		if (type74->network_code != NULL) 
			free(type74->network_code);

		t = type74->next;

		free((char *) type74);

		type74 = t;

	}

	type74_head = NULL;

}
