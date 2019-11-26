#include "memory.h"

#include "alufunc.h"
#include "cpu.h"
#include "cpufunc.h"
#include "defs.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 

void mem_initialize(struct MainMemory* main_memory)
{
    memset(&(main_memory->memory), 0 , sizeof(main_memory->memory));
}
// Debug methods that allow read, writing exactly one byte
// from memory, ignoring alignment conditions.
void mem_dbg_read_into_mdre(struct CPU* cpu, struct MainMemory* main_memory)
{
    WORD address = getMARWord(cpu);
    cpu->MDRE = main_memory->memory[address];
}
void mem_dbg_read_into_mdro(struct CPU* cpu, struct MainMemory* main_memory)
{
    WORD address = getMARWord(cpu);
    cpu->MDRO = main_memory->memory[address];
}
void mem_dbg_write_mdre(struct CPU* cpu, struct MainMemory* main_memory)
{
    WORD address = getMARWord(cpu);
    main_memory->memory[address] = cpu->MDRE;
}
void mem_dbg_write_mdro(struct CPU* cpu, struct MainMemory* main_memory)
{
    WORD address = getMARWord(cpu);
    main_memory->memory[address] = cpu->MDRO;
}