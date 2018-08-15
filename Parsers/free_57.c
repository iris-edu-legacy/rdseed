
/*===========================================================================*/
/* SEED reader     |            free_type57                |  station header */
/*===========================================================================*/
/*
	Name:		free_type57
*/

#include "rdseed.h"

void free_type57 (type57)
struct type57 *type57;
{

	if (type57 == NULL)
		return;

	free_type57(type57->next);

	free((char *)type57);

}
