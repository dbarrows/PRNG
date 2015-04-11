typedef unsigned char byte;

byte encrypt_block(byte p, byte* keys);
byte decrypt_block(byte c, byte* keys);
void feistel_round(byte L_in, byte R_in, byte* L_out, byte* R_out, byte key);
byte feistel_round_function(byte input, byte key);
byte feistel_s_box(byte input);
byte feistel_expand(byte input);
byte* generate_round_keys(byte key);
byte PC1(byte key);
byte PC2(byte key);
byte left_wrap_4(byte value);