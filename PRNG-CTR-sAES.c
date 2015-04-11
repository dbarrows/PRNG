// Author:  Dexter Barrows

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include "sAES.h"

word generate_IV_from_nonce(int nonce, byte* key);
char* word_to_bit_string(word input);
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

    int num_blocks = ceil( (double)length / 16.0);
    int pad = length - (length/16)*16;

    key_string = argv[3];
    byte * key = (byte*)malloc(4*sizeof(byte));
    for(i = 0; i < 4; i++)
        key[i] = convert_char_to_hex(key_string[i]);
    word key_w = 0x0;
    for(i = 0; i < 4; i++)
        key_w += (key[i] << (4*(3-i)));

    printf("Pad:\t\t%d\n", pad);
    printf("#Blocks:\t%d\n", num_blocks);
    printf("Key:\t\t0x%04X\n", key_w);

    word IV;
    if(nonce_flag) {
        IV = generate_IV_from_nonce(nonce, key);
        printf("Nonce:\t\t%d\n", nonce);
    } else
        IV = 0xA51E;

    printf("IV:\t\t0x%04X\n", IV);

    word* CTR = (word*)malloc(num_blocks*sizeof(word));
    for(i = 0; i < num_blocks; i++) {
        CTR[i] = generate_IV_from_nonce( IV++, key);
        //printf("CTR%d:\t\t0x%04X\n", i,CTR[i]);
    }

    char** bitstream = (char**)malloc(num_blocks*sizeof(char*));
    for(i = 0; i < num_blocks; i++) { 
        word out = encrypt_word(CTR[i], key);
        bitstream[i] = word_to_bit_string(out);
    }
    if(pad > 0)
        bitstream[num_blocks-1][16-pad] = '\0';

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
word generate_IV_from_nonce(int nonce, byte* key) {
    word data = nonce;
    int i;
    for(i = 0; i < 7; i++) {
        if(i%2 == 0)
            data = encrypt_word(data, key) >> 1;
        else
            data = encrypt_word(data, key) << 1;
    }
    
    return data;
}

//converts a byte to its string representation in base 2
char* word_to_bit_string(word input) {
    char* bitstring = (char*)malloc(17*sizeof(char));

    int i;
    for(i = 0; i < 16; i++) {

        //detect bit value and print representative character
        int bit_flag = ((input << i) & 0x8000) >> 15;
        if(bit_flag)                                    
            bitstring[i] = '1';
        else
            bitstring[i] = '0';
    }
    bitstring[16] = '\0';

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




