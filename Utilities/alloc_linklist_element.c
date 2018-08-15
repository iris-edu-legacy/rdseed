/*============================================================================*/
/*                 |        alloc_linklist_element          |       c utility */
/*============================================================================*/
/*
	Name:		alloc_linklist_element
	Purpose:	allocate space for a data element, regardless of the structure
				definition; allocate space for a linked list element
	Usage:		extern struct some_struct *linkedlist_head;
				extern struct some_struct *linkedlist_tail;
				struct some_struct *structure;
				int size_of_structure;
				char *calling_routine;
				size_of_structure = sizeof (some_struct);
				sprintf (calling_routine, "name of this routine");
				structure = (struct some_struct *)
					alloc_linklist_element (size_of_structure, calling_routine);
	Input:		size = size of memory piece to allocate (member of a linked
					list structure)
				caller = name of calling routine
	Output:		structure = a generic pointer to the new element in the linked
					list; see Notes
	Externals:	none
	Messages:	none
	Warnings:	none
	Errors:		none
	Fatals:		if unable to allocate memory, print message and terminate
				program
	Called by:	anything
	Calls to:	none
	Algorithm:	use malloc to allocate the appropriate amount of space
	Notes:		The use of linked lists is mostly a cookie-cutter operation.
				The material presented here allows one to do it routinely
				while minimizing typing and the chance of coding mistakes.

				Define the structure, initialize the linked list head and tail,
				and define a macro to link new elements into the list in the
				project header file or at the top (outside) the main function:
					struct some_struct
					{
						...
						struct some_struct *next;
					}
					struct some_struct *linkedlist_head = NULL;
					struct some_struct *linkedlist_tail = NULL;
					#define append_linklist_element(new, head, tail) \
    					if (head != NULL) tail->next = new; \
    					tail = new; \
    					if (head == NULL) head = tail;

				In the caller to this procedure, allocate space for a new
				element in the list with alloc_linklist_element.  This routine
				returns a generic pointer (here, a char *, which is typedefed
				to "caddr_t" in /usr/include/sys/types.h on Sun systems).  
				The programmer must cast the returned pointer to the proper
				type or struct as shown in the usage example.
					structure = (struct some_struct *)
						alloc_linklist_element (size_of_structure,
						calling_routine);

				After allocation, fill in the structure in the calling routine,
				link it to the list as in this example:
					append_linklist_element (structure, linkedlist_head,
						linkedlist_tail);

	Problems:	none known
	References:	none
	Author:		Dennis O'Neill
	Language:	C
	Revisions:	07/28/89  Dennis O'Neill  original version
*/
#include "rdseed.h"

#include <stdio.h>


char *alloc_linklist_element (size, caller)
int size;										/* number of byte to alloc */
char *caller;									/* name of calling proc */
{
	char *element;

	if ((element = (char *) malloc (size)) ==  NULL)
	{
		fprintf (stderr, "FATAL ERROR [alloc_linklist_element]:  ");
		fprintf (stderr, "unable to allocate %d bytes\n", size);
		fprintf (stderr, "\tfor a new element in a linked list.\n");
		fprintf (stderr, "\tCalled by routine %s.\n", caller);
		fprintf (stderr, "\tExecution terminating.\n");
		exit (1);
	}

	memset(element, 0, size);

	return (element);
}
