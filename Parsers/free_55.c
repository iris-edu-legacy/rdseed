
/*===========================================================================*/
/* SEED reader     |               free_type55             |  station header */
/*===========================================================================*/
/*
	Name:		free_type55
*/

#include "rdseed.h"

void free_type55 (type55)
struct type55 *type55;
{
	if (type55 == NULL)
		return;

	free_type55(type55->next);

	if (type55->response)
		free((char *)(type55->response)); 

	free((char *)type55);
}
