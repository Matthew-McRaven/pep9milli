#ifndef MODEL_H
#define MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

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

enum AddressingModes
{
    i_addr, d_addr, n_addr,
    s_addr, sf_addr, x_addr,
    sx_addr, sfx_addr,
    NONE_ADDR
};

// Which addressing mode is associated with a particular instruction specifier
enum AddressingModes instr_addr_mode[256];

#ifdef __cplusplus
}
#endif

#endif