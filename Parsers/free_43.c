/*===========================================================================*/
/* SEED reader     |              free_type43              |  station header */
/*===========================================================================*/
/*
	Name:		free_type43
*/

#include "rdseed.h"

void free_type43 ()
{
	struct type43 *type43, *t;

	type43 = type43_head;
	while (type43 != NULL)
	{
		if (type43->name != 0)
			free(type43->name);

		if (type43->zero != 0)
			free(type43->zero);

		if (type43->pole != 0)
			free(type43->pole);

		t = type43->next;

		free((char *)type43);

		type43 = t;

	}
	type43_head = NULL;
}
