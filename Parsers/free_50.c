/*===========================================================================*/
/* SEED reader     |               free_type50              |  station header */
/*===========================================================================*/
/*
	Name:		free_type50
*/

#include "rdseed.h"

void free_type50 ()
{
	struct type50 *type50, *t;

	type50 = type50_head;
	while (type50 != NULL)
	{
		if (type50->station)
			free(type50->station);

		if (type50->name)
			free(type50->name);

		if (type50->start)
			free(type50->start);

		if (type50->end != NULL) 
			free(type50->end);

		if (type50->update)
			free(type50->update);

		if (type50->network_code != NULL) 
			free(type50->network_code);

		if (type50->type51_head != NULL) free_type51(type50->type51_head);
		if (type50->type52_head != NULL) free_type52(type50->type52_head);

		t = type50->next;

		free((char *) type50);

		type50 = t;
	}
	type50_head = NULL;

	current_station = NULL;
	current_channel = NULL;

}
