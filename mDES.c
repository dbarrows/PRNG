// Author:  Dexter Barrows

#include "mDES.h"
#include <stdlib.h>

byte encrypt_block(byte p, byte* keys) {

    //split plaintext data chunks into L and R sides, provide temp storage for between rounds
    byte L = ((0xf0) & p) >> 4;
    byte R = (0xf & p);
    byte *L_temp = (byte*)malloc(sizeof(byte));
    byte *R_temp = (byte*)malloc(sizeof(byte));

    //run data through 16 feistel rounds
    int i;
    for(i = 0; i < 16; i++) {
        feistel_round(L,R,L_temp,R_temp,keys[i]);
        L = *L_temp;
        R = *R_temp;
    }

    byte L_cipher = R << 4;
    byte R_cipher = L;

    byte c = L_cipher + R_cipher;    

    return c;
}

byte decrypt_block(byte c, byte* keys) {

    //split plaintext data chunks into L and R sides, provide temp storage for between rounds
    byte L = ((0xf0) & c) >> 4;
    byte R = (0xf & c);
    byte *L_temp = (byte*)malloc(sizeof(byte));
    byte *R_temp = (byte*)malloc(sizeof(byte));

    //run data through 16 feistel rounds
    int i;
    for(i = 15; i >= 0; i--) {
        feistel_round(L,R,L_temp,R_temp,keys[i]);
        L = *L_temp;
        R = *R_temp;
    }

    byte L_cipher = R << 4;
    byte R_cipher = L;

    byte p = L_cipher + R_cipher;

    return p;
}

void feistel_round(byte L_in, byte R_in, byte* L_out, byte* R_out, byte key) {

    //get next L and R halves
    *L_out = R_in;
    *R_out = L_in^feistel_round_function(R_in,key);

}

byte feistel_round_function(byte input, byte key) {

    //perform feistel function operations
    byte expanded_input = feistel_expand(input);
    byte s_input        = key^expanded_input;
    byte s_output       = feistel_s_box(s_input);
    byte output         = left_wrap_4(s_output);

    return output;
}

byte feistel_s_box(byte input) {

    byte output;
    int S[4][16] = {{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
                    {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
                    {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
                    {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}};

    //get row and column data from input
    byte row1   = ((1<<5) & input) >> 4;
    byte row2   = (1 & input);
    byte row    = row1 + row2;

    byte col = ((0xf<<1) & input) >> 1;

    //get corresponding element from S-box
    output = S[row][col];

    return output;
}

byte feistel_expand(byte input) {

    //map for expansion
    int map[6]  = {3,0,1,2,3,1};
    byte sum    = 0;
    int i;

    //get elements from input according to map and place into output
    for(i = 0; i < 6; i++) {
        int shift = 3 - map[i];
        byte bit_flag = ( (1 << shift) & input ) >> shift;
        sum += (bit_flag << (5-i));
    }
    
    return sum;
}

byte* generate_round_keys(byte key) {

    //allocate storage and hold shift amounts for each round's key generation
    byte key_per1 = PC1(key);
    byte* keys = (byte*)malloc(16*sizeof(byte));
    int shifts[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

    //split key into halves
    byte c = (0xf0 & key_per1) >> 4;
    byte d = (0x0f & key_per1);

    byte c_temp = c;
    byte d_temp = d;
    byte key_temp;

    //shift, get sub-elements from halves, and permute
    int i,j;
    for(i = 0; i < 16; i++) {
        for(j = 0; j < shifts[i]; j++){
            c_temp = left_wrap_4(c_temp);
            d_temp = left_wrap_4(d_temp);
        }
        
        byte c_sub = (0x7 & c_temp) << 3;
        byte d_sub = 0x7 & d_temp;

        keys[i] = PC2(c_sub + d_sub);
    }

    return keys;
}

byte PC1(byte key) {

    //shift elements with wrap according to alogrithm
    int carry_flag = ((1<<7) & key) >> 7;
    byte out = key << 1;

    if(carry_flag)
        out ^= (0x1);

    return out;
}

byte PC2(byte key){

    int index[6]    = {1,2,3,4,5,6};
    byte sum        = 0;
    int i;

    //reverses order according to map
    for(i = 0; i < 6; i++) {
        int shift = index[i]-1;
        byte bit_flag = ( (1 << shift) & key ) >> shift;
        sum += (bit_flag << (5-i));
    }
    
    return sum;
}

byte left_wrap_4(byte value) {

    //performs 4-but shift with wrap
    byte out = value << 1;
    int carry_flag = ((1<<4) & out) >> 4;

    if(carry_flag)
        out ^= (0x11);

    return out;
}