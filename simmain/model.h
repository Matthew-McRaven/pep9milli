#ifndef MODEL_H
#define MODEL_H

#include "cpu.h"
#include "defs.h"
#include "memory.h"

struct VerificationModel 
{
    struct CPU* cpu;
    struct MainMemory* main_memory;
};

// Define a Microcode line as a function that acts on a verification model.
typedef FLAG(*MicrocodeLine)(struct VerificationModel *);

#endif