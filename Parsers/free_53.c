/*===========================================================================*/
/* SEED reader     |               free_type53             |  station header */
/*===========================================================================*/
/*
	Name:		free_type53
*/

#include "rdseed.h"

void free_type53 (type53)
struct type53 *type53;
{
	if (type53 == NULL)
		return;

	free_type53(type53->next);

	if (type53->transfer)
		free(type53->transfer);

	if (type53->zero)
		free((char *)(type53->zero));

	if (type53->pole)
		free((char *)(type53->pole));

	free((char *) type53);

	return;
}
