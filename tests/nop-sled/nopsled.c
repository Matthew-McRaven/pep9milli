#include "sim.h"

#include "alufunc.h"
#include "cpu.h"
#include "cpufunc.h"
#include "klee/klee.h"
#include "memory.h"
#include "model.h"

#include <stdlib.h>


FLAG NOP0(struct VerificationModel* model);
FLAG NOP1(struct VerificationModel* model);
FLAG NOP2(struct VerificationModel* model);
FLAG stop(struct VerificationModel* model);

static MicrocodeLine microcodeTable[] = 
{
    NOP0,    //00
    NOP1,    //01
    NOP2,    //02
    stop,    //03
};

void init_model(struct VerificationModel *model)
{
    model->cpu->microPC = 0;
    init_static_regs(model->cpu);
}

FLAG test_model(struct VerificationModel *model)
{
    return 1;
}

MicrocodeLine fetch_current_line(struct CPU *cpu)
{
    return microcodeTable[cpu->microPC];
}

FLAG stop(struct VerificationModel *model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, Stop, 0, 0);
}

FLAG NOP0(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, AUTO_INCR, 1, 1); 
}
FLAG NOP1(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, AUTO_INCR, 2, 2); 
}
FLAG NOP2(struct VerificationModel* model)
{
    // Cache pointer to cpu to save repeated pointer lookups.
    struct CPU* cpu = model->cpu;

    return cpu_update_UPC(cpu, AUTO_INCR, 3, 3); 
}