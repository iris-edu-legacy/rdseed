#include <stdio.h>
#include <rdseed.h>

/* ----------------------------------------------------------------------- 
 * Routines to handle ATC related functionality
 *
 */


/* -----------------------------------------------------------------------
 * Defines
 */
#define TRIM(s) {char *p; if ((p = strchr(s, ' ')) != NULL) *p = 0;}

#define SUBSTR(str,xch,ch)      \
        { char *c; \
          while (c = strchr(str, xch)) \
                *c = ch; \
        }

#define IS_BTW(a,y,z) \
        ((timecmp(a, y) >= 0) && (timecmp(a, z) <= 0))


#define append_linklist_element(new, head, tail) \
    new->next = NULL; \
    if (head != NULL) tail->next = new; \
    else head = new; \
    tail = new;

/* ---------------------------------------------------------------------- 
 * Variables
 */ 

struct atc_list {
	char net[3];
	char stn[6];
	char loc[3];
	char chn[4];
	char start[23];
	char end[23];
	float start_offset;
	float end_offset;
	char flag;
	char *comment;	
	struct atc_list *next;
} *atc_listhead, *atc_listtail;

static int atc_loaded = 0;

/* --------------------------------------------------------------------- */

int atc_add(char *atc_line)

{

	struct atc_list *atc;

	char **parts;
	int i, n;

	if (atc_line == (char *) NULL)
		return;

	if (strlen(atc_line) == 0)
		return;

	atc = (struct atc_list *)malloc(sizeof(struct atc_list));

        if (atc == (struct atc_list *)NULL)
        {
                fprintf(stderr, "Error ! atc_add(): out of memory\n");

                fprintf(stderr, "Unable to parse ATC data\n");

		return 0;
        }

	n = split(atc_line, &parts, '|');

	if (n < 8 || n > 10)
	{
		fprintf(stderr, "Error! atc_add(): Unable to parse ATC file\n");

		fprintf(stderr, "atc file line = %s\n", atc_line);

		fprintf(stderr, "stopped parsing field # %d\n", n);

		return 0;
	}
	
	TRIM(parts[0]);
	TRIM(parts[1]);
	TRIM(parts[2]);
	TRIM(parts[3]);
	TRIM(parts[4]);
	TRIM(parts[5]);
	TRIM(parts[6]);
	TRIM(parts[7]);

	memset(atc, 0, sizeof(struct atc_list));

	for (i = 0; i < n; i++)
		switch (i) 
		{

			case 0: 
				strncpy(atc->net, parts[0], sizeof(atc->net));
				break;
	
			case 1:
				strncpy(atc->stn, parts[1], sizeof(atc->stn));
				break;

			case 2:
				strncpy(atc->loc, parts[2], sizeof(atc->loc));
				break;

			case 3:
				strncpy(atc->chn, parts[3], sizeof(atc->chn));
				break;
			case 4:	
				strncpy(atc->start, parts[4], sizeof(atc->start));
				break;

			case 5:
	
				strncpy(atc->end, parts[5], sizeof(atc->end));
				break;

			case 6:
				atc->start_offset = atof(parts[6]);
				break;

			case 7:
				atc->end_offset = atof(parts[7]);
				break;
			
			case 8:
				atc->flag = *parts[8];
				break;

			case 9:
				atc->comment = (char *)malloc(strlen(parts[9]) + 1);
				if (atc->comment == (char *)NULL)
				{
					fprintf(stderr, "Warning! atc_add(): out of memory\n");
 
                			fprintf(stderr, "Unable to parse comment from ATC data\n");
				}
				else
					strcpy(atc->comment, parts[9]);
				
				break;

		}

	append_linklist_element(atc, atc_listhead, atc_listtail);

	fuse(&parts, n);
	
	atc_loaded = 1;

	return 1;

}

/* ----------------------------------------------------------------------- */
void atc_dump()

{

	struct atc_list *atc = atc_listhead;

	while (atc)
	{
		printf("%s,%s,%s,%s,%s,%s,%6.2f,%6.2f,%c,%s\n",
				atc->net, atc->stn, 
				atc->loc, atc->chn,
				atc->start, atc->end,
				atc->start_offset,
				atc->end_offset,
				atc->flag, 
				atc->comment?atc->comment:"");

		atc = atc->next;
	}

}

/* ----------------------------------------------------------------------- */



void atc_clear()

{
	struct atc_list *atc;

	for(atc = atc_listhead; 
		atc != (struct atc_list *)NULL; 
				atc = atc->next)
	{
		if (atc->comment != (char *)NULL)
			free(atc->comment);

		free(atc);
	}
			
}

/* ----------------------------------------------------------------------- */
struct atc_list *fetch(char *s, char *c, char *l, char *n, struct time *t)

{
	struct atc_list *atc;   

	struct time atc_start;
	struct time atc_end;

	atc = atc_listhead;

	TRIM(s);
	TRIM(c);
	TRIM(l);
	TRIM(n);


	while (atc != (struct atc_list *) NULL)
	{
		if (wstrcmp(s, atc->stn, strlen(s)) != 0)
		{
			atc = atc->next;
			continue;
		}

		if (wstrcmp(c, atc->chn, strlen(c)) != 0)
		{
			atc = atc->next;
			continue;
		}


		if (wstrcmp(n, atc->net, strlen(n)) != 0)
		{
			atc = atc->next;
			continue;
		}

		if (strcmp(l, "") != 0)
		if (wstrcmp(l, atc->loc, strlen(l)) != 0)
		{
			atc = atc->next;
			continue;
		}
	
		timecvt(&atc_start, atc->start);
		timecvt(&atc_end, atc->end);

		if (IS_BTW(*t, atc_start, atc_end))
			return atc;

		atc = atc->next;

	}
	

	return (struct atc_list *) NULL;

}	/* fetch */

/* ----------------------------------------------------------------------- */
void atc_correct(struct time *t, char *s, char *c, 
				char *n, char *l, float sample_rate)

{
	struct time t1;
	struct time t2;
	double diff;
	double m;

	struct atc_list *atc;

	atc = fetch(s, c, l, n, t);

	if (atc == NULL)
		return;

	timecvt(&t1, atc->start);
	timecvt(&t2, atc->end);
	
	diff = timedif(t1, t2);

	/* diff is in 10000th sec, end/start offsets are in seconds.
	 * convert to secs to 10000
	 */
	m = ((atc->end_offset - atc->start_offset) * 10000)/diff;

	/* m is the correction/fracsec */
	
	/* compute the number of fracsecs btw our time 
	 * and the start of correction 
	 */

	diff = timedif(t1, *t);

	*t = timeadd_double(*t, m * diff);

	return;
	
}

/* ----------------------------------------------------------------------- */

void atc_load(FILE *f)

{
	char buffer[8192];

	struct input_data_hdr *fsdh;

	struct time s_time, e_time;

	struct type74 *t74 = type74_head;

	int i, j, num_items, default_Lrecl;

	long int now_where;

        default_Lrecl = LRECL;

        while (t74 != NULL)
        {
	
		if (strcmp(t74->channel, "ATC") != 0)
		{
			t74 = t74->next;
			continue;
		}

		if (!chk_station(t74->station))
		{
			t74 = t74->next;
			continue;
		}

		if (!chk_network(t74->network_code))
		{
			t74 = t74->next;
			continue;
		}
		if (!chk_location(t74->location))
		{
			t74 = t74->next;
			continue;
		}
	
        	timecvt(&s_time, t74->starttime);
        	timecvt(&e_time, t74->endtime);

		if (!chk_time(s_time, e_time))
		{
			t74 = t74->next;
			continue;
		}

		now_where = ftello(f);

        	fseeko(f, ((t74->start_index-1)*LRECL) - now_where, 1);

        	/* we are assuming that the data record size will not
         	 * change for a timespan
         	 */
        	LRECL = get_blk_1000_Lrecl(f);

		for (j = 0; j < (t74->end_index-t74->start_index) + 1; j++)
        	{
                	for (i = 0; i < default_Lrecl/LRECL; i++)
                	{
                        	if (j == (t74->end_index-t74->start_index))
                                	if (i >= t74->end_subindex)
                                        	break;

				memset(buffer, 0, sizeof(buffer));

                        	num_items = fread(buffer, LRECL, 1, f);

                        	if (num_items != 1)
                        	{
                                	fprintf(stderr, "atc_load(): Unable to read the atc data\n");

                                	if (num_items < 0)
                                        	perror("rdseed");

                                	LRECL = default_Lrecl;
					return;
 
                        	}

				/* extract the \n terminated strings */
				/* cut to the chase */
				fsdh = (struct input_data_hdr *) &buffer[8];

				extract_atc_lines(&buffer[fsdh->bod]);
			}

		}
	
                t74 = t74->next;

		LRECL = default_Lrecl; 
        }

	 LRECL = default_Lrecl;

}

/* ----------------------------------------------------------------------- */
int extract_atc_lines(char *ptr)

{
	char **parts;

	int i, n;

	n = split(ptr, &parts, '\n');

	/* skip the first 5 lines */
	for (i = 5; i < n; i++)
		atc_add(parts[i]);

	fuse(&parts, n);

	return;

	
}

/* ----------------------------------------------------------------------- */






