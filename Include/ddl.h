struct ext_byte
{
	int num_bytes;
	unsigned int *order;
};
 
struct ext_bits
{
	int num_bits;
	int t;
	int num_order;
	unsigned int *order1;
	unsigned int *order2;
};
 
struct ext_data
{
	int bits[2];
	int scale;
	int code;
	int offset;
};
 
struct comp_sign
{
	int type;
	int mult;
};
 
struct sign_bit
{
	int bit;
	int mult;
};
 
struct bias_sign
{
	int bias;
};
 
struct repeat_fields
{
	int count;
};
 
struct bit_order
{
	int type;
};
 
struct bit_direction
{
	int bit;
};
 
struct multiplexed
{
	int code;
};
 
struct interleave
{
	int flag;
	int size;
};
 
struct gain_code
{
	int num_codes;
	int *code;
	int *mult;
};
 
struct exponent  /* mantissa * base**(mult*(exponent+bias) + offset) */
{
	int base;
	int bias;
	int mult;
	int offset;
};
 
struct hidden_bit
{
	int flag;
};
 
struct zero_code
{
	int exponent;
	int mantissa;
};
 
struct forward_constant
{
	int type;
};
 
struct reverse_constant
{
	int type;
};
 
struct control_code
{
	int bits;
	int start;
	int skip;
};
 
struct control_number
{
	int number;
};
 struct control_key
{
	int code;
};
 
struct sub_control_key
{
	int code;
};
 
struct compress_frame
{
	int length;
};
 
struct prim_struct
{
	char type;
	union
		{
		struct ext_byte ext_byte;
		struct ext_bits ext_bits;
		struct ext_data ext_data;
		struct comp_sign	comp_sign;
		struct sign_bit	sign_bit;
		struct bias_sign	bias_sign;
		struct repeat_fields	repeat_fields;
		struct bit_order	bit_order;
		struct bit_direction	bit_direction;
		struct multiplexed	multiplexed;
		struct interleave	interleave;
		struct gain_code	gain_code;
		struct exponent	exponent;
		struct hidden_bit	hidden_bit;
		struct zero_code	zero_code;
		struct forward_constant	forward_constant;
		struct reverse_constant	reverse_constant;
		struct control_code	control_code;
		struct control_number	control_number;
		struct control_key	control_key;
		struct sub_control_key	sub_control_key;
		struct compress_frame	compress_frame;
		} data;
	struct prim_struct *next;
};
 
struct prim_struct *key[50];
 
