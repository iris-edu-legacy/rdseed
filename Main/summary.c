#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#include "rdseed.h"

#define SUBSTR(str,xch,ch)      \
        { char *c; \
          while (c = strchr(str, xch)) \
                *c = ch; \
        }

#define IS_BTW(x,a,b) \
        ((timecmp(x, a) >= 0) && (timecmp(x, b) <= 0))

 
#define append_linklist_element(new, head, tail) \
    new->next = NULL; \
    if (head != NULL) tail->next = new; \
    else head = new; \
    tail = new;

struct twindow 
{
	char time_start[23]; 
        char time_end[23];        
	int already_used;

	char chn_list[1024];
	char loc_list[1024];

	struct twindow *next;
};


struct stn_tspan
{
	char net[5];
	char stn[10];

	char *this_event;
	
	struct twindow *ts_head;
	struct twindow *ts_tail;

	struct stn_tspan *next;	

} *stn_listhead = NULL, *stn_listtail = NULL;

struct tspan_list
{
	char time_start[23];
        char time_end[23];

	int start_seq_num;
	int end_seq_num;

	int already_used;

	struct tspan_list *next;

} *tspans_listhead;

static char *this_event = "";

char *add_time();
void dump_stn_nodes();
void dump_twindows();
struct type74 *find_closest_b74();

extern float WeedVersion;
extern int output;

float weed_version = 3.0;

float summary_file_get_version();

read_summary_file(fname)
char *fname;

{
	FILE *sumfile;

	char buffer[512];
	char this_ev[512+1], *ch;
	char *stn_ev;
	char token[30];
	int  bias_start;
	int  bias_end;

	/* ### warning  Must be big enough for indeterminate channel count */
	char chn[3000]; 
	char loc[3000];

	char time_win_start[23];
	char time_win_end[23];

	char tmp[23];
	
	struct stn_tspan *stn_ptr;
	struct twindow *twindow_ptr;

	weed_version = 3.0;

	strcpy(chn, "");
	strcpy(loc, "");

	sumfile = fopen(fname, "r");

	if (sumfile == NULL)
	{
		fprintf(stderr, "Unable to open summary file: %s!\n", fname);
		perror("read_summary_file");
		return(0);
	}

	/* read in the first token */
	while (fscanf(sumfile, "%s", token) != EOF)
	{

		/* scan first tokens, WEED_ID, EVENT, STATION, or PHASE */
		if (strcmp(token, "") == 0)
			continue;

		if (strstr(token, "Weed_version"))
		{
			fgets(buffer, 512, sumfile);

			weed_version = atof(buffer);
		}

		if (strstr(token, "EVENT"))
		{
			/* save Event line */
			fgets(buffer, 512, sumfile);

			/* erase \n at end */
			buffer[strlen(buffer) - 1] = '\0';

			/* save it for later */
			strcpy(this_ev, buffer);
	
			continue;

		}

		if (strstr(token, "STATION")) 
                {
			stn_ptr = (struct stn_tspan *)
					malloc(sizeof(struct stn_tspan));

			        
                        if (stn_ptr == NULL)   
                        {
                                fprintf(stderr, "Error : unable to obtain memory for station from summary file!\n"); 
                                fprintf(stderr, "Quitting\n");
 
                                exit(-1);
 
                        }
        
			memset((char *)stn_ptr, 0, sizeof(struct stn_tspan));

			fscanf(sumfile, 
				"%s %s", 
				stn_ptr->stn, stn_ptr->net);

			stn_ev = malloc(strlen(this_ev)+1);

			if (stn_ev == NULL)
			{
				fprintf(stderr, "Error : unable to obtain memory for event from summary file!\n");
				fprintf(stderr, "Quitting\n");

				exit(-1);

			}

			strcpy(stn_ev, this_ev);
			stn_ptr->this_event = stn_ev;

			append_linklist_element(stn_ptr, 
						stn_listhead, stn_listtail);
			add_stn(stn_ptr->stn);

			fgets(buffer, 512, sumfile);

			continue;
                }

		if (strstr(token, "PHASE")) 
                {
			char *ch_ptr;

			twindow_ptr = (struct twindow *)
                                        malloc(sizeof(struct twindow));

			memset((char *)twindow_ptr, 0, sizeof(struct twindow));

			if (weed_version >= 2.7)
				fscanf(sumfile, 
					"%*s %d %*s %d %s %s %s %s %*s\n",
					&bias_start,
					&bias_end,
					chn,
					loc,
					time_win_start,
					time_win_end);
			else
				fscanf(sumfile, 
                                        "%*s %d %*s %d %s %s %s %*s\n",
                                        &bias_start,
                                        &bias_end, 
                                        chn,
                                        time_win_start,
                                        time_win_end);   

			//  replace hyphens with spaces
			SUBSTR(loc, '-', ' ');

			/* strip out the quotes in the channel list */
			strncpy(twindow_ptr->chn_list, 
				&chn[1], /* start quote */
				strlen(chn) > 511 ? 511 : strlen(chn) - 2);

			/* now the ending quote */
			twindow_ptr->chn_list[strlen(chn) > 511 ? 
						511 : strlen(chn) - 2] = 0;

			if (weed_version > 1.0)
			{
				/* strip out the quotes in the location list */
                        	strncpy(twindow_ptr->loc_list,
                                	&loc[1], /* start quote */
                                	strlen(loc) > 1023 ? 1023 : strlen(loc) - 2);
 
                        	/* now the ending quote */
                        	twindow_ptr->loc_list[strlen(loc) > 1023 ?
                                                	1023 : strlen(loc) - 2] = 0;
			}
			else
				strcpy(twindow_ptr->loc_list, "*");

			add_chn(twindow_ptr->chn_list);

			add_loc(twindow_ptr->loc_list);

			/* WEED version -1.5 and greater allows the user
			 * to enter negative or positive values here.
			 * If < 2.0 assume subtraction only, make into 
			 * negitive
			 */

			add_time(time_win_start, tmp,
                                          WeedVersion >= 1.5 ?
                                                        bias_start :
                                                        -bias_start);

			strcpy(twindow_ptr->time_start, tmp);

			add_time(time_win_end, tmp,
                                          WeedVersion >= 1.5 ? 
                                                        bias_end: 
                                                        -bias_end); 
 
			strcpy(twindow_ptr->time_end, tmp);

			append_linklist_element(twindow_ptr,
                                                stn_ptr->ts_head, 
						stn_ptr->ts_tail);

			continue;

                }
	}		/* while fscanf */

	fclose(sumfile);

	/* must parse channel_list as built in add_chn(), parse
	 * into channel_point, all globals, used by parsers()
	 */
	channel_count = 0;

	ch = strtok(channel_list, ",");

	if (ch != NULL)	
		do 
		{
			channel_point[channel_count] = ch;
			channel_count++;

		} while ((ch = strtok(NULL, ",")) != NULL);

	location_count = 0;

        ch = strtok(location_list, ",");

        if (ch != NULL)
                do
                {
                        location_point[location_count] = ch;
                        location_count++;
 
                } while ((ch = strtok(NULL, ",")) != NULL);

	// dump_stn_nodes(stn_listhead);


	return 1;
	
}

/* ------------------------------------------------------------------- */

char *strptime();

char *ddd2mmdd();


/* --------------------------------------------------------------------- */
void dump_stn_nodes(stn_node)
struct stn_tspan *stn_node;

{
	while (stn_node)
	{
		printf("%s / %s:%s\n", stn_node->net, stn_node->stn, stn_node->this_event);

		dump_twindows(stn_node->ts_head);

		stn_node = stn_node->next;
	}

}

/* -------------------------------------------------------------------- */
int add_stn(stn)
char *stn;

{
	int i;
	int found = FALSE;

	for (i = 0; i < station_count; i++)
		if (strcmp(station_point[i], stn) == 0)
			found = TRUE;

	if (!found)
	{
		station_point[i] = stn;
		station_count++;

	}

	return;

}

/* -------------------------------------------------------------------- */
/* add_chn() and add_loc() simply add the chn and locs to a global string
 * making sure that no duplicates exists. Then when read_summary_file()
 * exits it loads these lists into the global variables.
 */

int add_chn(chn)
char *chn;

{
	int i;
        int found = FALSE;
	char *ch;

	char *ch_ptr;
	char channels[512];

	/* protect original string - also channel_list is global */
	strncpy(channels, chn, strlen(chn));
	channels[strlen(chn)] = 0;

	/* the time window channel string could be comma delimited */
	ch = strtok(channels, ",");

	while(ch != NULL)
	{
        	if (strstr(channel_list, ch) == 0)
        	{
			strcat(channel_list, ch);
			strcat(channel_list, ",");
        	}

		ch = strtok(NULL, ",");
	}

        return;

}

/* ------------------------------------------------------------------ */

int add_loc(loc)
char *loc;

{
	int i;
        int found = FALSE;
	char *ch;

	char *ch_ptr;
	char locations[1024];

	/* protect original string - also location_list is global */
	strncpy(locations, loc, strlen(loc));
	locations[strlen(loc)] = 0;

	SUBSTR(locations, '-', ' ');

	SUBSTR(locations, ':', ',');

	/* the time window channel string could be comma delimited */
	ch = strtok(locations, ",");

	while(ch != NULL)
	{
        	if (strstr(location_list, ch) == 0)
        	{
			strcat(location_list, ch);
			strcat(location_list, ",");

        	}

		ch = strtok(NULL, ",");

	}

	/* replace dashes with spaces */
	// SUBSTR(location_list, '-', ' ');

        return;

}
/* --------------------------------------------------------------------- */
void dump_twindows(twin)
struct twindow *twin;

{
        char time_start[23];
        char time_end[23];

        char chn_list[100];

	while (twin)
	{

		printf("\t%s <> %s - %s\n", 
				twin->time_start, twin->time_end,
				twin->chn_list);

		twin = twin->next;
	}

	return;
}

/* ------------------------------------------------------------------------- */
int free_stn_nodes()
{
	free_stn(stn_listhead);	

	stn_listhead = NULL;
	stn_listtail = NULL;

	return;
}

/* ------------------------------------------------------------------------- */
/* multiple stations point at a single event */
int free_stn(node)
struct stn_tspan *node;

{
	if (node == NULL)
	{
		return;
	}
	else
	{
		free_stn(node->next);

		free_tspans(node->ts_head);

		node->ts_head = NULL;
		node->ts_tail = NULL;
		free(node->this_event);
		free(node);

		node = NULL;

	}

	return;


}

/* ------------------------------------------------------------------------- */
int free_tspans(ts_node)
struct twindow *ts_node;

{
        if (ts_node == NULL)
        {
                return;
        }
        else
        {   
                free_tspans(ts_node->next);

                free(ts_node);

		ts_node = NULL;
        }

        return;


}

/* ------------------------------------------------------------------------- */
	

char *add_time(time_asc, tmp, incr)
char *time_asc;		/* YYYY,DDD,HH:MM:SS.FFFF */
char *tmp;
int incr;

{
	struct time tm, newtime;

	char YYYYDDDetc[30];

	/*memset((char *)&tm, 0, sizeof(struct tm));*/
	memset((char *)&tm, 0, sizeof(struct time));

	SUBSTR(time_asc, ',', ' ');

	SUBSTR(time_asc, ':', ' ');

	SUBSTR(time_asc, '.', ' ');

	sscanf(time_asc, "%d %d %d %d %d %d", 
				&tm.year, &tm.day,
				&tm.hour, &tm.minute, &tm.second,
				&tm.fracsec);

	if (incr < 0)
		newtime = timesub(tm, (-incr) * 10000);
	else
		newtime = timeadd(tm, incr * 10000);

	/*sprintf(YYYYDDDetc, "%d,%03d,%02d:%02d:%02d.%d", */
	sprintf(YYYYDDDetc, "%04d,%03d,%02d:%02d:%02d.%04d",
		newtime.year, 
		newtime.day,
		newtime.hour, newtime.minute, 
		newtime.second, newtime.fracsec);

	strcpy(tmp, YYYYDDDetc);

	return tmp;		
}

/* ---------------------------------------------------------------------- */

#define isaleap(year) ((((year)%100 != 0) && ((year)%4 == 0)) || ((year)%400 == 0))


/* ----------------------------------------------------------------------- */
static int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 
				31, 31, 30, 31, 30, 31};

char *ddd2mmdd(ddd, yyyy)
int ddd;
int yyyy;

{

	static char mmddyyyy[200];
	
	int mon;

	for (mon = 1; mon < 13; mon++)
	{
		if (ddd <= days_in_month[mon])
			break;

		ddd -= days_in_month[mon];

		/* adjust for leap year */

		if (isaleap(yyyy) && (mon == 2))
			ddd--;

	}

	if (mon == 13)	
	{
		fprintf(stderr, "Bad day number!\n");
		return;
	}


	sprintf(mmddyyyy, "%d/%d/%d", mon, ddd, yyyy);	

	return mmddyyyy;
}

/* ------------------------------------------------------------------------- */
void set_event(struct time *blk_start, struct time *blk_end)

{

	struct tm tm;
	time_t t;

	struct time ev_start;

	char *ch;
	char **c_ptr;
	char **parts;
	char **pieces;
	char **bitty_pieces;
	int n, nn, nnn, nnnn;

	double diff_start, diff_end, this_diff;

	float smallest = -1;

	struct stn_tspan *stn_ptr;
	struct stn_tspan *sav_stn_ptr;

	float weed_version;

	weed_version = summary_file_get_version();

	char ev_time[200];

	stn_ptr = stn_listhead;

	while (stn_ptr)
        {
		// extract the yyyy/mm/dd to make into yyyyddd format

		trim(stn_ptr->this_event);

		n = split(stn_ptr->this_event, &c_ptr, ',');

		if (weed_version < 4.0)
			nn = split(c_ptr[1], &parts, ' ');
		else
			nn = split(c_ptr[0], &parts, ' ');

		if (weed_version < 4.0)
			nnn = split(parts[0], &pieces, '/');
		else
			nnn = split(parts[0], &pieces, '-');

		nnnn = split(parts[1], &bitty_pieces, ':');

		// lop off the frac part if there

		if ((ch = strchr(bitty_pieces[2], '.')) != NULL)
			*ch = '\0';

		tm.tm_year = atoi(pieces[0]) - 1900;		// see man ctime

		tm.tm_mon = atoi(pieces[1]) - 1;	// 0..11

		tm.tm_mday = atoi(pieces[2]);

		tm.tm_hour = atoi(bitty_pieces[0]);

		tm.tm_min = atoi(bitty_pieces[1]);

		tm.tm_sec = atoi(bitty_pieces[2]);

		tm.tm_isdst = -1;

		t = mktime(&tm);

		sprintf(ev_time, "%04d,%03d,%02d,%02d,%02d", tm.tm_year + 1900, tm.tm_yday + 1, tm.tm_hour, tm.tm_min, tm.tm_sec);

       		timecvt(&ev_start, ev_time);

		diff_start = timedif(*blk_start, ev_start);
                diff_end = timedif(*blk_end, ev_start);
                
		this_diff = fabsl(diff_start) + fabsl(diff_end);

		if (smallest < 0)  	// flagged for first
		{
			smallest = this_diff;
			sav_stn_ptr = stn_ptr;
		}
	
		else	
                if (this_diff < smallest)
		{
			smallest = this_diff;

			sav_stn_ptr = stn_ptr;
		}

		stn_ptr = stn_ptr->next;

	}

	this_event = sav_stn_ptr->this_event;

}

/* ------------------------------------------------------------------------- */


struct twindow *scan_summary_file(stn, chn, net, loc, blk_start, blk_end)
char *stn, *chn, *net, *loc;
struct time *blk_start;
struct time *blk_end;

{
        char **c_ptr, **l_ptr;
	struct stn_tspan *stn_ptr;

	struct time req_start;
	struct time req_end;

	int found_chn;
	
        int i, n, nn;

        stn_ptr = stn_listhead;
 
        while (stn_ptr)
        {
		found_chn = FALSE;

               	if ((strcmp(stn_ptr->stn, stn) != 0)) 
		{
			stn_ptr = stn_ptr->next;
			continue;
		}

		if (strcmp(stn_ptr->net, "??") == 0 ? 0 : 
                        (strcmp(stn_ptr->net, net) != 0))
		{
			stn_ptr = stn_ptr->next;
                        continue;
		}


		/* check phase lines, comparing channel */

		rdseed_strupr(stn_ptr->ts_head->chn_list);
 
		n = split(stn_ptr->ts_head->chn_list, &c_ptr, ',');
 
                for (i = 0; i < n; i++)
		{
			if (wstrcmp(chn, c_ptr[i], 3) == 0)
			{
				nn = split(stn_ptr->ts_head->loc_list, &l_ptr, ',');

				/* nn and n should be equal */
				if (nn != n)
				{
					fprintf(stderr, "Warning, unable to parse location codes in the summary file. \n");
					found_chn = TRUE;
				}
				else
				{
					if (wstrcmp(loc, l_ptr[i], 2))
						found_chn = TRUE;
				}

				fuse(&l_ptr, nn);

				found_chn = TRUE;

			}
	
		}

		fuse(&c_ptr, n);

		if (!found_chn)
		{
			stn_ptr = stn_ptr->next;
			continue;
		}		

               	timecvt(&req_start, stn_ptr->ts_head->time_start);
                timecvt(&req_end, stn_ptr->ts_head->time_end);

		/* at last - compare times */
		
		if (!((IS_BTW(req_start, *blk_start, *blk_end))   ||
			(IS_BTW(req_end, *blk_start, *blk_end))  ||
			(IS_BTW(*blk_start, req_start, req_end))  ||
			(IS_BTW(*blk_end, req_start, req_end))))
                {
			stn_ptr = stn_ptr->next;
                        continue;
                }
	
		/* got here - everything checks out */

		return stn_ptr->ts_head;	
	}

	return 0;	/* didn't find it */
	
}

/* ----------------------------------------------------------------- */

int reset_chn_name_generator();

struct type74 *extract_74s();

static struct stn_tspan *stn_ptr;
static struct stn_tspan *now_stn;


/* ----------------------------------------------------------------- */

int process_summary_file()

{
        struct twindow *twin, *tw;
	
	struct type74 *b74_ptr;
 
	char *chn_ptr, **c_ptr;
	char **l_ptr;

	char channel[4];

	int i, n, nn;

	stn_ptr = stn_listhead;

	while (stn_ptr)	
	{

		/* 1st check to see if stn, net is in the contents */
		if (!scan_74s(stn_ptr->stn, stn_ptr->net))
		{
			stn_ptr = stn_ptr->next;
			continue;
		}

		/* make a check for non-null ts_head */
		if (stn_ptr->ts_head == NULL)
		{
			stn_ptr = stn_ptr->next;
                        continue;
		}

		n = split(stn_ptr->ts_head->chn_list, &c_ptr, ',');
		nn = split(stn_ptr->ts_head->loc_list, &l_ptr, ',');

		for (i = 0; i < n; i++)
		{

			b74_ptr = type74_head;
			while (b74_ptr != NULL)
			{

				/* Notice: only one twindow per stn node, no linked
		 	 	 * list for now, each time window gets a separate node
		 	  	 */

				b74_ptr = extract_74s(b74_ptr, 
							stn_ptr->stn,
						      	stn_ptr->net,
					   		c_ptr[i],
							nn == 0 ? 
							"" : 
							nn > i ? l_ptr[i] : l_ptr[nn-1],
					   		stn_ptr->ts_head->time_start, 
					   		stn_ptr->ts_head->time_end);

				if (b74_ptr != NULL)
				{
					this_event = stn_ptr->this_event;

					/* save this stn's summary file line for later 
					 * comparisons.
					 * See chk_summary()
					 */

					now_stn = stn_ptr;

					extract_this_timespan(b74_ptr);

					// time_span_out();

					/* funky process_data() flag */
					output = 0;

					last_time.year = 0;
					last_time.day = 0;
					last_time.hour = 0;
					last_time.minute = 0;
					last_time.second = 0;
					last_time.fracsec = 0;

					b74_ptr = b74_ptr->next;
				}

			}  /* while chn_ptr */

		}		/* for */

		fuse(&c_ptr, n);
		fuse(&l_ptr, nn);

		stn_ptr = stn_ptr->next;
	}

	time_span_out();

}

/* ----------------------------------------------------------------- */
int chk_summary(blk_start, blk_end) 
struct time blk_start;
struct time blk_end;

{
	struct time this_stn_start;
	struct time this_stn_end;

	timecvt(&this_stn_start, now_stn->ts_head->time_start);
	timecvt(&this_stn_end, now_stn->ts_head->time_end);

	return (timecmp(blk_start, this_stn_end) <= 0) &&
		(timecmp(blk_end, this_stn_start) >= 0);

}

/* ----------------------------------------------------------------- */

int scan_list(struct type74 *this_74, char *r_start, char *r_end)

{

	/* this_74 holds a valid timespan as determined
	 * from initial scan from blockette 74s. Now 
	 * rescan looking for blk 74s which lie outside
	 * this_74s params, but within the sumfile request
	 */

	struct type74 *nother_74;

        struct time req_start, tspan_start;
        struct time req_end, tspan_end;

 
	nother_74 = this_74->next;

	if (nother_74 == NULL)
		nother_74 = type74_head; /* start at head of list */

	while (nother_74 != this_74)
	{
		if (chk_s_n_c_l(this_74->station,
				 this_74->network_code,
				 this_74->channel,
				 this_74->location,
				 nother_74))
		{
			/* see of times fall in btw request */

                	/* 1st check for overlap */
 
                	timecvt(&req_start, r_start);
                	timecvt(&tspan_start, nother_74->starttime);
 
         
                	timecvt(&req_end, r_end);
                	timecvt(&tspan_end, nother_74->endtime);
 
                	if (!((IS_BTW(req_start, tspan_start, tspan_end))   ||
                        	(IS_BTW(req_end, tspan_start, tspan_end))  ||
                        	(IS_BTW(tspan_start, req_start, req_end))  ||
                        	(IS_BTW(tspan_end, req_start, req_end))))
                	{
		
				nother_74 = nother_74->next;
	
				if (nother_74 == NULL)
					nother_74 = type74_head;

                        	continue;
                	}

			/* if it falls btw request span, check to
			 * see that at least part of it lies outside
			 * the already extracted blockette 74
			 * but not totally within it. Redundant 
			 */

			/* req_start and req_end are handy variable */
			timecvt(&req_start, this_74->starttime);
			timecvt(&req_end, this_74->endtime);

			if ((IS_BTW(req_start, tspan_start, tspan_end))   ||
                                (IS_BTW(req_end, tspan_start, tspan_end)) && 
                                !((timecmp(tspan_start, req_start) >= 0)  && 
                                (timecmp(tspan_end, req_end) < 0)))
                        {

				this_event = stn_ptr->this_event;

				extract_this_timespan(nother_74);
				time_span_out();

				/* funky process_data() flag */
                               output = 0;
                        }
			
		}	 
	
		nother_74 = nother_74->next;
	
		if (nother_74 == NULL)
			nother_74 = type74_head; /* start at head of list */

	}

}

/* ----------------------------------------------------------------- */
struct twindow *get_twin_from_summary()

{
        return stn_ptr->ts_head;
}

/* ----------------------------------------------------------------- */

/* ----------------------------------------------------------------- */
struct type74 *extract_74s(type74, stn, net, chn, locs, start, end)
struct type74 *type74;
char *stn, *net, *chn, *locs, *start, *end;

{

/*
 *	We are looking at these possibilities
 *
 *	Request from summary file totally lies within b74 start, end
 *		stop here as we found it
 *
 *	Search again for a B74 that overlaps it, or a B74 which lies
 *	totally within the requested timespan. Determine which is
 *      closer.
 */
	int found_times, found_stn;

	struct type74 *sav_type74 = type74;
	found_times = found_stn = FALSE;

	while (type74 != NULL)
	{

		if (!chk_s_n_c_l(stn, net, chn, locs, type74))
		{
			type74 = type74->next;
			continue;
		}
		

		if (chk_type74(type74, start, end))
			return type74;

                type74 = type74->next;

        }

#if 0

	/* if encompassing 74 not found, look for best fit */

	if (!found_times && found_stn)
	{
		type74 = find_closest_b74(net, 
					  stn, 
					  chn, 
					  locs,
					  start, 
					  end, sav_type74);

		if (type74 == NULL)
		{
			fprintf(stderr, "Warning : scan_74s() - unable to locate matching blockette 74 for summary file requested entry\n");

			fprintf(stderr, "Request: station/chn/net %s/%s/%s for :%s..to..%s\n", 
						stn, 
						chn, 
						net, 
						start, 
						end);

		}
	}

#endif

	return type74;
	
}
/* ----------------------------------------------------------------- */
int chk_type74(struct type74 *type74, char *start, char *end)

{
	struct time req_start, tspan_start;
        struct time req_end, tspan_end;

	timecvt(&req_start, start);
	timecvt(&tspan_start, type74->starttime);
         
	timecvt(&req_end, end);
	timecvt(&tspan_end, type74->endtime);

	return ((timecmp(tspan_start, req_end) <= 0) &&
		(timecmp(tspan_end, req_start) >= 0));
}

/* ----------------------------------------------------------------- */

struct type74 *find_closest_b74(net, stn, chn_list, locs, start, end, type74)
char *net;
char *stn;
char *chn_list;
char *locs;
char *start, *end;
struct type74 *type74;

{
	struct type74 *save_type74 = NULL;

	struct time req_start, tspan_start;
	struct time req_end, tspan_end;

	float diff_start, diff_end, this_diff;

	float smallest = 999999999;

	while (type74 != NULL)
	{
		if (!chk_s_n_c_l(stn, net, chn_list, locs, type74))
		{
			type74 = type74->next;
			continue;
		}

		/* check B74's start end time pairs which
		 * come closest to the request start/end times
                 */

		/* strategy is to add up the differences of the
		 * corresponding endpoints, smallest wins
		 */

		/* 1st check for overlap */

		timecvt(&req_start, start);
		timecvt(&tspan_start, type74->starttime);

	
		timecvt(&req_end, end);
                timecvt(&tspan_end, type74->endtime);

		if (!((IS_BTW(req_start, tspan_start, tspan_end))   ||
			(IS_BTW(req_end, tspan_start, tspan_end))  ||
			(IS_BTW(tspan_start, req_start, req_end))  || 
                        (IS_BTW(tspan_end, req_start, req_end))))
		{
			type74 = type74->next;
			continue;
		}
		     
		diff_start = timedif(req_start, tspan_start);
                diff_end = timedif(req_end, tspan_end);
                
		this_diff = fabs(diff_start) + fabs(diff_end);
 
                if (this_diff < smallest)
		{
			smallest = this_diff;

			save_type74 = type74;

		}

		type74 = type74->next;
 
	}

	return save_type74;

}

int chk_s_n_c_l(s, n, c, l, type74)
char *s, *n, *c, *l; 
struct type74 *type74;

{

	if (strcmp(s, type74->station) != 0)
		return 0;

	if (strcmp(n, "??") == 0 ? 0 :
		   (strcmp(n, type74->network_code) != 0))
	{
		return 0;

	}


	if (chk_chn_list(c, type74->channel) == 0)
		return 0;

	if (!chk_locs(l, type74->location))
		return 0;

	return 1;
}


/* ----------------------------------------------------------------- */
int scan_74s(stn, net)
char *stn, *net;

{
        struct type74 *type74;

        type74 = type74_head;

        while (type74 != NULL)
        {
                if (strcmp(stn, type74->station) != 0)
                {
                        type74 = type74->next;
                        continue;
                }

                if (strcmp(net, "??") == 0 ? 0 :
                        (strcmp(net, type74->network_code) != 0))
                {

                        type74 = type74->next;
                        continue;
 
                }

		return 1;
 
	} 	

	return 0;

}

/* ----------------------------------------------------------------- */
/* check the start/end pair to see if it falls btw the summary file
 * station pointer's start stop end pair
 *
 * stn_ptr is static and points to the station currently being processed
 */

int chk_sumfile_times(this_start, this_end)
struct time this_start;
struct time this_end;

{
        struct time req_start, req_end;

	struct twindow *twin = stn_ptr->ts_head;

	

        while (twin)
	{
        
		timecvt(&req_start, twin->time_start);
        	timecvt(&req_end, twin->time_end);

		/* check for overlap of the B74 over the requested start/stop */

		if ((timecmp(this_start, req_end) <= 0) && 
			(timecmp(this_end, req_start) >= 0))
			return 1;

		twin = twin->next;
	}
	
	return 0;
}

/* ----------------------------------------------------------------------- */
int chk_chn_list(chn_list, channel)
char *chn_list;
char *channel;

{
	char **ch_ptr;
	int i, n;

	rdseed_strupr(chn_list);
	rdseed_strupr(channel);

	n = split(chn_list, &ch_ptr, ',');

	for (i = 0; i < n; i++)
		if (wstrcmp(channel, ch_ptr[i], 3) == 0)
		{
			fuse(&ch_ptr, n);
			return 1;
		}
	
	fuse(&ch_ptr, n);

	return 0;
	
}

/* ----------------------------------------------------------------------- */
int chk_locs(l, location)
char *l;
char *location;

{

	char **l_ptr;
        int i, n;

	/* if locations as entered are null string, accept all */
	if (strlen(l) == 0)
		return 1;
 
        rdseed_strupr(l);
        rdseed_strupr(location);
         
        n = split(l, &l_ptr, ':');     
 
        for (i = 0; i < n; i++)
	{
		/* map hypen (dash) '-' to space.
		 * Since rdseed truncates locs at the first 
		 * space, we do the same.
		 */

		if (strchr(l_ptr[i], '-'))
		{
			char *p = strchr(l_ptr[i], '-');

			*p = 0;

		}

                if (wstrcmp(location, l_ptr[i], 2) == 0)  
                {
                        fuse(&l_ptr, n);
                        return 1; 
                }

	}
         
        fuse(&l_ptr, n); 
 
        return 0;
         

}

/* ----------------------------------------------------------------------- */


char *summary_file_get_event()
 
{
	return this_event;

}

float summary_file_get_version()
{
	return weed_version;
}

/* ------------------------------------------------------------------------- */
#if 0 

int chk_tspans(start_1, end_1, start_2, end_2)
char *start_1, *end_1, *start_2, *end_2;

{
	struct time blk_start, blk_end;
	struct time req_start, req_end;

	timecvt(&blk_start, start_1);
	timecvt(&blk_end, end_1);
	
        timecvt(&req_start, start_2);
        timecvt(&req_end, end_2); 

	/* Look at 4 possibilities :
	 *
 	 * 1: starts are equal i.e:
	 *	blk |----------------------| 
	 *      req |---------------------------| 
	 *
	 * 2. start of blk is greater than req start, 
	 *       but not greater than req end, i.e:
	 *     blk       |------------------------------|
	 *     req  |-------------------------|
	 *
	 * 3. start of blk before req start, but end is before req end, i.e:
	 *      blk |--------------------------|
	 *      req              |-----------------|
	 * 
	 * 4. blk overlaps request
	 *     blk |--------------------------|
	 *     req      |-----------------|
	 * 	
	 */

	/* #4 */
	if ((timecmp(req_start, blk_start) >= 0) &&
		(timecmp(req_end, blk_end) <= 0))
		return 4;

	/* # 1 - does start of block == request ? */
	if (timecmp(blk_start, req_start) == 0)
		return 1;

	if (timecmp(blk_start, req_start) > 0)
	{
	/* #2 */
		if (timecmp(blk_start, req_end) <= 0)
			return 1;
	}

	/* # 3*/
	if ((timecmp(blk_end, req_start) > 0) &&
		(timecmp(blk_end, req_end) <= 0))
		return 1;

	/* Got here? didn't find it */
	return 0;
	
}
#endif

/* ------------------------------------------------------------ */
int setup_tspan(tspan_start, tspan_end)
char *tspan_start, *tspan_end;

{
	/* fake out computer into thinking this timespan was entered 
	 * at the computer. Add into holding array... */

	start_time_count++;
	end_time_count++;

	if (start_time_count == 1)	/* 1st time through */
		start_time_point = (struct time *)malloc(sizeof(struct time));
	else
                start_time_point = (struct time *)realloc(start_time_point,
							   sizeof(struct time) * start_time_count);

	if (end_time_count == 1)
		end_time_point = (struct time *)malloc(sizeof(struct time));
	else
		end_time_point = (struct time *)realloc(end_time_point,
							sizeof(struct time) * end_time_count);

	timecvt(&start_time_point[start_time_count - 1], tspan_start);
	timecvt(&end_time_point[end_time_count - 1], tspan_end);

}

/* ------------------------------------------------------------------------- */


