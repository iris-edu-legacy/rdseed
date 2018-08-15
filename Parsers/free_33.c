/*===========================================================================*/
/* SEED reader     |               free_type33               |   abbrev dctnry */
/*===========================================================================*/
/*
	Name:		free_type33
*/

#include "rdseed.h"

void free_type33 ()
{
	struct type33 *type33, *t;

	type33 = type33_head;
	while (type33 != NULL)
	{
		free(type33->abbreviation);
		t = type33->next;
		free((char *)type33);
		type33 = t;
	}
	type33_head = NULL;
}
