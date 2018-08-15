/*===========================================================================*/
/* SEED reader     |            ah_resp                    |     sub program */
/*                 |                                       |                 */
/*===========================================================================*/
/*
    Name:       ah_resp
    Purpose:    Extract responses from Standard Exchange of Earthquake Data
            (SEED) format volume and write output data as L-DGO AH-file format.
            The routine fills response table into AH-file header.
    Usage:      usage is:
                ah_resp (&hed)

                    where &hed is pointer to ahhed


        AH format file header includes instrument response table
        in terms of poles and zeroes, and normalization factor (A0)
        and overall digital sensitivity (DS).

        following extra information is put into AH header:

        extra[0] = 1, for analog response only,
             = 2, analog and digital response,
        extra[1] = Normalization frequency for analog response,
        extra[2] = Frequency where sensitivity (DS) is measured,
        station.cali[i].pole.r = Number of numerator coefficients for FIR filter
                         i = number of poles(analog) + 1
        station.cali[j].pole.r = numerator coefficients are stored
        station.cali[j].pole.i = starting from j, in parallel

    Author:   Chris Laughbon 
            07/27/94

*/

/* -------------------------------------------------------------------- */
/* ----- Includes ----- */
#include <string.h>

#include "rdseed.h"
#include "ahhead.h"

/* ----- Defines ----- */

/* ----- Variables --- */
static struct type53sub *poles;
static struct type53sub *zeros;

/* ----- Prototypes ---- */ 

float calc_A0();
 
int cmp_floats();

struct type33 *find_type_33();
struct type34 *find_type_34();
struct type53 *find_type_53();
struct type58 *find_type_58_stage_0();

struct type41 *get_41();
struct type42 *get_42();
struct type44 *get_44();
struct type45 *get_45();
struct type46 *get_46();
struct type47 *get_47();






struct type48 *find_type_48();
struct type48 *find_type_48_stage();

struct type48 *get_type_48();
struct type58 *get_type_58();

struct type48 *get_48();


struct type43 *find_type_43();
struct type43 *get_type_43();
struct type43 *get_43();



float get_A0();

void determine_gamma();
 
/* ------------------------------------------------------------------------- */
void fill_ah_resp(hed)
ahhed *hed;

{
	struct type33 *type_33;
	struct type34 *type_34;

	struct type53 *type_53;
	struct type43 *type_43;

	int i;

	int num_poles, num_zeros;

	float Sd, fn, fs, A0, calculated_A0;

	int gamma;

	poles = zeros = 0;

	A0 = calculated_A0 = 0;
	
	
	/* find out the response type - set the gamma, etc */
	type_34 = find_type_34(type34_head, current_channel->signal_units_code);

	if (type_34 != NULL)
		determine_gamma(type_34, &gamma);
	else
	{
		fprintf(stderr, "Warning - couldn't find the abbrevation for the signal units code! Signal units code =%d\n", current_channel->signal_units_code);

		fprintf(stderr, "For station: %s; channel: %s\n\n", current_station->station, current_channel->channel);

		fprintf(stderr, "Setting the number of zeros to add to 0\n");
		gamma = 0;

	}

	type_33 = find_type_33(type33_head, current_channel->instrument_code);

	/* transfer abbreviations */
	if (type_33 != NULL)
	{
		/* COMSIZE is defined in ahhead.h - currently 80 chars */

		/* instrument name */
		strncat(hed->record.rcomment, 
				type_33->abbreviation, COMSIZE);

	}
	else
	{
		sprintf(hed->station.stype, "N/A");
		sprintf(hed->record.rcomment, "Not Found");
	}

        type_53 = (struct type53 *)NULL;
        type_43 = (struct type43 *)NULL;
        
        /* one or the other will be there, if not.. . finished */
 
        type_53 = find_type_53(current_channel->response_head);
 
        type_43 = find_type_43(current_channel->response_head);
 
	if ((type_53 == NULL) && (type_43 == NULL))
	{
		fprintf(stderr, "AH output(): Unable to continue! Unable to calulate A0. No responses.\n");

		return;
	}

	A0 = get_A0(&poles, &zeros, 
			&num_poles, &num_zeros, 
			&Sd, gamma, 
			current_channel->response_head,
			sizeof(hed->station.cal)/sizeof(hed->station.cal[0]), FALSE);


	if (A0 == -1) 	/* error */
	{
		if (poles != NULL)
			free(poles);

		if (zeros != NULL)
			free(zeros);

		poles = zeros = 0;

		return;

	}

	/* load up the poles and zeros */
	hed->station.cal[0].pole.r = num_poles;
	hed->station.cal[0].zero.r = num_zeros;

	for (i = 0; i < num_poles; i++)
	{
		hed->station.cal[i+1].pole.r = poles[i].real;

		hed->station.cal[i+1].pole.i = poles[i].imag;
	}

	for (i = 0; i < num_zeros; i++)
	{
		hed->station.cal[i+1].zero.r = zeros[i].real;

		hed->station.cal[i+1].zero.i = zeros[i].imag;
	}

	hed->station.DS = Sd;
	hed->station.A0 = A0;

	free(poles);
	free(zeros);

	return;
	
}

/* -------------------------------------------------------------- */
float get_A0(poles, zeros, num_ps, num_zs, 
		sd, gammas, res_head, max_pzs, sac_flag)
struct type53sub **poles;
struct type53sub **zeros;
int *num_ps, *num_zs;
float *sd;
int gammas;
struct response *res_head;
int max_pzs;
int sac_flag;


{
	int found_43;
	
	struct type58 *type_58;

	struct type48 *type_48;

	/* struct type60 *type_60; */

        struct type43 *type_43;
        struct type53 *type_53;

	int num_poles, num_zeros;
	float fn;
	float fs;
	float calculated_A0, A0;

	struct response *r = res_head;

	int i;
	int j;


	num_zeros = num_poles = 0;

	A0 = 1;

	type_53 = NULL;

	found_43 = FALSE;

	calculated_A0 = 0;

	/* make sure they are null so realloc will malloc 1st time only */

	*poles = NULL;
	*zeros = NULL;

	while (r != NULL) {

		if (r->type == 'P') {

			/* found polesnzeros blockette */

			type_53 = r->ptr.type53;

  			if (type_53->stage == 1) 
				fn = type_53->norm_freq;
 
    			A0 *= type_53->ao_norm;

			if (num_poles + type_53->number_poles >= max_pzs)
			{

				fprintf(stderr, "Warning, exceeded maximum number of poles. Clipping at stage =%d\n", type_53->stage);
                                break;
			}

			if (num_zeros + type_53->number_zeroes + (type_53->stage==1?gammas:0) >= max_pzs) 
                        { 
 
                                fprintf(stderr, "Warning, exceeded maximum number of zeros. Clipping at stage =%d\nNetwork: %s, Station: %s, Channel: %s, Location:%s\n", 
					type_53->stage,
					current_station->network_code,
					current_station->station,
					current_channel->channel,
					current_channel->location);
                                break;
                        } 

			/* reallocate memory to hold the pNzs */
			if (type_53->number_poles > 0)
			if ((*poles = (struct type53sub *)
					realloc(*poles, 
						sizeof(struct type53sub) * 
							(type_53->number_poles + num_poles))) == NULL)
			{
				fprintf(stderr, "Unable to obtain memory for AH poles conversion!\nA0 set to zero for station %s; channel %s\n",
					current_station->station,
					current_channel->channel);

				return 0;
	
		
			}

			if ((type_53->stage == 1 ? type_53->number_zeroes + gammas : 
						type_53->number_zeroes) > 0)
			{

				if ((*zeros = (struct type53sub *)
					realloc(*zeros, sizeof(struct type53sub) * 
						((type_53->stage == 1 ? 
							type_53->number_zeroes + (sac_flag ? 0 : gammas) : 
								type_53->number_zeroes) + num_zeros))) == NULL)
                		{
                       			fprintf(stderr, "Unable to obtain memory for AH zeros conversion!\nA0 set to zero for station %s; channel %s\n",
                                	current_station->station,
                                	current_channel->channel);
                        	
					return 0;
                		}

			}

		
                	for (i = num_poles; i < type_53->number_poles + num_poles; i++)
                        	(*poles)[i] = type_53->pole[i - num_poles];

                	for (i = num_zeros; i < type_53->number_zeroes + num_zeros; i++)
			{
                        	(*zeros)[i] = type_53->zero[i - num_zeros];
			}


    			/*
     		 	 * First, AH assumes the units of the poles and zeros are rad/sec,
     		 	 * so we convert Type B (Hz) to Type A (rad/sec) if necessary.
     		 	 *  
     		  	 * If Type==B then convert to type A format by:
     		 	 *
     		 	 * P(n) = 2*pi*P(n)      { n=1...Np }
     		 	 * Z(m) = 2*pi*Z(m)      { m=1...Nz }
    		 	 * A0   = A0 * (2*pi)**(Np-Nz)
     		 	 */ 
			if (*type_53->transfer == 'B')
    			{
 
				for (i = 0; i < type_53->number_poles; i++)
        			{
            				(*poles)[num_poles + i].real *= (2 * PI);
            				(*poles)[num_poles + i].imag *= (2 * PI);
        			}
 
        			for (i = 0; i < type_53->number_zeroes; i++)
        			{
            				(*zeros)[num_zeros + i].real *= (2 * PI);
            				(*zeros)[num_zeros + i].imag *= (2 * PI);

        			}
 
        			/* A0   = A0 * (2*pi)**(Np-Nz) */

        			A0 = A0 *
            				pow(2 * PI, 
					    (double)(type_53->number_poles - 
						type_53->number_zeroes));
 
    			}       /* if transfer function was analog - 'B' */

			/* add zeros, increment num_zeros, if stage 1 */
			if ((type_53->stage == 1) && (!sac_flag))
			{
				for (i = 0; i < gammas; i++)
				{
                                	(*zeros)[num_zeros + type_53->number_zeroes + i].real = 0;
					(*zeros)[num_zeros + type_53->number_zeroes + i].imag = 0;
                        		(*zeros)[num_zeros + type_53->number_zeroes + i].real_error = 0;
                        		(*zeros)[num_zeros + type_53->number_zeroes + i].imag_error = 0;
				}

				num_zeros += gammas;
			}

    			num_poles += type_53->number_poles;
    			num_zeros += type_53->number_zeroes;

		/* type 53s */
		} else if (r->type == 'R') {

			int stage;
			int i;

			/*for (i = 0, stage = 1; i < r->ptr.type60->number_stages; i++, stage++)*/
			for (stage = 1; stage <= r->ptr.type60->number_stages;stage++) 
			{

				type_43 = get_type_43(r->ptr.type60->stage+(stage - 1));

				if (type_43 == NULL)
					continue;

				if (stage == 1)
    					fn = type_43->norm_freq;
 
    				A0 *= type_43->ao_norm;

				if (num_poles + type_43->number_poles + (stage==1?gammas:0)>= max_pzs) {
					fprintf(stderr, "Warning, exceeded maximum number of poles. Clipping at stage =%d\n", type_53->stage);
                                	break;
				}
	
				if (num_zeros + type_43->number_zeroes >= max_pzs) { 
                                	fprintf(stderr, "Warning, exceeded maximum number of zeros. Clipping at stage =%d\nNetwork: %s, Station: %s, Channel: %s, Location:%s\n", 
						stage,
						current_station->network_code,
						current_station->station,
						current_channel->channel,
						current_channel->location);
                                	break;
                        	} 

				/* reallocate memory to hold the pNzs */
				if (type_43->number_poles > 0)
				if ((*poles = (struct type53sub *)
					realloc(*poles, 
						sizeof(struct type53sub) * 
							(type_43->number_poles + num_poles))) == NULL)
				{
					fprintf(stderr, "Unable to obtain memory for AH poles conversion!\nA0 set to zero for station %s; channel %s\n",
						current_station->station,
						current_channel->channel);

					return 0;
				}

				if (((stage == 1 ? type_43->number_zeroes + gammas : 
						type_43->number_zeroes) > 0))
				{
					if ((*zeros = (struct type53sub *)
						realloc(*zeros, sizeof(struct type53sub) * 
							(stage == 1 ? type_43->number_zeroes + (sac_flag ? 0 : gammas) :
								type_43->number_zeroes + num_zeros))) == NULL)
               				{
               					fprintf(stderr, "Unable to obtain memory for AH zeros conversion!\nA0 set to zero for station %s; channel %s\n",
                                			current_station->station,
                                			current_channel->channel);
                       				return 0;
               				}

				}

                		for (i = num_poles; i < type_43->number_poles + num_poles; i++)
				{
					(*poles)[i].real = type_43->pole[i-num_poles].real;
                        		(*poles)[i].imag = type_43->pole[i-num_poles].imag;
                        		(*poles)[i].real_error = type_43->pole[i-num_poles].real_error;
                        		(*poles)[i].imag_error = type_43->pole[i-num_poles].imag_error;
				}

                		for (i = num_zeros; i < type_43->number_zeroes + num_zeros; i++)
				{
                        		(*zeros)[i].real = type_43->zero[i-num_zeros].real;
                        		(*zeros)[i].imag = type_43->zero[i-num_zeros].imag;
                        		(*zeros)[i].real_error = type_43->zero[i-num_zeros].real_error;
                        		(*zeros)[i].imag_error = type_43->zero[i-num_zeros].imag_error;
                		}


    				/*
     		 	 	 * First, AH assumes the units of the poles and zeros are rad/sec,
     		 	 	 * so we convert Type B (Hz) to Type A (rad/sec) if necessary.
     		 	 	 *  
     		  	 	 * If Type==B then convert to type A format by:
     		 	 	 *
     		 	 	 * P(n) = 2*pi*P(n)      { n=1...Np }
     		 	  	 * Z(m) = 2*pi*Z(m)      { m=1...Nz }
    		 	 	 * A0   = A0 * (2*pi)**(Np-Nz)
     		 	 	 */ 

				if (type_43->response_type == 'B')
    				{
 
					for (i = 0; i < type_43->number_poles; i++)
        				{
            					(*poles)[num_poles + i].real *= (2 * PI);
            					(*poles)[num_poles + i].imag *= (2 * PI);
        				}
 
        				for (i = 0; i < type_43->number_zeroes; i++)
        				{
            					(*zeros)[num_zeros + i].real *= (2 * PI);
            					(*zeros)[num_zeros + i].imag *= (2 * PI);
        				}
 
        				/* A0   = A0 * (2*pi)**(Np-Nz) */
        				A0 = A0 *
            					pow(2 * PI, 
						    (double)(type_43->number_poles - 
								type_43->number_zeroes));
 
    				}       /* if transfer function was analog - 'B' */

				/* add zeros, increment num_zeros, if stage 1 */
				if ((stage == 1) && (!sac_flag))
				{
                                	for (i = 0; i < gammas; i++) {
                                        	(*zeros)[num_zeros + type_43->number_zeroes + i].real = 0;
                                        	(*zeros)[num_zeros + type_43->number_zeroes + i].imag = 0;
                                        	(*zeros)[num_zeros + type_43->number_zeroes + i].real_error = 0;
                                        	(*zeros)[num_zeros + type_43->number_zeroes + i].imag_error = 0;
                                	}
                                	num_zeros += gammas;
				}

    				num_poles += type_43->number_poles;
    				num_zeros += type_43->number_zeroes;

				found_43 = TRUE;

			}

		}

		r = r->next;
	}

	if (type_53 == NULL && (!found_43))
	{
		fprintf(stderr, "Warning - couldn't find the Poles and Zeros!!\n");

		fprintf(stderr, 
			"For station: %s; channel: %s\n", 
			current_station->station, 
			current_channel->channel);
        
        	return -1;	/* flag error condition */
	}

 
    /*
     * Second, there is no place to specify the units of the response.
     * An AH file assumes that if an instrument response is deconvolved,
     * the seismogram will be displacement in meters.
     * Convert velocity or acceleration to displacement:
     *
     * Convert to displacement:
     *
     * if acceleration, gamma=2    \
     * elseif velocity,     gamma=1 \
     * elseif displacement, gamma=0  \___Done above
     * else  print error message     /
     * endif                        /
     *
     * Sd = Sd * (2*pi*fs)**gamma
     * Nz = Nz + gamma
     * set values of new zeros equal zero
     * A0 = A0 / (2*pi*fn)**gamma
     * Units = M - Displacement Meters
     */
                
    	type_58 = find_type_58_stage_0(res_head);
 
    	if (type_58 != NULL)
	{
		*sd = type_58->sensitivity;

    		fs = type_58->frequency;

	}
	else
	{
		type_48 = find_type_48_stage(0, res_head);
		
		if (type_48 != NULL)
		{
			*sd = type_48->sensitivity;
    			fs = type_48->frequency;

		}
		else
		{	

        		fprintf(stderr, "WARNING - couldn't find - blockette (58/48) stage zero!!\n");
 
        		fprintf(stderr, "For station: %s; channel: %s\n",
                    	current_station->station, current_channel->channel);
 
        		return -1;	/* error condition */
    		}

	} 

	if (sac_flag)
	{
		*num_ps = num_poles;
		*num_zs = num_zeros;

		return A0;
	}

	*sd *= pow(2 * PI * fs, (double)gammas);

	A0 = A0 / pow((double)(2 * PI * fn), (double)gammas);

    /*
     * Third, there is no place in the AH header to specify either
     * the frequency of normalization or the frequency of the
     * digital sensitivity.  This is not a problem as long as these
     * two are the same.  If they are different then evaluate the
     * normalization at the frequency of the digital sensitivity.
     *
     *
     * if fn is not equal to fs then
     *  A0 = abs(prod{n=1...Np} [2*pi*i*fs - P(n)] /
                        prod{m=1..Nz} [2*pi*i*fs - Z(m)])
     *
     * endif
     * i = sqrt(-1)
     */

	/* default to the A0 as computed above */
	if ((num_zeros == 0) || (num_poles == 0))
		calculated_A0 = A0;
	else
    		calculated_A0 = calc_A0(num_poles, 
					num_zeros, 
					*poles,
					*zeros,
					fs);
 
	if (!cmp_floats(fn, fs))
        	A0 = calculated_A0;
	else
    	/* they are the same, perform consistancy check */
    	{
        	/* check to see if they differ by greater than .5% */
        	if (fabs((A0 - calculated_A0) / calculated_A0) > .005)
        	{
            		fprintf(stderr, "Warning, Normalization given for station: %s, channel %s is :%f.\nThis is inconsistent with the value calculated from poles and zeros: %f.\n\n", current_station->station, current_channel->channel, A0, calculated_A0);

            		/* use the calculated A0 */
            		A0 = calculated_A0;
        	}

	}

	*num_ps = num_poles;
	*num_zs = num_zeros;

	return A0;

}
/* -------------------------------------------------------------- */

void determine_gamma(t_34, add_zeros)
struct type34 *t_34;
int *add_zeros;

{
	char *p;

	*add_zeros = 0;

	if (t_34 != NULL)
	{

		if (t_34->description)
		{
			p = t_34->description;

			rdseed_strupr(t_34->description);
			if (strstr(t_34->description, "VEL") != 0)
            		{
                		*add_zeros = 1;
				return;
            		}
			if (strstr(t_34->description, "ACCEL") != 0) 
            		{
                		*add_zeros = 2; 
				return;
            		} 

			if (strstr(t_34->description, "DISP") != 0) 
            		{
				/* no zeros - just go on */
				return;
			}
		}
		else
		{
			p = t_34->name;
			if (strcmp(t_34->name, "M") == 0)
				return;		/* displacement */
 
			if (strcmp(t_34->name, "M/S") == 0)
			{
				*add_zeros = 1;
				return;		/* Velocity */ 
			} 

			if (strcmp(t_34->name, "M/S**2") == 0)
			{
				*add_zeros = 2;
				return;		/* Acceleration */
			}

		}		
	}		/* if t_34 != NULL */
	
	/* if we got here - flag error ! */
	fprintf(stderr, "WARNING - unknown response type - we only know acceleration, velocity, and displacement.\nFound: %s\n", p);
            
   	fprintf(stderr, "For station: %s; channel: %s\n", current_station->station, current_channel->channel);
   
	fprintf(stderr, "Assuming a gamma of zero!\n");
 
	return;
	
}

/* --------------------------------------------------------------- */
struct type53 *find_type_53(r)
struct response *r;


{
	/*
	 * Type = Transfer function type (Blockette 53)
	 *  should be either A (Laplace transform analog response, in rad/sec)
	 *  or B (Analog response, in Hz)
	 */


	int low_stage = 999999;
	struct type53 *ptr = NULL;

    	while (r)
    	{
		if ((r->type == 'P') && 
			(*(r->ptr.type53->transfer) != 'D') &&
			(r->ptr.type53->stage != 0))
			if (r->ptr.type53->stage < low_stage)
			{
				low_stage = r->ptr.type53->stage;
				ptr = r->ptr.type53;
			}

		r = r->next;

    	}

    return ptr;
}

/* ----------------------------------------------------------------------- */
struct type43 *find_type_43(r)
struct response *r;


{
 
	struct type60 *type_60;
		
	struct type43 *type_43;

	int ix;
 
    	int low_stage = 999999;
    	struct type43 *ptr = NULL;

	while (r)
    	{
        	if (r->type == 'R')
		{
			type_60 = r->ptr.type60; 

			for (ix = 0; ix < type_60->number_stages; ix++)
			{		

				type_43 = get_type_43(type_60->stage + ix);

				if ((type_43 != NULL) &&
				   (type_60->stage[ix].value != 0) &&
				   (type_60->stage[ix].value < low_stage))
				{
					ptr = type_43;
					low_stage = type_60->stage[ix].value;
				}
					
			}
		}
               
        r = r->next;
    }
 
    return ptr;
}
 
/* ------------------------------------------------------------------------ */

struct type41 *get_type_41(stage)
struct type60sub1 *stage;

{
	
	struct type41 *type41_ptr;

	int i;

	for (i = 0; i < stage->number_responses; i++)
	{

		type41_ptr = get_41(stage->response[i].reference, type41_head);	
		
		if (type41_ptr)
			return type41_ptr;
	}

	return NULL;

}

/* ------------------------------------------------------------------------ */

struct type42 *get_type_42(stage)
struct type60sub1 *stage;

{
	
	struct type42 *type42_ptr;

	int i;

	for (i = 0; i < stage->number_responses; i++)
	{

		type42_ptr = get_42(stage->response[i].reference, type42_head);	
		
		if (type42_ptr)
			return type42_ptr;
	}

	return NULL;

}

/* ------------------------------------------------------------------------ */

struct type43 *get_type_43(stage)
struct type60sub1 *stage;

{
	
	struct type43 *type43_ptr;

	int i;

	for (i = 0; i < stage->number_responses; i++)
	{

		type43_ptr = get_43(stage->response[i].reference, type43_head);	
		
		if (type43_ptr)
			return type43_ptr;
	}

	return NULL;

}

/* ------------------------------------------------------------------------ */

struct type44 *get_type_44(stage)
struct type60sub1 *stage;

{
	
	struct type44 *type44_ptr;

	int i;

	for (i = 0; i < stage->number_responses; i++)
	{

		type44_ptr = get_44(stage->response[i].reference, type44_head);	
		
		if (type44_ptr)
			return type44_ptr;
	}

	return NULL;

}

/* ------------------------------------------------------------------------ */

struct type45 *get_type_45(stage)
struct type60sub1 *stage;

{
	
	struct type45 *type45_ptr;

	int i;

	for (i = 0; i < stage->number_responses; i++)
	{

		type45_ptr = get_45(stage->response[i].reference, type45_head);	
		
		if (type45_ptr)
			return type45_ptr;
	}

	return NULL;

}

/* ------------------------------------------------------------------------ */

struct type41 *get_41(code, t_41)
int code;
struct type41 *t_41;

{
	while (t_41)
	{
		if (t_41->response_code == code)
			break;

		t_41 = t_41->next;
	}

	return t_41;

}

/* ------------------------------------------------------------------------- */
struct type42 *get_42(code, t_42)
int code;
struct type42 *t_42;

{
	while (t_42)
	{
		if (t_42->response_code == code)
			break;

		t_42 = t_42->next;
	}

	return t_42;

}

/* ------------------------------------------------------------------------- */
struct type43 *get_43(code, t_43)
int code;
struct type43 *t_43;

{
	while (t_43)
	{
		if (t_43->response_code == code)
			break;

		t_43 = t_43->next;
	}

	return t_43;

}

/* ------------------------------------------------------------------------- */
struct type44 *get_44(code, t_44)
int code;
struct type44 *t_44;

{
	while (t_44)
	{
		if (t_44->response_code == code)
			break;

		t_44 = t_44->next;
	}

	return t_44;

}
/* ------------------------------------------------------------------------- */
struct type45 *get_45(code, t_45)
int code;
struct type45 *t_45;

{
	while (t_45)
	{
		if (t_45->response_code == code)
			break;

		t_45 = t_45->next;
	}

	return t_45;

}

/* ------------------------------------------------------------------------- */
struct type46 *get_46(code, t_46)
int code;
struct type46 *t_46;

{
	while (t_46)
	{
		if (t_46->response_code == code)
			break;

		t_46 = t_46->next;
	}

	return t_46;

}
/* ------------------------------------------------------------------------- */
struct type48 *find_type_48(r)
struct response *r;


{

    struct type60 *type_60;
       
    struct type48 *type_48;

    int ix;

    int low_stage = 999999;
    struct type48 *ptr = NULL;

    while (r)
    {
        if (r->type == 'R')
        {

            type_60 = r->ptr.type60;

            for (ix = 0; ix < type_60->number_stages; ix++)
            {

                type_48 = get_type_48(type_60->stage+ix);

                if (type_48 != NULL)
			if (type_60->stage[ix].value < low_stage)
			{
				ptr = type_48;
				low_stage = type_60->stage[ix].value;
			}
    
            }
        }
        r = r->next;
    }

    return ptr;
}

/* ------------------------------------------------------------------------ */
struct type48 *find_type_48_stage(int which_stage, struct response *r)

{

	struct type60 *type_60;
       
	struct type48 *ptr = NULL;

	int ix;

	while (r)
	{

		if (r->type == 'R')
		{

			type_60 = r->ptr.type60;

			for (ix = 0; ix < type_60->number_stages; ix++)
			{

				if (type_60->stage[ix].value == which_stage)
				{
					ptr = get_type_48(type_60->stage+ix);
				}

			}
		}

		r = r->next;
	}

	return ptr;
}

/* ------------------------------------------------------------------------ */

struct type46 *get_type_46(stage)
struct type60sub1 *stage;

{
	
	struct type46 *type46_ptr;

	int i;

	for (i = 0; i < stage->number_responses; i++)
	{

		type46_ptr = get_46(stage->response[i].reference, type46_head);	
		
		if (type46_ptr)
			return type46_ptr;
	}

	return NULL;

}
/* ------------------------------------------------------------------------ */

struct type48 *get_type_48(stage)
struct type60sub1 *stage;

{

    struct type48 *type48_ptr;

    int i;

    for (i = 0; i < stage->number_responses; i++)
    {
        type48_ptr = get_48(stage->response[i].reference, type48_head);

        if (type48_ptr)
            return type48_ptr;
    }
 
    return NULL;
 
}

/* ------------------------------------------------------------------------- */

struct type48 *get_48(code, t_48)
int code;
struct type48 *t_48;

{
    while (t_48)
    {
        if (t_48->response_code == code)
            break;

        t_48 = t_48->next;
    }

    return t_48;

}

/* ------------------------------------------------------------------------- */
struct type33 *find_type_33(t_33, code)
struct type33 *t_33;
int code;

{
	while (t_33)
	{
		if (t_33->code == code)
			return t_33;

		t_33 = t_33 ->next;
	}

	return NULL;

}
 
/* -------------------------------------------------------------- */
struct type34 *find_type_34(t_34, code)
struct type34 *t_34;
int code;
 
{
    while (t_34)
    {
        if (t_34->code == code)
            return t_34;
 
        t_34 = t_34 ->next;
    }
 
    return NULL;
 
}
 
/* -------------------------------------------------------------- */
/* routines to calculate the response A0 normalization from 
 * pole and zeros as supplied by Sandy Stromme.
 *
 */

/* 
struct complex {
        double x;
        double y;
};
*/ 
/* prototypes */
d_complex c_mult();
 
d_complex c_add();
 
d_complex c_sub();
 
d_complex c_div();
 
double c_abs();
/* -------------------------------------------------------------- */
/* entry routine */
float calc_A0(n_ps, n_zs, poles, zeros, ref_freq)
int n_ps;
int n_zs;
struct type53sub poles[];
struct type53sub zeros[]; 
float ref_freq;
 
{
	int i;

    	d_complex numer, denom, f0, hold;
    	double a0;

	 
    	f0.r = 0;
 
    	f0.i = 2 * PI * ref_freq;

	hold.i = zeros[0].imag;
	hold.r = zeros[0].real;
	denom = c_sub(f0, hold);

    	for (i = 1; i < n_zs; i++)
	{
		hold.i = zeros[i].imag;
    		hold.r = zeros[i].real;

       		denom = c_mult(denom, c_sub(f0, hold));

	}

	hold.i = poles[0].imag;
	hold.r = poles[0].real;

	numer = c_sub(f0, hold);
 
	for (i = 1; i < n_ps; i++)
	{
		hold.i = poles[i].imag; 
    		hold.r = poles[i].real;

        	numer = c_mult(numer, c_sub(f0, hold));

	}
   
    	a0 = c_abs(c_div(numer, denom));
 
    	return a0;
    
}
 
/* ----------------------------------------------------------- */
d_complex c_mult (a, b)
d_complex a, b;
{
 

        d_complex mult;
 
        mult.r = (a.r * b.r) - (a.i * b.i);
        mult.i = (a.r * b.i) + (a.i * b.r);
 
        return mult;
 
}
 
d_complex c_add (a, b)
d_complex a, b;
{
 
        d_complex add;
 
        add.r = a.r + b.r;
        add.i = a.i + b.i;
 
        return add;
 
}
 
d_complex c_sub (a, b)
d_complex a, b;
{
 
        d_complex sub;
 
        sub.r = a.r - b.r;
        sub.i = a.i - b.i;
 
        return sub;
 
}

 
 
d_complex c_div (a, b)
d_complex a, b;
 
{
 
        d_complex div;
 
        b.i = -b.i;
        div = c_mult(a, b);
        div.r /= b.r * b.r + b.i * b.i;
        div.i /= b.r * b.r + b.i * b.i;
 
        return div;
 
}
 
 
double c_abs(a)
d_complex a;
 
{
        return (pow((double)(a.r * a.r) + (a.i * a.i), 0.5));
}
/* --------------------------------------------------------------- */
struct type58 *find_type_58_stage_0(r)
struct response *r;

{
	return get_type_58(0, r);
}
 
/* --------------------------------------------------------------- */


struct type58 *get_type_58(int stage, struct response *r)
 
{
    while (r)
    {
        if (r->type == 'S')                 /* 'S' for type 58 */
            if (r->ptr.type58->stage == stage)
                return r->ptr.type58;
    
        r = r->next;
    }
 
    return NULL;
    
}
 
/* -------------------------------------------------------------- */
int cmp_floats(f1, f2)
float f1;
float f2;
 
{
    char f_1[30];
    char f_2[30];
 
    sprintf(f_1, "%6.6f", f1);
    sprintf(f_2, "%6.6f", f2);
 
    return (strcmp(f_1, f_2) == 0);
}
