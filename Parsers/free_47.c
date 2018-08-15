/*===========================================================================*/
/* SEED reader     |              free_type47              | station header */
/*===========================================================================*/
/*
	Name:		free_type47
*/

#include "rdseed.h"

void free_type47 ()
{
	struct type47 *type47, *t;

	type47 = type47_head;
	while (type47 != NULL)
	{
		if (type47->name)
			free(type47->name);

		t = type47->next;

		free((char *)type47);

		type47 = t;

	}

	type47_head = NULL;

}
