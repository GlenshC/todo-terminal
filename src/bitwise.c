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

uint8_t todo_getBit(uint8_t *arr, size_t index, TodoListByteFields fieldEnum)
{
    uint8_t byte = getByte_arr(arr, index, fieldEnum);
    return readBits(byte, bitIndex(index, fieldEnum), fieldEnum);
}

void todo_writeBit(uint8_t *arr, uint8_t value, size_t index, TodoListByteFields fieldEnum)
{
    uint8_t *byte = &getByte_arr(arr, index, fieldEnum);
    // GC_LOG("index: %llu byteIndex: %llu bitIndex: %llu\n", index, byteIndex(index, fieldEnum), bitIndex(index, fieldEnum));
    writeBit_ptr(byte, value, bitIndex(index, fieldEnum), fieldEnum);
}

