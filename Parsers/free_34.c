/*===========================================================================*/
/* SEED reader     |               free_type34               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		free_type34
*/

#include "rdseed.h"

void free_type34 ()
{
	struct type34 *type34, *t;

	type34 = type34_head;
	while (type34 != NULL)
	{
		free(type34->name);
		if (type34->description != NULL) free(type34->description);
		t = type34->next;
		free((char *)type34);
		type34 = t;
	}
	type34_head = NULL;
}
