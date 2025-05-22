#include "bitwise.h"
// #include <stdio.h>


uint8_t readBits(uint8_t byte, size_t index, uint8_t nbits)
{
    roundoffNibble(nbits);
    
    return ((byte >> (index*nbits)) & ((1 << nbits) - 1));
}

uint8_t writeBits(uint8_t byte, uint8_t value, size_t index, uint8_t nbits)
{
    roundoffNibble(nbits);
    
    uint8_t mask = ~(((1 << nbits) - 1) << (index * nbits));
        
    return (byte & mask) | (value << (index * nbits)); // overwrite
}
