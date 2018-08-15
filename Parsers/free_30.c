/*===========================================================================*/
/* SEED reader     |              free_type30               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		free_type30
*/

#include "rdseed.h"

void free_type30 ()
{
	int i;
	struct type30 *type30, *t;

	type30 = type30_head;
	while (type30 != NULL)
	{
		free(type30->name);

		for (i = 0; i < type30->number_keys; i++)
			free((char *)(type30->decoder_key[i]));
		t = type30->next;
		free((char *)type30);

		type30 = t;
	}

	type30_head = NULL;
}
