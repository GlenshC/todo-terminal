#ifndef TODO_BITWISE_H
#define TODO_BITWISE_H

#include "types.h"

/* 
    roundoffNibble(x)               // rounds x to powers of 2 in range [0, 8]
    sizeofBytes(x, nbits)           // get size
    
    getarrByte(arr, index, nbits)   // gets the byte from array at index (writable)
    readarrBit                      // read bit from array at index (readonly)
    writearrBit                     // write bit from array at index
*/


#define roundoffNibble(x) ((x &= 0x0F), (--x), (x |= (x >> 1)), (x |= (x >> 2)), (++x))

#define sizeofBytes(n, nbits) ((size_t)((((n)*(nbits)) + 7) / 8))
#define byteIndex(index, nbits) ((size_t)((((index)*(nbits))+1)/8))

// undefined behavior when nbits = 0 or nbits > 8
#define bitIndex(index, nbits) ((size_t)((index) % (8/(nbits)))) 


#define getByte_arr(arr, index, nbits) (arr[byteIndex((index), (nbits))])
#define readBit_arr(arr, index, nbits) (readBits(getByte_arr((arr), (index), (nbits)), bitIndex((index), (nbits)), (nbits)))
#define writeBit_arr(arr, value, index, nbits) (getByte_arr((arr), (index), (nbits)) = (writeBits(getByte_arr((arr), (index), (nbits)), (value), bitIndex((index), (nbits)), (nbits))))
#define writeBit_ptr(ptr, value, index, nbits) (*(ptr) = (writeBits(*(ptr), (value), bitIndex((index), (nbits)), (nbits))))


uint8_t readBits(uint8_t byte, size_t bit_index, uint8_t nbits);
uint8_t writeBits(uint8_t byte, uint8_t value, size_t bit_index, uint8_t nbits);

#endif //TODO_BITWISE_H