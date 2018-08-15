
/*===========================================================================*/
/* SEED reader     |            free_type35               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		free_type35
*/

#include "rdseed.h"

void free_type35 ()
{
	struct type35 *type35, *t;
	int i;

	type35 = type35_head;

	while (type35 != NULL)
	{
		for (i = 0; i < type35->number_beams; i++)
		{
			free(type35->beam[i].station);
			free(type35->beam[i].location);
			free(type35->beam[i].channel);
		}

		if (type35->beam != 0)
			free((char *)(type35->beam));

		t = type35->next;

		free((char *)type35);

		type35 = t;

	}
	type35_head = NULL;

}
