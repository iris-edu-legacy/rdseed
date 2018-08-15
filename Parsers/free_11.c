/*===========================================================================*/
/* SEED reader     |             free_type11               |   volume header */
/*===========================================================================*/
/*
	Name:		free_type11
*/
#include "rdseed.h"

extern struct type11 *type11_tail;

void free_type11 ()
{
	int i;
	

	struct type11 *type11, *t;

        type11 = type11_head;
        while (type11 != NULL)
        {

		for (i = 0; i < type11->number_stations; i++)
		{
			free(type11->station[i].station_id);

			type11->station[i].station_id = NULL;
		}

		if (type11->station != NULL) 
			free((char *)type11->station);

		t = type11->next;

		free(type11);

		type11 = t;

	}

	type11_head = NULL;
	type11_tail = NULL;


}
void dump11()
{
	int i;
	struct type11 *type11, *t;

        type11 = type11_head;

printf("type11head=%p\n", type11_head);

        while (type11 != NULL)
        {

		for (i = 0; i < type11->number_stations; i++)
		{
printf("station_id=%p, %s\n", type11->station[i].station_id, type11->station[i].station_id);
		}

		type11 = type11->next;

	}

}

