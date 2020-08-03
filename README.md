# lz77-compressor
A quick attempt at creating the lz77 compressor in C++

This compressor was made to compress level data so those fit better inside an NES ROM.
Files compressed with this tool will have an `.lz77` extension. 

My next step is to write a decompressor in 6502 ASM for my NES homebrew.
It can be used for NES/SNES homebrew purposes.

PROVIDED "AS-IS" and "WITH ALL FAULTS".

THIS DOES NOT COME WITH A DECOMPRESSOR, READ THE DISCLAIMER AND DO NOT USE AS A TOOL OR FOR ANY KIND OF IMPORTANT/NECESSARY/USEFUL/MEANINGFUL
OR OTHERWISE PERSONAL/IMPERSONAL DATA. YOU HAVE BEEN WARNED.

DISCLAIMER
------------------------------------------------
```
DISCLAIMER: This "lz77-compressor" is provided by baboomerang (the writer & provider of this software)\
"as is" and "with all faults." baboomerang (the writer & provider of this software)\
makes no representations or warranties of any kind concerning the safety,\
suitability, lack of viruses, inaccuracies, typographical errors, or other harmful\
components of this "lz77-compressor". There are inherent dangers in the use of any software,\
and you are solely responsible for determining whether this "lz77-compressor" is compatible \
with your equipment and other software installed on your equipment. You are also solely \
responsible for the protection of your equipment and backup of your data, and\
baboomerang (the writer & provider of this software) will not be liable for any damages\
you may suffer in connection with using, modifying, or distributing this "lz77-compressor" or any part of it.
```
### HOWTO INSTALL: Compile the cpp code and figure it out for your system.

### Usage
This algorithm doesnt perform too well with very small data.
```
Usage: $ ./lz77 <file>

example: $ ./lz77 case1.hex
Previous size: 13 Compressed size: 18
Results saved to ./case1.hex.lz77

example2: $ ./lz77 titlescreen.bin
Previous size: 960 Compressed size: 516
Results saved to ./titlescreen.bin.lz77
```

## FILE COMPARISON
```
$ xxd case1.hex
00000000: 6261 6e61 6e61 6261 6361 6e61 0a         bananabacana.
$ xxd case1.hex.lz77
00000000: 0000 6200 0061 0000 6e02 0261 0602 6306  ..b..a..n..a..c.
00000010: 030a                                     ..
```
