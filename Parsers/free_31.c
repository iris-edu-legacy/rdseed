/*===========================================================================*/
/* SEED reader     |              free_type31              |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		free_type31
*/

#include "rdseed.h"

void free_type31 ()
{
	struct type31 *type31, *t;


	type31 = type31_head;
	while (type31 != NULL)
	{
		free(type31->class);
		free(type31->comment);

		t = type31->next;
		free((char *)type31);
		type31 = t;
	}
	type31_head = NULL;
}
