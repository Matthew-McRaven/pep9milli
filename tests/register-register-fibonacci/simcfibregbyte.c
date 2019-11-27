#include "sim.h"

#include "alufunc.h"
#include "cpu.h"
#include "klee/klee.h"
#include "model.h"

#include <stdlib.h>


FLAG clearX(struct VerificationModel* model);
FLAG stop(struct VerificationModel* model);
FLAG f00(struct VerificationModel* model);
FLAG f01(struct VerificationModel* model);
FLAG f02(struct VerificationModel* model);
FLAG f03(struct VerificationModel* model);
FLAG f04(struct VerificationModel* model);
FLAG f05(struct VerificationModel* model);
FLAG f06(struct VerificationModel* model);
FLAG f07(struct VerificationModel* model);
FLAG f08(struct VerificationModel* model);
FLAG f09(struct VerificationModel* model);
FLAG f10(struct VerificationModel* model);
FLAG f11(struct VerificationModel* model);
FLAG f12(struct VerificationModel* model);
FLAG f13(struct VerificationModel* model);
FLAG dummy(struct VerificationModel* model);
FLAG dummy(struct VerificationModel* model);
FLAG dummy(struct VerificationModel* model);
FLAG c00(struct VerificationModel* model);
FLAG c01(struct VerificationModel* model);
FLAG c02(struct VerificationModel* model);
FLAG c03(struct VerificationModel* model);
FLAG c04(struct VerificationModel* model);
FLAG c05(struct VerificationModel* model);
FLAG c06(struct VerificationModel* model);
FLAG c07(struct VerificationModel* model);
FLAG c08(struct VerificationModel* model);
FLAG c09(struct VerificationModel* model);
FLAG c10(struct VerificationModel* model);
FLAG c11(struct VerificationModel* model);
FLAG c12(struct VerificationModel* model);
FLAG c13(struct VerificationModel* model);


static MicrocodeLine microcodeTable[] = 
{
    clearX, //00
    f00, //01
    f01, //02
    f02, //03
    f03, //04
    f04, //05
    f05, //06
    f06, //07
    f07, //08
    f08, //09
    f09, //10
    f10, //11
    f11, //12
    f12, //13
    f13, //14
    stop, //15
    c00, //16
    c01, //17
    c02, //18
    c03, //19
    c04, //20
    c05, //21
    c06, //22
    c07, //23
    c08, //24
    c09, //25
    c10, //26
    c11, //27
    c12, //28
    c13, //29
};

void init_model(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    //zeroCPU(cpu);
    cpu->microPC = 0;
}

FLAG test_model(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    FLAG result = 1;
    //printf("%d", cpu->regBank.registers[0]);
    // Assert that the fibonnaci sequence is actually computer in registers 3..16.
    int last2 = 0;
    int last1 = 1;
    for(int it = 2; it <= 13; it++) {
        klee_assert(cpu->regBank.registers[it+3] == last1 + last2);
        int temp = last1 + last2;
        last2 = last1;
        last1 = temp;
    }
    // Assert that either register 2 is the largest register, 
    // or it is reg[reg[2]] <= reg[0] <= reg[reg[2] + 1].
    BYTE target = cpu->regBank.registers[0];
    BYTE idx = cpu->regBank.registers[2] + 3;
    //printf("%d  ", target);
    if(idx != 16 
    && !(cpu->regBank.registers[idx] <= target && target <= cpu->regBank.registers[idx+1]))
    {
        //printf("Target %d\n", target);
        //printf("Index %d\n", idx);
        //printf("At %d\n", cpu->regBank.registers[idx+1]);
        //printf("Next %d\n", cpu->regBank.registers[idx]);
        klee_assert(0);
    }
    return 1;
}

MicrocodeLine fetch_current_line(struct CPU *cpu)
{
    return microcodeTable[cpu->microPC];
}

FLAG clearX(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    cpu->regBank.registers[2] = 0;
    cpu->regBank.registers[17] = 255;
    cpu->microPC = 1;
    return 0;
}
FLAG stop(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    return 1;
}
FLAG f00(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 3;
    cpu->regBank.registers[at] = 0;
    cpu->microPC = at - 1;
    return 0;
}
FLAG f01(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 4;
    cpu->regBank.registers[at] = 1;
    cpu->microPC = at - 1;
    return 0;
}
FLAG f02(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 5;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f03(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 6;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f04(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 7;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f05(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 8;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f06(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 9;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f07(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 10;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f08(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 11;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f09(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 12;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f10(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 13;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f11(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 14;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f12(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 15;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = at-1;
    return 0;
}
FLAG f13(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 16;
    cpu->regBank.registers[at] = byte_add_nocarry(cpu->regBank.registers[at-1], cpu->regBank.registers[at-2]).result;
    cpu->microPC = 16; // Jump to stop
    return 0;
}

FLAG c00(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 3;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    cpu->regBank.registers[2] = 0;
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        //printf("Current %d\n", cpu->regBank.registers[at]);
        //printf("Target %d\n", cpu->regBank.registers[0]);
        //printf("Difference %d\n", byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]).result);
        klee_assert(cpu->regBank.registers[0] == 0);
        cpu->microPC = 15;
    } else {
        klee_assert(cpu->regBank.registers[0] != 0);
        cpu->microPC = at + 14;
    }  
    return 0;
}

FLAG c01(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 4;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        klee_assert(cpu->regBank.registers[0] == 1);
        cpu->microPC = 15;
    } else {
        klee_assert(cpu->regBank.registers[0] > 1);
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c02(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 5;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c03(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 6;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c04(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 7;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c05(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 8;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c06(struct VerificationModel *model) 
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 9;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c07(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 10;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c08(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 11;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c09(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 12;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c10(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 13;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c11(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 14;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c12(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    int at = 15;
    struct ALUByteResult res = byte_sub_nocarry(cpu->regBank.registers[at], cpu->regBank.registers[0]);
    if(cpu->regBank.registers[at] >= cpu->regBank.registers[0]) {
        cpu->microPC = 15;
    } else {
        cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
        cpu->microPC = at + 14;
    }  
    return 0;
}
FLAG c13(struct VerificationModel *model)
{
    // Cache pointer to CPU to reduce code verbosity.
    struct CPU* cpu = model->cpu;

    cpu->regBank.registers[2] = byte_add_nocarry(cpu->regBank.registers[2], 1).result;
    cpu->microPC = 15;
    return 0;
}