
/*===========================================================================*/
/* SEED reader     |               free_type56             |  station header */
/*===========================================================================*/
/*
	Name:		free_type56
*/

#include "rdseed.h"

void free_type56 (type56)
struct type56 *type56;
{
	if (type56 == NULL)
		return;

	free_type56(type56->next);

	if (type56->corner)
		free((char *)(type56->corner)); 

	free((char *)type56);

}
