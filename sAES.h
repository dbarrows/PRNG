// Author:  Dexter Barrows
// SN:      500370093

#include <ctype.h>
#include <stdint.h>

typedef unsigned char byte;
typedef uint16_t word;

int poly_degree(byte poly);
byte gf_reduce_4(byte poly);
byte gf_mult_4(byte poly1, byte poly2);
byte rotate_byte(byte input);
byte s_box_4(byte input);
byte s_box_8(byte input);
byte inv_s_box_4(byte input);
byte inv_s_box_8(byte input);
byte * generate_keys(byte* key);
word encrypt_word(word input, byte* key);
word decrypt_word(word input, byte* key);
word add_key(word input, word key);
word n_substitution(word input);
word inv_n_substitution(word input);
word shift_row(word input);
word mix_column(word input);
word inv_mix_column(word input);