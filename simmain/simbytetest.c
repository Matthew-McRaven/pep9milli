#include "sim.h"

#include "alufunc.h"
#include "cpu.h"
#include "klee/klee.h"

#include <stdlib.h>

FLAG checkNeg(struct VerificationModel* model);
FLAG invIfNeg(struct VerificationModel* model);
FLAG suba1 (struct VerificationModel* model);
FLAG seta1 (struct VerificationModel* model);
FLAG asla1 (struct VerificationModel* model);
FLAG stop (struct VerificationModel* model);


static MicrocodeLine microcodeTable[] = 
{
    checkNeg, //0
    invIfNeg, //1
    suba1, //2
    seta1, //3
    asla1, //4
    stop //5
};

void init_model(struct VerificationModel* model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    //zeroCPU(cpu);
    cpu->microPC = 0;
}

FLAG test_model(struct VerificationModel* model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    FLAG result = 1;
    //printf("%d", cpu->regBank.registers[0]);
    klee_assert(cpu->regBank.registers[0] == 8);
    return 1;
}

MicrocodeLine fetch_current_line(struct CPU *cpu)
{
    return microcodeTable[cpu->microPC];
}

FLAG stop (struct VerificationModel* model)
{
    return 1;
}

FLAG checkNeg(struct VerificationModel* model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    if(byte_ident(cpu->regBank.registers[0]).NZVC[N]){
        //klee_assert(cpu->regBank.registers[0] >= 128);
        cpu->microPC = 1;
    }
    else {
        //klee_assert(cpu->regBank.registers[0] <= 127);
        cpu->microPC = 2;
    }
    return 0;
}
FLAG invIfNeg(struct VerificationModel* model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    BYTE* target = &(cpu->regBank.registers[0]);
    //klee_assert(cpu->regBank.registers[0] >= 128);
    *target =  byte_and(*target, 127).result;
    //klee_assert(cpu->regBank.registers[0] <= 127);
    cpu->microPC = 2;
    return 0;
}

FLAG suba1 (struct VerificationModel* model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    BYTE* target = &(cpu->regBank.registers[0]);
    //klee_assert(cpu->regBank.registers[0] <= 127);
    *target =  byte_sub_nocarry(*target,1).result;
    if(*target == 0) {
        //klee_assert(cpu->regBank.registers[0] == 0);
        cpu->microPC = 3;
    }
    else {
        //klee_assert(cpu->regBank.registers[0] != 0);
        cpu->microPC = 2;
    }
    return 0;
}
FLAG seta1 (struct VerificationModel* model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    cpu->regBank.registers[0] =  byte_add_nocarry(cpu->regBank.registers[0], 1).result;
    //klee_assert(cpu->regBank.registers[0] == 1);
    cpu->microPC = 4;
    return 0;
}

FLAG asla1 (struct VerificationModel* model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    //klee_assert(cpu->regBank.registers[0] < 8);
    cpu->regBank.registers[0] =  byte_asl(cpu->regBank.registers[0]).result;

    if(cpu->regBank.registers[0] == 8) {
        //klee_assert(cpu->regBank.registers[0] == 8);
        cpu->microPC = 5;
    }
    else {
        cpu->microPC = 4;
    }
    return 0; 
}
