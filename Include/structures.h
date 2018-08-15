/*===========================================================================*/
/* SEED reader     |            structures.h               |     header file */
/*===========================================================================*/
/*
	Name:		structures.h
	Purpose:	contains structure definitions for rdseed programs
	Usage:		#include "structures.h"
	Input:		none
	Output:		none
	Warnings:	none
	Errors:		none
	Called by:	rdseed.h
	Calls to:	none
	Algorithm:	none
	Notes:		Memory for most structures, strings, and so on is dynamically
				allocated at run time.
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/04/88  Dennis O'Neill  Added STRINGTERM definition
				11/21/88  Dennis O'Neill  Production release 1.0
				05/01/90  Sue Schoch      Production release 2.0
                          001             Included blockette 57 support
				09/01/93  CL  added modification for volume label
				03/25/99  Stephane Zuzlewski	Added Polynomial support
*/

/* If this header is already included, don't include it again */
#ifndef STRUCTURES_H
#define STRUCTURES_H

/*                 +=======================================+                 */
/*=================| Command-line option/argument structure|=================*/
/*                 +=======================================+                 */

#define OPTS 100
#define STRLEN 100
struct			optstruct
{
	int		number_options;		/* # options found */
	int		next_arg;		/* argv index to next arg */
	char		option[OPTS];		/* list of allowed options */
	char		option_present[OPTS];	/* TRUE if option present */
	char		argument[OPTS][STRLEN];	/* argument if present */
};

/*                 +=======================================+                 */
/*=================|        Date-and-time structure        |=================*/
/*                 +=======================================+                 */

struct	time
{
	int	year;
	int	day;
	int	hour;
	int	minute;
	int	second;
	int	fracsec;
};
/*                 +=======================================+                 */
/*=================|     Phase-Date-and-time structure     |=================*/
/*                 +=======================================+                 */

struct			phase
{
	char			name[30];
	int			year;
	int			day;
	int			hour;
	int			minute;
	int			second;
	int			fracsec;
};

/*                 +=======================================+                 */
/*=================|         input data structures         |=================*/
/*                 +=======================================+                 */

struct	logical_record		/* input logical record */
{
	long int recordnumber;		/* input record number */
	char	type;			/* type of record */
	int	continuation;		/* continuation (T|F) */
// 	char	data[LRECL_DEF-8];	/* input data */
	char    data[LRECL_DEF];
};

/*                 +=======================================+                 */
/*=================|        volume header structures       |=================*/
/*                 +=======================================+                 */

struct			type10							/* volume identifier */
{
	double	version;	/* SEED version number */
	int	log2lrecl;	/* log base 2 of lrecl */
	char	*bov;		/* beginning of volume date */
	char	*eov;		/* end of volume date */
	char	*volume_time;	/* creation date of volume */
	char	*organization;	/* originating organization */
	char 	*volume_label;	/* optional volume label */
};

struct	type11sub	/* used in struct type11 */
{
	char	*station_id;	/* station identifier */
	long int	sequence_number;	/* sequence number */
};

struct	type11		/* vol. station header index */
{
	int number_stations;	/* number of stations */
	struct	type11sub *station;	/* substructure */
	struct type11 *next;
};

struct	type12sub	/* used in struct type12 */
{
	char	*bos;	/* beginning of span */
	char	*eos;	/* end of span */
	long int sequence_number; /* sequence number */
};

struct	type12	/* vol. timespan index */
{
	int	number_spans; /* number of spans */
	struct	type12sub *timespan;	/* timespan */
	struct type12 *next;
};

/*                 +=======================================+                 */
/*=================|     dictionary (table) structures     |=================*/
/*                 +=======================================+                 */

struct			type30							/* data format dictionary */
{
	char	*name;		/* short descriptive name */
	int	code;		/* data format id code */
	int	family;		/* data decoder family type */
	int	number_keys;	/* number of decoder keys */
	char	**decoder_key;	/* decoder keys */
	char	**decoder_key_prim;	/* decoder keys */
	struct	type30 *next;	/* ptr to next type30 entry */
};

struct	type31	/* comment description dctnry */
{
	int	code;	/* comment id code */
	char	*class;	/* comment class code */
	char	*comment;	/* comment text */
	int	units;	/* units of comment */
	struct	type31 *next; /* ptr to next type31 entry */
};

struct	type32	/* cited source dictionary */
{
	int	code;		/* source id code */
	char	*author;	/* author/publication */
	char	*catalog;	/* date, catalog info */
	char	*publisher;	/* publisher name */
	struct	type32 *next;	/* ptr to next type32 entry */
};

struct	type33			/* generic abbrev dctnry */
{
	int	code;		/* abbreviation ident code */
	char	*abbreviation;	/* abbrev. description */
	struct	type33 *next;	/* ptr to next type33 entry */
};

struct	type34		/* units abbreviation dctnry */
{
	int	code;		/* units ident code */
	char	*name;		/* unit name */
	char	*description;	/* unit description */
	struct	type34 *next;	/* ptr to next type34 entry */
};

struct	type35sub		/* used in struct type35 */
{
	char	*station;	/* station ident */
	char	*location;	/* location code */
	char	*channel;	/* channel code */
	int	subchannel;	/* subchannel for mux */
	double	weight;		/* channel weight */
};

struct	type35			/* beam configuration */
{
	int	code;			/* beam ident code */
	int	number_beams;		/* number of beam cmpts */
	struct	type35sub *beam;	/* substructure */
	struct	type35 *next;		/* ptr to next type35 entry */
};

/*                 +=======================================+                 */
/*=================|       station header structures       |=================*/
/*                 +=======================================+                 */

struct	type41	/* symmetrical coefficients response */
{
	int	response_code;	/* response lookup key */
	char	*name;		/* transfer function name */
	char	symmetry_code;  /* response symmetry code */
	int	input_units_code;	/* response in units lookup */
	int	output_units_code;	/* response out units lookup */
	int	number_numerators;	/* number of FIR factors */
	double	*numerator;	/* pointer to numerator list*/
	struct	type41 *next;	/* ptr to next type44 entry */
};

struct	type42sub		/* coefficient (type42) */
{
	double	coefficient;	/* coefficient value */
	double	error;		/* error value */
};

struct	type42	/* polynomial response */
{
	int	response_code;	/* response lookup key */
	char	*name;		/* transfer function name */
	char	transfer_fct_type;	/* transfer function type */
	int	input_units_code;	/* response in units lookup */
	int	output_units_code;	/* response out units lookup */
	char	poly_approx_type;	/* polynomial approximation type */
	char	valid_freq_units;	/* valid frequency units */
	double	lower_valid_freq;	/* lower valid frequency bound */
	double	upper_valid_freq;	/* upper valid frequency bound */
	double	lower_bound_approx;	/* lower bound of approximation */
	double	upper_bound_approx;	/* upper bound of approximation */
	double	max_abs_error;		/* maximum absolute error */
	int	number_coefficients;	/* number of poly. coefficients */
	struct	type42sub *coefficient;		/* pointer to coefficient list */
	struct	type42	*next;		/* ptr to next type42 entry */
};

struct	type43sub	/* pole, zero (type43) */
{
	double	real;	/* real part */
	double	imag;	/* imaginary part */
	double	real_error;	/* real error */
	double	imag_error;	/* imaginary error */
};
	
struct	type43	/* poles and zeroes response */
{
	int	response_code;	/* response lookup key */
	char	*name;		/* transfer function name */
	char	response_type;  /* Laplace,analog,digital */
	int	input_units_code; /* response in units lookup */
	int	output_units_code;	/* response out units lookup */
	double	ao_norm;	/* AO normalization factor */
	double	norm_freq;	/* normalization frequency */
	int	number_zeroes;	/* number of complex zeroes */
	struct	type43sub *zero;/* struct for a complex zero */
	int	number_poles;	/* number of complex poles */
	struct	type43sub *pole;/* struct for a complex pole */
	struct	type43 *next;	/* ptr to next type43 entry */
};

struct	type44sub	/* coefficient (type44) */
{
	double	coefficient;	/* coefficient value */
	double	error;		/* error value */
};

struct	type44		/* coefficients response */
{
	int response_code;	/* response lookup key */
	char *name;		/* transfer function name */
	char response_type;   	/* Laplace,analog,digital */
	int input_units_code;	/* response in units lookup */
	int output_units_code;	/* response out units lookup */
	int number_numerators;	/* number of numerators */
	struct type44sub *numerator;	/* struct of numerator */
	int number_denominators;	/* number of denominators */
	struct type44sub *denominator;  /* struct for denominator */
	struct type44 *next;		/* ptr to next type44 entry */
};

struct	type45sub	/* response list (type45) */
{
	double frequency;		
	double amplitude;	
	double amplitude_error;	
	double phase;	
	double phase_error;
};

struct	type45			/* listed response */
{
	int response_code;	/* response lookup key */
	char	*name;		/* transfer function name */
	int	input_units_code;	/* response in units lookup */
	int	output_units_code;	/* response out units lookup */
	int	number_responses;	/* number of responses */
	struct	type45sub *response;	/* struct for response list */
	struct	type45 *next;		/* ptr to next type45 entry */
};

struct type46sub	/* generic response (type46) */
{
	double	frequency;	/* corner frequency */
	double	slope;		/* slope, db/decade */
};

struct	type46	/* "generic" response */
{		/* aka corner freq/slope resp */
	int response_code;	/* response lookup key */
	char	*name;		/* transfer function name */
	int	input_units_code;	/* response in units lookup */
	int	output_units_code;	/* response out units lookup */
	int	number_corners;		/* number of corners */
	struct	type46sub *corner;	/* struct of corner */
	struct		type46 *next;	/* ptr to next type46 entry */
};

struct	type47				/* decimation description */
{
	int response_code;		/* response lookup key */
	char	*name;			/* transfer function name */
	double	input_sample_rate;	/* input sample rate */
	int	decimation_factor;	/* decimation factor */
	int	decimation_offset;	/* decimation offset */
	double	delay;			/* estimated delay */
	double	correction;		/* correction applied */
	struct	type47 *next;		/* ptr to next type47 entry */
};

struct type48sub	/* chnl sensitivity (type48) */
{
	double	sensitivity;	/* sensitivity */
	double	frequency;	/* frequency of sensitivity */
	char	*time;		/* time of calibration */
};

struct	type48			/* channel sensitivity */
{
	int	response_code;	/* response lookup key */
	char	*name;		/* transfer function name */
	double	sensitivity;	/* sensitivity, cts/unit */
	double	frequency;	/* frequency of sensitivity */
	int	number_calibrations;	/* number of calibrations */
	struct	type48sub *calibration;	/* struct of calibration */
	struct		type48 *next;	/* ptr to next type48 entry */
};

/*                 +=======================================+                 */
/*=================|       station header structures       |=================*/
/*                 +=======================================+                 */

struct	type50 				/* station identifier */
{
	char	*station;		/* station call letters */
	double	latitude;		/* station latitude, -South */
	double	longitude;		/* station longitude, -West */
	double	elevation;		/* station elevation, meters */
	/*int	number_channels;*/	/* number of channels */
	/*int	number_comments;*/	/* number of comment blkts */
	int	reserved1;		/* reserved 4 byte field */
	int	reserved2;		/* reserved 3 byte field */
	char	*name;			/* site name */
	int	owner_code;		/* lookup for owner name */
	int	longword_order;		/* 4-byte word order */
	int	word_order;		/* 2-byte word order */
	char	*start;			/* start effective date */
	char	*end;			/* end effective date */
	char	*update;		/* update flag */
	char	*network_code;		/* v2.3 network code */
	struct	type51 *type51_head;	/* first station comment */
	struct	type51 *type51_tail;	/* last station comment */
	struct	type52 *type52_head;	/* first channel */
	struct	type52 *type52_tail;	/* last channel */
	struct	type50 *station_update;	/* station update */
	struct	type50 *next;		/* ptr to next type50 entry */
};

struct	type51		/* station comment */
{
	char	*start;		/* beginning effective time */
	char	*end;		/* ending effective time */
	int	comment_code;	/* lookup for comment */
	long int level_code;	/* lookup for level */
	struct	type51 *next;	/* ptr to next type51 entry */
};

struct	type52	/* channel identifier */
{
	char *location;	/* location id */
	char	*channel;/* channel id */
	int	subchannel;		/* subchannel for mux */
	int	instrument_code;	/* lookup for inst id */
	char	*inst_comment;		/* instrument comment */
	int	signal_units_code;	/* lookup for signal units */
	int	calib_units_code;	/* lookup for calibration */
	double	latitude;		/* inst latitude */
	double	longitude;		/* inst longitude */
	double	elevation;		/* inst elevation */
	double	local_depth;		/* local depth */
	double	azimuth;		/* inst azimuth rel to N */
	double	dip;			/* inst dip down from horiz */
	int	format_code;		/* lookup for format code */
	int	log2drecl;		/* log2 of data record length */
	double	samplerate;		/* sample rate, Hz */
	double	clock_tolerance;	/* max clock drift tol */
	/*int	number_ch_comments;*/	/* number of channel comments */
	int	reserved1;		/* reserved 4 byte field */
	char	*channel_flag;		/* channel flags */
	char	*start;			/* start effective date */
	char	*end;			/* end effective date */
	char	*update;		/* update flag */
	struct	response *response_head;	/* first response */
	struct	response *response_tail;	/* last response */
	struct	type59 *type59_head; 		/* first comment */
	struct	type59 *type59_tail;		/* last comment */
	struct	type52 *channel_update;		/* ptr to update */
	struct	type52 *next;			/* ptr to next type52 entry */
};

struct	response/* response structure */
{
	char	type;		/* response type */
	union
	{
		struct	type53 *type53;	/* type 053 blockette */
		struct	type54 *type54;	/* type 054 blockette */
		struct	type55 *type55;	/* type 055 blockette */
		struct	type56 *type56;	/* type 056 blockette */
		struct	type57 *type57;	/** 001 **/ /* type 057 blockette */
		struct	type58 *type58;	/* type 058 blockette */
		struct	type60 *type60;	/* type 060 blockette */
		struct	type61 *type61;	/* type 060 blockette */
		struct	type62 *type62;	/* type 062 blockette */
	} ptr;				/* ptr to response type */
	struct		response *next;	/* ptr to next response entry */
};

struct	type53sub	/* used in struct type53 */
{
	double	real;	/* real part */
	double	imag;	/* imaginary part */
	double	real_error;		/* real error */
	double	imag_error;		/* imaginary error */
};
	
struct	type53	/* poles and zeroes rspns */
{
	char	*transfer;	/* transfer function type */
	int	stage;		/* stage sequence number */
	int	input_units_code;	/* response in units lookup */
	int	output_units_code;	/* response out units lookup */
	double	ao_norm;		/* AO normalization factor */
	double	norm_freq;		/* normalization frequency */
	int	number_zeroes;		/* number of complex zeroes */
	struct	type53sub *zero;	/* struct for a complex zero */
	int	number_poles;		/* number of complex poles */
	struct	type53sub *pole;	/* struct for a complex pole */
	struct	type53 *next;		/* ptr to next type53 entry */
};

struct	type54sub	/* used in struct type54 */
{
	double	coefficient;	/* coefficient value */
	double	error;		/* error value */
};

struct	type54				/* coefficients response */
{
	char	*transfer;		/* transfer function type */
	int	stage;			/* stage sequence number */
	int	input_units_code;	/* response in units lookup */
	int	output_units_code;	/* response out units lookup */
	int	number_numerators;	/* number of numerators */
	struct	type54sub *numerator;	/* struct of numerator */
	int	number_denominators;	/* number of denominators */
	struct	type54sub *denominator;	/* struct for denominator */
	struct	type54 *next;		/* ptr to next type54 entry */
};

struct	type55sub	/* used in struct type55 */
{
	double	frequency;		/* frequency */
	double	amplitude;		/* amplitude */
	double	amplitude_error;	/* amplitude error */
	double	phase;			/* phase */
	double	phase_error;		/* phase error */
};

struct	type55			/* listed response */
{
	int	stage;			/* cascade sequence number */
	int	input_units_code;	/* response in units lookup */
	int	output_units_code;	/* response out units lookup */
	int	number_responses;	/* number of responses */
	struct	type55sub *response;	/* struct of listed response */
	struct	type55 *next;		/* ptr to next type55 entry */
};

struct type56sub			/* used in struct type56 */
{
	double	frequency;	/* corner frequency */
	double	slope;		/* slope, db/decade */
};

struct	type56		/* "generic" response */
{			/* (corner freq/slope rspns) */
	int stage;			/* cascade sequence number */
	int input_units_code;		/* response in units lookup */
	int output_units_code;		/* response out units lookup */
	int number_corners;		/* number of corners */
	struct type56sub *corner;	/* struct of corner */
	struct type56 *next;		/* ptr to next type56 entry */
};

struct type57				/* decimation blockette */
{
	int	stage;			/* stage sequence number */
	double	input_sample_rate;	/* input sample rate */
	int	decimation_factor;	/* decimation factor */
	int	decimation_offset;	/* decimation offset */
	double  delay;			/* estimated delay */
	double	correction;		/* correction applied */
	struct	type57 *next;		/** 001 **/ /* ptr to next type57 entry */
};

struct	type58sub			/* used in struct type58 */
{
	double	sensitivity;		/* sensitivity */
	double	frequency;		/* frequency of sensitivity */
	char	*time;			/* time of calibration */
};

struct	type58				/* channel sensitivity */
{
	int stage;			/* cascade sequence number */
	double	sensitivity;		/* sensitivity, cts/unit */
	double	frequency;		/* frequency of sensitivity */
	int	number_calibrations;		/* number of calibrations */
	struct	type58sub *calibration;		/* struct of calibration */
	struct	type58 *next;			/* ptr to next type58 entry */
};

struct	type59			/* channel comment */
{
	char *start;			/* beginning effective time */
	char *end;			/* ending effective time */
	int comment_code;		/* lookup for comment */
	long int level_code;		/* lookup for level */
	struct type59 *next;		/* ptr to next type59 entry */
};

struct	type60sub1			/* used in struct type60 */
{
	int value;                      /* stage number */
	int number_responses;		/* number of responses */
	struct type60sub2 *response;	/* struct for reference*/
};

struct type60sub2	/* used in struct type60 */
{
	int	reference;
};

struct	type60		/* coefficients response */
{
	int number_stages;	/* number of stages     */
	struct	type60sub1 *stage;	/* struct of values */
	struct	type60 *next;		/* ptr to next type60 entry */
};
	
struct	type61			/* symmetrical coefficients response */
{
	int stage;		/* stage sequence number */
	char *name;		/* transfer function name */
	char symmetry_code;	/* response symmetry code */
	int input_units_code;	/* response in units lookup */
	int output_units_code;	/* response out units lookup */
	int number_numerators;	/* number of FIR factors */
	double *numerator;	/* pointer to numerator list*/
	struct type61 *next;	/* ptr to next type44 entry */
};

struct	type62sub		/* coefficient (type62) */
{
	double	coefficient;	/* coefficient value */
	double	error;		/* error value */
};

struct	type62	/* polynomial response */
{
	char	transfer_fct_type;	/* transfer function type */
	int	stage;			/* stage sequence number */
	int	input_units_code;	/* response in units lookup */
	int	output_units_code;	/* response out units lookup */
	char	poly_approx_type;	/* polynomial approximation type */
	char	valid_freq_units;	/* valid frequency units */
	double	lower_valid_freq;	/* lower valid frequency bound */
	double	upper_valid_freq;	/* upper valid frequency bound */
	double	lower_bound_approx;	/* lower bound of approximation */
	double	upper_bound_approx;	/* upper bound of approximation */
	double	max_abs_error;		/* maximum absolute error */
	int	number_coefficients;	/* number of poly. coefficients */
	struct	type62sub *coefficient;		/* pointer to coefficient list */
	struct	type62	*next;		/* ptr to next type62 entry */
};


/*                 +=======================================+                 */
/*=================|      time span header structures      |=================*/
/*                 +=======================================+                 */

struct	type70		/* time span identifier */
{
	char	*flag;			/* time span flag */
	char	*start_of_data;		/* start of data time */
	char	*end_of_data;		/* end of data time */
	struct	type70 *next;		/* ptr to next type70 entry */
};

struct	type71sub	/* used by struct type71 */
{
	double	magnitude;	/* magnitude */
	char	*type;		/* magnitude type */
	int	source_code;	/* lookup for source ref */
};

struct	type71	/* hypocenter identifier */
{
	char *origin_time;		/* event origin time */
	int		source_code;	/* lookup for source ref */
	double		latitude;	/* event latitude */
	double		longitude;	/* event longitude */
	double		depth;		/* event depth */
	int		number_magnitudes;	/* number of magnitudes */
	struct		type71sub *magnitude;	/* struct for a magnitude */
	int		seismic_region;		/* Seismic Region */
	int		seismic_location;	/* Seismic Location */
	char		*region_name;		/* Region Name */
	struct		type72 *type72_head;	/* type 72 list header */
	struct		type72 *type72_tail;	/* type 72 list header */
	struct		type71 *next;		/* ptr to next type71 entry */
};

struct	type72			/* event phase identifier */
{
	char	*station;	/* station name */
	char	*location;	/* location name */
	char	*channel;	/* channel name */
	char	*arrival_time;	/* arrival time of phase */
	double	amplitude;	/* amplitude of phase */
	double	period;		/* period of signal (sec) */
	double	sig2noise;	/* signal to noise ratio */
	char	*phasename;	/* name of phase */
	int	source_code;	/* phase pick source look-up code*/
	char	*network_code;	/* v2.3 network code */
	struct	type72 *next;	/* ptr to next type72 entry */
};

struct	type73sub			/* used by struct type73 */
{
	char	*station;		/* station name */
	char	*location;		/* station location */
	char	*channel;		/* channel name */
	char	*time;			/* time */
	long int sequence_number;	/* sequence number */
	int	subsequence;		/* subsequence number */
};

struct	type73				/* timespan data start index */
{
	int number_datapieces;		/* number of data pieces */
	struct	type73sub *datapiece;	/* struct for data piece */
	struct	type73 *next;		/* ptr to next type73 entry */
};

struct	type74sub	/* used by struct type74 */
{
	char	*time;		/* record start time */
	long int index;		/* index to record */
	int	subindex;	/* subindex number */
};

struct	type74	/* time series index */
{
	char	*station;		/* station name */
	char	*location;		/* station location */
	char	*channel;		/* channel name */
	char	*starttime;		/* start time of time series */
	long int start_index;		/* index to start of data */
	int	start_subindex;		/* start subindex number */
	char	*endtime;		/* end time of time series */
	long int end_index;		/* index to end of data */
	int	end_subindex;		/* end subindex number */
	int	number_accelerators;	/* number of data pieces */
	char	*network_code;		/* v2.3 network code */
	struct	type74sub *accelerator;	/* struct for data piece */
	struct	type74 *next;		/* ptr to next type74 entry */
};

/*                 +=======================================+                 */
/*=================|        Data record structures         |=================*/
/*                 +=======================================+                 */

struct	input_time			/* time from input stream */
{
	unsigned short int year;
	unsigned short int day;
	char hour;
	char minute;
	char second;
	char unused;
	unsigned short int fracsec;
};

struct	input_data_hdr		/* fixed data header */
{
	char station[5];	/* station name */
	char location[2];	/* station location */
	char channel[3];	/* channel name */
	char network[2];	/* network code */
	struct	input_time time;	/* time */
	unsigned short int nsamples;	/* number samples */
	short int sample_rate;		/* sample rate factor */
	short int sample_rate_multiplier;	/* sample rate multiplier */
	char	activity_flags;		/* activity flags */
	char	io_flags;		/* i/o flags */
	char	data_quality_flags;	/* data quality flags */
	char	number_blockettes;	/* # blockettes which follow */
	int number_time_corrections;/* # .0001s time corrections */
	unsigned short int bod;		/* beginning of data */
	unsigned short int bofb;	/* beginning 1st blkt */
};

struct mini_data_hdr 
{
	struct input_data_hdr hdr;
	char blockettes[521];
};

struct	data_hdr /* fixed data header 	*/
{
	char	*station;	/* station name		*/
	char	*location;	/* station location 	*/
	char	*channel;	/* channel name 	*/
	char 	network[3];	/* what else? network 	*/
	struct	time time;	/* time 		*/
	int	nsamples;	/* number samples 	*/
	int	num_mux_chan;   /* Number of mux channels */
	char	mux_chan_name[16];	/* Mux chan names */
	double	sample_rate;		/* sample rate */
	char	activity_flags;		/* activity flags */
	char	io_flags;		/* i/o flags */
	char	data_quality_flags;	/* data quality flags */
	int	number_blockettes;	/* # of blockettes following */
	long int number_time_corrections;/* # .0001s time corrections */
	int	bod;		/* byte # where data starts */
	int	bofb;		/* byte # of first blockette */
};

struct	data_blk_hdr	/* data blockette header */
{
	unsigned short int type;	 /* blockette type */
	unsigned short int next_blk_byte;/* start of next blockette byte number */
};
struct		data_blk_201		/* blockette 201 */
{
	struct	data_blk_hdr hdr;		/* blockette header 		*/
	float	signal_amplitude;		/* */
	float	signal_period;			/* */
	float	backgr_est;			/* background estimate 		*/
 	unsigned char event_detect_flags;	/* */
	unsigned char reserved_byte;		/* */
	struct input_time sig_onset;		/* signal onset time		*/
	unsigned char sig_noise_ratio[6];	/* signal to noise ratio values */
	unsigned char look_back;		/* lookback value 		*/
	unsigned char pick_algo;		/* pick algoritm  		*/
	char detector_name[24];			/* */
};


struct		data_blk_100		/* blockette 100 */
{
	struct	data_blk_hdr hdr;	/* blockette header */
	float	sample_rate;		/* actual sample rate */
	unsigned char flags;		/* flags */
	unsigned char reserved[3];	/* reserved bytes */
};

struct data_blk_1000		/* mini seed */
{
	struct      	data_blk_hdr hdr;
	unsigned char 	encoding_fmt;
	unsigned char 	word_order;
	unsigned char 	rec_length;
	unsigned char 	reserved;
};

struct data_blk_1001		/* data extension blockette */
{
	struct          data_blk_hdr hdr;

	unsigned char   timing_quality;
        unsigned char   usec;
        unsigned char   reserved;
        unsigned char   frame_count;

};

struct data_blk_2000
{
	struct data_blk_hdr hdr;

        short blk_length;
        short opaque_offset;
        int   rec_num;
        unsigned char word_order;
        unsigned char data_flags;
        unsigned char number_header_flds;
        char **data_header_flds;
        unsigned char *opaque_buff;
};

/*                 +=======================================+                 */
/*=================|          end of header file           |=================*/
/*                 +=======================================+                 */

/* the next line should be the end of the header file */
#endif

