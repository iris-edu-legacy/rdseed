/*===========================================================================*/
/* SEED reader     |              decode_ddl               |     decode data */
/*===========================================================================*/
/*
        Name:           decode_ddl
        Purpose:        translate SEED tape data from DDL data format descriptor keys.                                numbers
        Usage:          void decode_ddl ();
                                char *data_ptr;
                                int nsamples;
                        decode_ddl (data_ptr, nsamples, index, type30);
        Input:          data_ptr - pointer to beginning of seismic data in a SEED file
								nsamples - number of samples
								index - index of next entry into output array
								type30 - pointer to type30 ddl descriptor.
 
        Output:         an array of numbers (seismic data)
        Externals:      none
        Warnings:       none
        Errors:         none
        Called by:      process_data
        Calls to:       none
        Language:       C, hopefully ANSI standard
        Author:         Allen Nance
        Revisions:      1/1/94  Allen Nance  Initial preliminary release 0.9
*/

#include "rdseed.h"
#include "ddl.h"
 
static char *blk_data_ptr;
static char *cur_data_ptr;
static union
	{
	unsigned char  byte[32];
	unsigned long  word[8];
	} copy_buffer;
 
static int input_bit;
static int copy_width;		/* copy buffer bit width */
static int copy_bit;		/* copy buffer bit position */
static int copy_order;
 
static unsigned int extract_accum;
static unsigned int extract_width;
static          int extract_samples;
static double	extract_float;
static double	extract_exponent;
 
static          int mult_chan;
static          int inter_flag;
static          int inter_size;
static          int discard;
static		int demux;
 
static          int mantissa;
static          int hidden_mantissa;
static          int exponent;
 
static          int forward_constant;
static          int forward_type;
static          int forward_flag;
static          int reverse_constant;
static          int reverse_type;
static          int reverse_flag;
 
static          int P_flag;
static          int compress_count;
static          int compress_code[32];
 
/* get bits from unsigned long word */

unsigned char get_bit(in, position, width)
unsigned char *in;
int position, width;
{

	int i, j;
	unsigned char c;
 
	i = width - (position+1);
	j = i>>3;
	i = 8 - ((i&7)+1);
 
	c = (in[j]>>i)&1;
 
	return(c);
}
 
char *d_W(in, prim)
char *in;
struct prim_struct *prim;
{
	int i,j,num_bytes;
 
	for (i=0;i<8;i++) copy_buffer.word[i] = 0;
	copy_width = 0;
	extract_accum = 0;
	extract_width = 0;
 
	if (input_bit)
		{
		input_bit = 0;
		in++;
		}
 
	num_bytes = prim->data.ext_byte.num_bytes;
 
	for (i=0; i<num_bytes; i++,j++)
		{
		copy_buffer.byte[i] = in[prim->data.ext_byte.order[i]];
		copy_width += 8;
		}
	copy_bit = copy_width-1;
 
/*	discard = FALSE; */
	return(in+num_bytes);
}
		
char *d_B(in, prim)
char *in;
struct prim_struct *prim;
{
	union
		{
		unsigned char b[2];
		unsigned short  w;
		} t;
	union
		{
		unsigned char  byte[32];
		unsigned long  word[8];
		} temp_buffer;
 
	unsigned int i,j,k,l,p;
	char c;
 
	for (i=0;i<8;i++) temp_buffer.word[i] = 0;
	for (i=0;i<8;i++) copy_buffer.word[i] = 0;
	copy_width = prim->data.ext_bits.num_bits;
	copy_bit = copy_width-1;
	extract_accum = 0;
	extract_width = 0;
 
	t.w = 0;
	t.b[1] = *in;
	t.w = t.w << input_bit;
 
	i = j = 0;
	while (i < copy_width)
		{
		t.w = t.w << 1;
		input_bit += 1;
		if (input_bit >= 8)
			{
			input_bit = 0;
			in++;
			temp_buffer.byte[j++] = t.b[0];
			t.b[1] = *in;
			}
		i += 1;
		}
	temp_buffer.byte[j++] = t.b[0];
 
	/* now reorder bits into copy buffer */
	p = 7;
	j = 0;
	for (i=0; i<prim->data.ext_bits.num_order; i++)
		{
		if ( *(prim->data.ext_bits.order2 + i) < 0)
			*(prim->data.ext_bits.order2 + i) = *(prim->data.ext_bits.order1 + i);
		if ( *(prim->data.ext_bits.order1 + i) > *(prim->data.ext_bits.order2 + i))
			{
			k = *(prim->data.ext_bits.order1 + i);
			l = *(prim->data.ext_bits.order2 + i);
			}
		else
			{
			k = *(prim->data.ext_bits.order2 + i);
			l = *(prim->data.ext_bits.order1 + i);
			}
		while (k >= l)
			{
			c = get_bit(temp_buffer.byte, k, copy_width);
			c = c << p;
			copy_buffer.byte[j] |= c;
			p -= 1;
			if (p < 0)
				{
				p = 7;
				j++;
				}
			k -= 1;
			}
		}
 
/*	discard = FALSE; */
	return(in);
}
	
d_D(prim)
struct prim_struct *prim;
{
	int i,j, k;
 
	if (prim->data.ext_data.bits[0] >= 0)
		{
		if (prim->data.ext_data.bits[1] >= 0)
			{
			i = prim->data.ext_data.bits[1];
			j = prim->data.ext_data.bits[0];
			}
		else
			{
			if (copy_order) /* 68000 bit order */
				{
				j = copy_bit;
				i = copy_bit + (prim->data.ext_data.bits[0]-1);
				}
			else /* bit stream order */
				{
				i = copy_bit;
				j = copy_bit - (prim->data.ext_data.bits[0]-1);
				}
			}
		}
	else
		{
		i = copy_width-1;
		j = 0;
		}
 
	k = j;			/* make i the largest */
	if (i < j)
		{
		j = i;
		i = k;
		}
 
	if (copy_order)
		{
		copy_bit = i+1;
		}
	else
		{
		copy_bit = j-1;
		}
 
	if (!discard) extract_width = (i-j)+1;
 
	k = 0;

	while(i>=j)
		{
		k = k<<1;
		k += get_bit(copy_buffer.byte, i, copy_width);
		i -= 1;
		}
 
	if (prim->data.ext_data.code == 0)
		{
		k = k + prim->data.ext_data.offset;
		if (prim->data.ext_data.scale < 0)
			k = k/prim->data.ext_data.scale;
		else
			k = k*(prim->data.ext_data.scale);
		}
	else
		{
		if (prim->data.ext_data.scale < 0)
			k = k/prim->data.ext_data.scale;
		else
			k = k*(prim->data.ext_data.scale);
		k = k + prim->data.ext_data.offset;
		}
 
	if (!discard) extract_accum += k;
/*	discard = FALSE; */
}
 
d_C(prim)
struct prim_struct *prim;
{
	int i;
 
	switch(prim->data.comp_sign.type)
		{
		case 1:
			{
			i = extract_accum;
			i = i<<(32-extract_width);
			i = i>>(32-extract_width);
			i += 1;
			if (prim->data.comp_sign.mult == 1) i = -i;
			extract_accum = i;
			break;
			}
		case 2:
			{
			i = extract_accum;
			i = i<<(32-extract_width);
			i = i>>(32-extract_width);
			if (prim->data.comp_sign.mult == 1) i = -i;
			extract_accum = i;
			break;
			}
		}
/*	discard = FALSE; */
}
 
d_S1(prim)
struct prim_struct *prim;
{
	int i;
 
	i = extract_accum;
	if (get_bit(copy_buffer.byte, prim->data.sign_bit.bit, copy_width))
		{
		i = -i;
		}
	extract_accum = i;
/*	discard = FALSE; */
}
 
d_S2(prim)
struct prim_struct *prim;
{
	int i,j,k,l;
 
	if (prim->data.control_code.start) /* 68000 bit order */
		{
		j = 0 + (prim->data.control_code.bits * prim->data.control_code.skip);
		i = (prim->data.control_code.bits-1) + (prim->data.control_code.bits * prim->data.control_code.skip);
		}
	else /* bit stream order */
		{
		i = copy_bit - (prim->data.control_code.bits * prim->data.control_code.skip);
		j = (copy_bit - (prim->data.control_code.bits-1))
			- (prim->data.control_code.bits * prim->data.control_code.skip);
		}
 
	for (l=0; l<compress_count; l++)
		{
		k = 0;
		while(i>=j)
			{
			k = k<<1;
			k += get_bit(copy_buffer.byte, i, copy_width);
			i -= 1;
			}
		compress_code[l] = k;
 
		if (prim->data.control_code.start) /* 68000 bit order */
			{
			j += prim->data.control_code.bits;
			i += prim->data.control_code.bits * 2;
			}
		else /* bit stream order */
			{
			j -= prim->data.control_code.bits;
			}
		}
}
 
d_A(prim)
struct prim_struct *prim;
{
	int i;
 
	i = (int) extract_accum;
	i += prim->data.bias_sign.bias;
	extract_accum = (unsigned int) i;
/*	discard = FALSE; */
}
 
d_O(prim)
struct prim_struct *prim;
{
	copy_order = prim->data.bit_order.type;
/*	discard = FALSE; */
}
 
d_J(prim)
struct prim_struct *prim;
{
	copy_bit = prim->data.bit_direction.bit;
/*	discard = FALSE; */
}
 
d_M(prim)
struct prim_struct *prim;
{
	mult_chan = prim->data.multiplexed.code;
/*	discard = FALSE; */
}
 
d_I(prim)
struct prim_struct *prim;
{
	inter_flag = prim->data.interleave.flag;
	inter_size = prim->data.interleave.size;
/*	discard = FALSE; */
}
 
d_F(prim)
struct prim_struct *prim;
{
	forward_type = prim->data.forward_constant.type;
	forward_flag = TRUE;
 
	if (reverse_flag)
		{
		reverse_constant = extract_accum;
		reverse_flag = FALSE;
		}
}
 
d_R(prim)
struct prim_struct *prim;
{
	reverse_type = prim->data.reverse_constant.type;
	reverse_flag = TRUE;
 
	if (forward_flag)
		{
		forward_constant = extract_accum;
		forward_flag = FALSE;
		}
}
 
d_P(prim)
struct prim_struct *prim;
{
	int i,j,num_bytes;
  
	extract_float = mantissa;
	for (i=0; i<prim->data.gain_code.num_codes; i++)
		{
		if (exponent == prim->data.gain_code.code[i])
			{
			if (prim->data.gain_code.mult[i] >= 0)
				extract_float = extract_float*prim->data.gain_code.mult[i];
			else
				extract_float = extract_float/(-prim->data.gain_code.mult[i]);

			break;
			}
		}

/*	discard = FALSE; */
	return;
}

d_E(prim)
struct prim_struct *prim;
{
/* calculate sample from mantissa and exponent */
	extract_exponent = pow ((double) prim->data.exponent.base, (double) ((prim->data.exponent.mult*(exponent+prim->data.exponent.bias))+prim->data.exponent.offset)  );

	extract_float = (double) mantissa * extract_exponent;

/*	discard = FALSE; */
	return;
}

d_Z(prim)
struct prim_struct *prim;
{
	if ((exponent == prim->data.zero_code.exponent) &&
	    (mantissa == prim->data.zero_code.mantissa))
		extract_float = 0.0;

/*	discard = FALSE; */
	return;
}

d_H(prim)
struct prim_struct *prim;
{
	extract_float += (double) hidden_mantissa * extract_exponent;

/*	discard = FALSE; */
	return;
}

void decode_key_int(head)
struct prim_struct *head;
{
	struct prim_struct *prim;
	int i;
 
	extract_accum = 0;
	extract_width = 0;

	prim = head;
	while (prim != NULL)
		{
		switch(prim->type)
			{
			case 'W': cur_data_ptr = d_W(cur_data_ptr, prim); break;
			case 'B': cur_data_ptr = d_B(cur_data_ptr, prim); break;
			case 'D': d_D(prim); break;
			case 'C': d_C(prim); break;
			case 'S': d_S1(prim); break;
			case 'A': d_A(prim); break;
			case 'X': discard = TRUE; break;
			case 'O': d_O(prim); break;
			case 'J': d_J(prim); break;
			case 'M': d_M(prim); break;
			case 'I': d_I(prim); break;
			case 'Y':
				{
				for (i=0; i<prim->data.repeat_fields.count; i++)
					decode_key_int(prim->next);
				return;
				}
			}
		prim = prim->next;
		}
	if (!discard)
		{
		mantissa = extract_accum;		/* make signed */
		*(seismic_data_ptr+demux) = mantissa;
		seismic_data_ptr += 1;
		}
	discard = FALSE;
}

void decode_int(head)
struct prim_struct *head;
{
	struct prim_struct *prim;
	int i;
 
	extract_accum = 0;
	extract_width = 0;

	prim = head;
	while (prim != NULL)
		{
		switch(prim->type)
			{
			case 'W': cur_data_ptr = d_W(cur_data_ptr, prim); break;
			case 'B': cur_data_ptr = d_B(cur_data_ptr, prim); break;
			case 'D': d_D(prim); break;
			case 'C': d_C(prim); break;
			case 'S': d_S1(prim); break;
			case 'A': d_A(prim); break;
			case 'X': discard = TRUE; break;
			case 'O': d_O(prim); break;
			case 'J': d_J(prim); break;
			case 'M': d_M(prim); break;
			case 'I': d_I(prim); break;
			}
		prim = prim->next;
		}
	discard = FALSE;
}

void decode_key_float(head)
struct prim_struct *head;
{
	struct prim_struct *prim;
	int i;
 
	extract_accum = 0;
	extract_width = 0;

	prim = head;
	while (prim != NULL)
		{
		switch(prim->type)
			{
			case 'W': cur_data_ptr = d_W(cur_data_ptr, prim); break;
			case 'B': cur_data_ptr = d_B(cur_data_ptr, prim); break;
			case 'D': d_D(prim); break;
			case 'C': d_C(prim); break;
			case 'S': d_S1(prim); break;
			case 'A': d_A(prim); break;
			case 'X': discard = TRUE; break;
			case 'O': d_O(prim); break;
			case 'J': d_J(prim); break;
			case 'E': d_E(prim); break;
			case 'P': d_P(prim); break;
			case 'Y':
				{
				extract_accum = 0;
				extract_width = 0;

				for (i=0; i<prim->data.repeat_fields.count; i++)
					decode_key_float(prim->next);
				return;
				}
			}
		prim = prim->next;
		}
	if (!discard)
		{
		*(seismic_data_ptr+demux) = (float) extract_float;
		seismic_data_ptr += 1;
		}
	discard = FALSE;
}
  
void decode_key_cmp1(head)
struct prim_struct *head;
{
	struct prim_struct *prim;
	int i;
 
	reverse_flag = FALSE;
	forward_flag = FALSE;
	prim = head;
	while (prim != NULL)
		{
		switch(prim->type)
			{
			case 'W': cur_data_ptr = d_W(cur_data_ptr, prim); break;
			case 'B': cur_data_ptr = d_B(cur_data_ptr, prim); break;
			case 'D': d_D(prim); break;
			case 'C': d_C(prim); break;
			case 'S': d_S1(prim); break;
			case 'A': d_A(prim); break;
			case 'X': discard = TRUE; break;
			case 'O': d_O(prim); break;
			case 'J': d_J(prim); break;
			case 'F': d_F(prim); break;
			case 'R': d_R(prim); break;
			case 'P':
				{
				cur_data_ptr = blk_data_ptr + prim->data.gain_code.code[0];
				break;
				}
			}
		prim = prim->next;
		}
 
	if (reverse_flag)
		{
		reverse_constant = extract_accum;
		reverse_flag = FALSE;
		}
 
	if (forward_flag)
		{
		forward_constant = extract_accum;
		forward_flag = FALSE;
		}
	discard = FALSE;
}
 
void decode_key_cmp2(head)
struct prim_struct *head;
{
	struct prim_struct *prim;
	int i;
 
	prim = head;
	while (prim != NULL)
		{
		switch(prim->type)
			{
			case 'W': cur_data_ptr = d_W(cur_data_ptr, prim); break;
			case 'B': cur_data_ptr = d_B(cur_data_ptr, prim); break;
			case 'X': discard = TRUE; break;
			case 'O': d_O(prim); break;
			case 'J': d_J(prim); break;
			case 'S': d_S2(prim); break;
			case 'N':
				compress_count = prim->data.control_number.number;
				break;
			case 'P':
				{
				if (!P_flag) /* only do this the first time */
					{
					cur_data_ptr = blk_data_ptr + prim->data.gain_code.code[0];
					P_flag = TRUE;
					}
				break;
				}
			}
		prim = prim->next;
		}
	discard = FALSE;
}
 
void decode_key_cmp3(head)
struct prim_struct *head;
{
	struct prim_struct *prim;
	int i;
 
	prim = head;
	while (prim != NULL)
		{
		switch(prim->type)
			{
			case 'W': cur_data_ptr = d_W(cur_data_ptr, prim); break;
			case 'B': cur_data_ptr = d_B(cur_data_ptr, prim); break;
			case 'D': d_D(prim); break;
			case 'C': d_C(prim); break;
			case 'S': d_S1(prim); break;
			case 'A': d_A(prim); break;
			case 'X': discard = TRUE; break;
			case 'O': d_O(prim); break;
			case 'J': d_J(prim); break;
			case 'N':
				if (prim->data.control_number.number != 0)
					{
					i = extract_accum;		/* make signed */
					if (extract_samples == 0) forward_constant = forward_constant - i;
					forward_constant += i;
					*seismic_data_ptr = forward_constant;
					seismic_data_ptr += 1;
					extract_samples++;
					}
				break;
			case 'Y':
				{
				extract_accum = 0;
				extract_width = 0;

				for (i=0; i<prim->data.repeat_fields.count; i++)
					decode_key_cmp3(prim->next);
				return;
				}
			}
		prim = prim->next;
		}
 
	if (!discard)
	{
		i = extract_accum;		/* make signed */
		if (extract_samples == 0) forward_constant = forward_constant - i;
		forward_constant += i;
		*seismic_data_ptr = forward_constant;
		seismic_data_ptr += 1;
		extract_samples++;
	}
	discard = FALSE;
}
 
void decode_key_cmp4(type30, nsamples)
struct type30 *type30;
int nsamples;
{
	struct prim_struct *prim;
	int i,j;
 
	for (i=0; i<compress_count; i++)
		{
		if (extract_samples < nsamples)
			{
			for (j=2; j<type30->number_keys; j++)
				{
				prim = (struct prim_struct *)type30->decoder_key_prim[j];
				if ((prim->type == 'T') && (prim->data.control_key.code==compress_code[i]))
					{
					decode_key_cmp3(prim->next);
					break;
					}
				}
			}
		}
}
 
void decode_ddl (data_ptr, nsamples, index, type30)
char *data_ptr;
int nsamples;
int index;
struct type30 *type30;
{
	int i, j, k;		/* counters */
 
	blk_data_ptr = data_ptr;
	cur_data_ptr = data_ptr;
 
	mult_chan = inter_flag = inter_size = discard = 0;
 
/*                +=======================================+                 */
/*================|   determine family type of decoding   |=================*/
/*                +=======================================+                 */
	switch (type30->family)
		{
		case 0:		/* Integer Family */
			{
			decode_int(type30->decoder_key_prim[0]);

			if (mult_chan > 1) data_hdr->num_mux_chan = mult_chan;
			else data_hdr->num_mux_chan = 1;

			extract_samples = 0;
			while (extract_samples < nsamples)
				{
				for (j = demux = 0;
				     j < data_hdr->num_mux_chan;
				     j++, demux += seis_buffer_length/data_hdr->num_mux_chan)
					{
					decode_key_int(type30->decoder_key_prim[1]);
					seismic_data_ptr -= 1;
					}
				extract_samples++;

				seismic_data_ptr += 1;
				}
			break;
			}
		case 1:		/* Gain Ranged Family */
			{
			decode_int(type30->decoder_key_prim[0]);
 
			if (mult_chan > 1) data_hdr->num_mux_chan = mult_chan;
			else data_hdr->num_mux_chan = 1;

			extract_samples = 0;
			while (extract_samples < nsamples)
				{
				for (j = demux = 0;
				     j < data_hdr->num_mux_chan;
				     j++, demux += seis_buffer_length/data_hdr->num_mux_chan)
					{
					decode_int(type30->decoder_key_prim[1]);
					mantissa = (int) extract_accum;
					decode_int(type30->decoder_key_prim[2]);
					exponent = (int) extract_accum;
					decode_key_float(type30->decoder_key_prim[3]); 
					seismic_data_ptr -= 1;
					}
				extract_samples++;
				seismic_data_ptr += 1;
				}
			break;
			}
		case 50:		/* Compression Family */
			{
			decode_key_cmp1(type30->decoder_key_prim[0]);
 
			P_flag = FALSE;
			extract_samples = 0;
			while (extract_samples < nsamples)
				{
				compress_count = 0;
				decode_key_cmp2(type30->decoder_key_prim[1]);
				decode_key_cmp4(type30, nsamples);
				}
			break;
			}
		default:
			{
			fprintf(stderr,"ERROR - Invalid DDL family code %d, data block ignored\n",
				type30->family);
			}
		}
}
