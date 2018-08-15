
/*===========================================================================*/
/* SEED reader     |               free_type32              |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		free_type32
*/

#include "rdseed.h"

void free_type32 ()
{
	struct type32 *type32, *t;

	type32 = type32_head;
	while (type32 != NULL)
	{
		free(type32->author);
		free(type32->catalog);
		free(type32->publisher);

		t = type32->next;
		free((char *)type32);
		type32 = t;
	}

	type32_head = NULL;

}
