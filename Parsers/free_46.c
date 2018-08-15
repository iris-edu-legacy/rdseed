/*===========================================================================*/
/* SEED reader     |             free_type46              |  station header */
/*===========================================================================*/
/*
	Name:		free_type46
*/

#include "rdseed.h"

void free_type46 ()
{
	struct type46 *type46, *t;

	type46 = type46_head;
	while (type46 != NULL)
	{
		if (type46->name)
			free(type46->name);

		if (type46->corner)
			free(type46->corner);

		t = type46->next;

		free((char *)type46);

		type46 = t;

	}
	type46_head = NULL;
}
