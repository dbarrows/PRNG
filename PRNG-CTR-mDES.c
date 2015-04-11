// Author:  Dexter Barrows

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include "mDES.h"

byte generate_IV_from_nonce(int nonce, byte* key);
char* byte_to_bit_string(byte input);
byte convert_char_to_hex(char raw_input);

int main(int argc, char *argv[]) {

    int nonce_flag = 0;
    int nonce;
    int i;

    int length          = atoi(argv[1]);
    char* key_string    = argv[3];
    if (argc > 4) {
        nonce = atoi(argv[4]);
        nonce_flag = 1;
    }

    int num_blocks = ceil( (double)length / 8.0);
    int pad = length - (length/8)*8;

    byte key1 = convert_char_to_hex(key_string[0]);
    byte key2 = convert_char_to_hex(key_string[1]);
    byte key = (key1 << 4) + key2;

    byte* keys = generate_round_keys(key);

    printf("Pad:\t\t%d\n", pad);
    printf("#Blocks:\t%d\n", num_blocks);
    printf("Key:\t\t0x%02X\n", key);

    byte IV;
    if(nonce_flag) {
        IV = generate_IV_from_nonce(nonce, keys);
        printf("Nonce:\t\t%d\n", nonce);
    } else
        IV = 0xA5;

    printf("IV:\t\t0x%02X\n", IV);

    byte* CTR = (byte*)malloc(num_blocks*sizeof(byte));
    for(i = 0; i < num_blocks; i++) {
        CTR[i] = generate_IV_from_nonce( IV++, keys);
        //printf("CTR%d:\t\t0x%02X\n", i,CTR[i]);
    }

    char** bitstream = (char**)malloc(num_blocks*sizeof(char*));
    for(i = 0; i < num_blocks; i++) { 
        byte out = encrypt_block(CTR[i], keys);
        bitstream[i] = byte_to_bit_string(out);
    }
    if(pad > 0)
        bitstream[num_blocks-1][8-pad] = '\0';

    char * outfile = argv[2];
    FILE * ofp = fopen(outfile, "w");
    if(ofp == NULL) {
        printf("Cannot open output file. Exiting...\n");
        exit(1);
    } else {
    for(i = 0; i < num_blocks; i++)
        fprintf(ofp, "%s\n", bitstream[i]);
    }
    fprintf(ofp,"\n");
    fclose(ofp);
}
//generate a word to use as an IV from a 16-bit int - should be relatively random
byte generate_IV_from_nonce(int nonce, byte* keys) {
    byte data = nonce;
    int i;
    for(i = 0; i < 7; i++) {
        if(i%2 == 0)
            data = encrypt_block(data, keys) >> 1;
        else
            data = encrypt_block(data, keys) << 1;
    }
    
    return data;
}

//converts a byte to its string representation in base 2
char* byte_to_bit_string(byte input) {
    char* bitstring = (char*)malloc(9*sizeof(char));

    int i;
    for(i = 0; i < 8; i++) {

        //detect bit value and print representative character
        int bit_flag = ((input << i) & 0x80) >> 7;
        if(bit_flag)                                    
            bitstring[i] = '1';
        else
            bitstring[i] = '0';
    }
    bitstring[8] = '\0';

    return bitstring;
}

byte convert_char_to_hex(char raw_input) {

    //get corresponding hex value from ASCII character
    char input = tolower(raw_input);
    byte output;
    if(input >= '0' && input <= '9')
        output = input - '0';
    else if(input >= 'a' && input <= 'f')
        output = input - 'a' + 10;
    else
        return (byte) input;

    return output;
}




