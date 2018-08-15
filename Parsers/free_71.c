
/*===========================================================================*/
/* SEED reader     |               free_type71             |     time header */
/*===========================================================================*/
/*
	Name:		free_type71
*/

#include "rdseed.h"

void free_type71 ()
{
	struct type71 *type71, *t; 
	int i;


	type71 = type71_head;
	while (type71 != NULL)
	{	
		free(type71->origin_time);

		for (i = 0; i < type71->number_magnitudes; i++)
		{
			if (type71->magnitude[i].type)
				free(type71->magnitude[i].type);
		}

		if (type71->magnitude)
			free((char *)type71->magnitude);

		if (type71->region_name != NULL) 
			free(type71->region_name);

		t = type71->next;

		free((char *)type71);

		type71 = t;

	}
	type71_head = NULL;
}
