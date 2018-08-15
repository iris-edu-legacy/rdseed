/*===========================================================================*/
/* SEED reader     |             free_type70               |     time header */
/*===========================================================================*/
/*
	Name:		free_type70
*/

#include "rdseed.h"

void free_type70 ()
{
	struct type70 *type70, *t;

	type70 = type70_head;
	while (type70 != NULL)
	{ 
		if (type70->flag)
			free(type70->flag);

		if (type70->start_of_data)
			free(type70->start_of_data);

		if (type70->end_of_data != NULL) 
			free(type70->end_of_data);

		t = type70->next;

		free((char *)type70);

		type70 = t;

	}

	type70_head = NULL;
}
