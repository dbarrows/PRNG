Pseudo-random Number Generator
==============================

Generates pseudo-randomly generated numbers.


Example run and data
--------------------

For the mDES implementation:

    $./PRNG-CTR-mDES 100 outfile_mdes 1f 5
    Pad:		4
    #Blocks:	13
    Key:		0x1F
    Nonce:		5
    IV:			0x5E

|Arguments|Description|
|---------|-----------|
|100 			| Number of pseudo-random bits to produce |
|outfile_mdes 	| Name of the file to write the output to |
|1f 			| Encryption key to use in hex |
|5 				| Nonce to use (optional) |

This produces:

    $ cat outfile_mdes
    10110110
    00101110
    01000101
    11111001
    01000101
    00001111
    00000111
    00101001
    11001000
    01011101
    11101000
    00111101
    0101

Notice the truncated last block. This is a product of the use of a block cipher of size 8 (mDES) - the last block has been truncated by 4 bits so the output is exactly 100 bits, as was specified in the input.

For the sAES implementation:

    $ ./PRNG-CTR-sAES 100 outfile_saes 1fe5 5
    Pad:		4
    #Blocks:	7
    Key:		0x1FE5
    Nonce:		5
    IV:			0x32EE

The arguments are:

|Arguments|Description|
|---------|-----------|
|100 			| Number of pseudo-random bits to produce |
|outfile_saes 	| Name of the file to write the output to |
|1fe5 			| Encryption key to use in hex |
|5 				| Nonce to use (optional) |

This produces:

    $ cat outfile_saes
    0100111111001011
    0001101101001010
    0110011010110001
    0100101011100100
    1000011000000111
    0100101110100100
    000100101000

Again, notice the truncated last block. Is has again been shortened to produce exactly 100 bits of output.