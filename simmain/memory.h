
#ifndef MEMORY_H
#define MEMORY_H

#include "cpu.h"
#include "cpufunc.h"
#include "defs.h"

struct MainMemory
{
    // Memory section
    BYTE memory[0x10000];
};
// Initialize memory to 0.
void mem_initialize(struct MainMemory* main_memory);
// Debug methods that allow read, writing exactly one byte
// from memory, ignoring alignment conditions.
void mem_dbg_read_into_mdre(struct CPU* cpu, struct MainMemory* main_memory);
void mem_dbg_read_into_mdro(struct CPU* cpu, struct MainMemory* main_memory);
void mem_dbg_write_mdre(struct CPU* cpu, struct MainMemory* main_memory);
void mem_dbg_write_mdro(struct CPU* cpu, struct MainMemory* main_memory);

// Memory read / write operations
//void memory_read_word(struct CPU* cpu, FLAG intoMDRE, FLAG intoMDRO);
//void memory_write_word(struct CPU* cpu);

#endif
