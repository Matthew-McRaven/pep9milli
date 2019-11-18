#ifndef DEFS_H
#define DEFS_H
#include <stdint.h>

// Define the size of a memory address in Pep/9
typedef uint8_t ADDRESS;
// Define twice the size of a word in Pep/9
typedef uint32_t DWORD;
// Define the size of memory word in Pep/9
typedef uint16_t WORD;
typedef uint8_t BYTE;
typedef uint8_t FLAG;
// Define a type to represent a register number
typedef  int8_t REGNUM;
typedef uint16_t MCAddress;

#endif