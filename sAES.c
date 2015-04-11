// Author:  Dexter Barrows

#include "sAES.h"
#include <stdlib.h>

//block encryption algorithm
word encrypt_word(word input, byte* key) {

	//take input and generate keys
	word output = input;

	byte * keys = generate_keys(key);
	word k1 = (keys[0] << 8) + keys[1];
	word k2 = (keys[2] << 8) + keys[3];
	word k3 = (keys[4] << 8) + keys[5];

    //round 1
	output = add_key(output,k1);

	//round 2
	output = n_substitution(output);
	output = shift_row(output);
	output = mix_column(output);
	output = add_key(output,k2);

	//round 3
	output = n_substitution(output);
	output = shift_row(output);
	output = add_key(output, k3);

	return output;
}

//block decryption algorithm
word decrypt_word(word input, byte* key) {

	//take input and generate keys in reverse order
    word output = input;

    byte * keys = generate_keys(key);
	word k3 = (keys[0] << 8) + keys[1];
	word k2 = (keys[2] << 8) + keys[3];
	word k1 = (keys[4] << 8) + keys[5];

    //round 1
	output = add_key(output,k1);

	//round 2
	output = shift_row(output);
	output = inv_n_substitution(output);
	output = add_key(output,k2);
	output = inv_mix_column(output);

	//round 3
	output = shift_row(output);
	output = inv_n_substitution(output);
	output = add_key(output, k3);

	return output;
}

//add key function
word add_key(word input, word key) {
    word output = input^key;
    return output;
}

//nibble substitution function
word n_substitution(word input) {
    byte b1 = (input & 0xff00) >> 8;
    byte b2 = input & 0xff;

    byte b1_sub = s_box_8(b1);
    byte b2_sub = s_box_8(b2);

    word output = (b1_sub << 8) + b2_sub;
    return output;
}

//inverse nibble substitution function
word inv_n_substitution(word input) {
    byte b1 = (input & 0xff00) >> 8;
    byte b2 = input & 0xff;

    byte b1_sub = inv_s_box_8(b1);
    byte b2_sub = inv_s_box_8(b2);

    word output = (b1_sub << 8) + b2_sub;
    return output;
}

//shift second row function
word shift_row(word input) {
    byte row_parts[4];

    int i;
    for(i = 0; i < 4; i++)
        row_parts[i] = ((0xf << 4*i) & input) >> (4*i);

    //swap second row nibbles
    byte temp = row_parts[0];
    row_parts[0] = row_parts[2];
    row_parts[2] = temp;

    word output = 0x0;
    for(i = 0; i < 4; i++)
        output += row_parts[i] << (4*i);

    return output;
}

//mix columns (matrix multiplication) function
word mix_column(word input) {
    byte in[4];
    byte out[4];

    int i;
    for(i = 3; i >= 0; i--)
        in[3-i] = ((0xf << 4*i) & input) >> (4*i);

    //perform matrix multiplication mod m(x)
    out[0] = in[0]^gf_mult_4(4,in[1]);
    out[1] = gf_mult_4(4,in[0])^in[1];
    out[2] = in[2]^gf_mult_4(4,in[3]);
    out[3] = gf_mult_4(4,in[2])^in[3];

    word output = 0x0;
    for(i = 3; i >= 0; i--)
        output += out[3-i] << (4*i);

    return output;
}

//inver mix columns (matrix multiplication) function
word inv_mix_column(word input) {
    byte in[4];
    byte out[4];

    int i;
    for(i = 3; i >= 0; i--)
        in[3-i] = ((0xf << 4*i) & input) >> (4*i);

    //perform matrix multiplication mod m(x)
    out[0] = gf_mult_4(9,in[0])^gf_mult_4(2,in[1]);
    out[1] = gf_mult_4(2,in[0])^gf_mult_4(9,in[1]);
    out[2] = gf_mult_4(9,in[2])^gf_mult_4(2,in[3]);
    out[3] = gf_mult_4(2,in[2])^gf_mult_4(9,in[3]);

    word output = 0x0;
    for(i = 3; i >= 0; i--)
        output += out[3-i] << (4*i);

    return output;
}

//returns the polynomial degree of the input byte
int poly_degree(byte poly) {
    int degree = 0;

    while(poly != 0){
        poly = poly >> 1;
        degree++;
    }

    return (degree-1);
}

//performs reduction mod m(x) for use after multiplication
byte gf_reduce_4(byte poly) {

    byte mx = 0x13;
    byte remainder = -1;
    int done = 0;

    int deg_div = 4;

    //polynomial division of input by m(x)
    byte cur_remain = poly;
    while ( 1 ) {
        int deg_rem = poly_degree(cur_remain);
        if(deg_div > deg_rem){
            remainder = cur_remain;
            break;
        }
        else{
            int diff = deg_rem - deg_div;
            cur_remain = cur_remain^(mx << diff);
        }
    }

    return remainder;
}

//performs multiplication of two nibbles mod m(x)
byte gf_mult_4(byte poly1, byte poly2) {
    int i;
    byte result = 0x0;

    //multiply
    for(i = 0; i < 4; i++){
        if ( (0x1 & (poly1 >> i)) == 0x1 )
            result = (result^(poly2 << i));
    }

    //reduce mod m(x)
    byte remainder = gf_reduce_4(result);

    return remainder;
}

//rotates a byte by 4 bits left, equivalent to shift right by 4 bits and swap nibbles
byte rotate_byte(byte input) {
    byte right = input & (0xf);
    byte left = input & (0xf0);

    byte output = (right << 4) + (left >> 4);

    return output;
}

//performs s-box substitution of a nibble
byte s_box_4(byte input) {

	byte sbox[4][4] = 	{{9,4,10,11},
                    	{13,1,8,5},
                    	{6,2,0,3},
                    	{12,14,15,7}};
   
    //get row and column sub-bits
    byte left = (input & 0xc) >> 2;
    byte right = input & 0x3;

    byte output = sbox[left][right];

    return output;
}

//performs inverse s-box substitution of a nibble
byte inv_s_box_4(byte input) {

    byte sbox[4][4] = 	{{10,5,9,11},
                    	{1,7,8,15},
                    	{6,0,2,3},
                    	{12,4,13,14}};

    byte left = (input & 0xc) >> 2;
    byte right = input & 0x3;

    byte output = sbox[left][right];

    return output;
}

//performs s-box substitution of each nibble in a byte (for convenience purposes)
byte s_box_8(byte input) {

    byte left = (input & 0xf0) >> 4;
    byte right = input & 0xf;

    byte left_sub = s_box_4(left);
    byte right_sub = s_box_4(right);

    byte output = (left_sub << 4) + right_sub;

    return output;
}

//performs inverse s-box substitution of each nibble in a byte (for convenience purposes)
byte inv_s_box_8(byte input) {

    byte left = (input & 0xf0) >> 4;
    byte right = input & 0xf;

    byte left_sub = inv_s_box_4(left);
    byte right_sub = inv_s_box_4(right);

    byte output = (left_sub << 4) + right_sub;

    return output;
}

//generates additional round keys from first key (key expansion)
byte * generate_keys(byte* in_key) {

    byte * keys = (byte*)malloc(6*sizeof(byte));

    //Rcon(x) for x in {1,2} values entered instead of calculated for convenience
    byte Rcon1 = 0x80;
    byte Rcon2 = 0x30;

    //from seed key
    keys[0] = (in_key[0] << 4) + in_key[1];
    keys[1] = (in_key[2] << 4) + in_key[3];

    //extended bytes for additional keys
    keys[2] = keys[0]^Rcon1^s_box_8(rotate_byte(keys[1]));
    keys[3] = keys[2]^keys[1];
    keys[4] = keys[2]^Rcon2^s_box_8(rotate_byte(keys[3]));
    keys[5] = keys[4]^keys[3];

    return keys;
}








