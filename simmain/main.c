#include <assert.h>
#include <stdio.h>

#include "alufunc.h"
#include "cpu.h"
#include "defs.h"
#include "klee/klee.h"
#include "memory.h"
#include "model.h"
#include "sim.h"

// Run microcode lines until the simulation finished
FLAG execute_sim(struct VerificationModel *model, uint32_t max_step_count)
{
    FLAG finished = 0;
    init_model(model);
    model->cpu->microPC = 0;
    uint32_t steps_executed = 0;

    while(!finished && steps_executed <= max_step_count) {
        finished = fetch_current_line(model->cpu)(model);
        steps_executed += 1;
    }
    if(steps_executed >= max_step_count) {
        assert(0 && "Exceeded maximum number of steps");
        return 0;
    }
    else {
        return test_model(model);
    }
}

int main(int argv, char** argc)
{
    // Declare symbolic registers
    struct CPU cpu;
    klee_make_symbolic(&cpu, sizeof(cpu), "CPU");
    struct MainMemory memory;
    klee_make_symbolic(&memory, sizeof(memory), "Memory");
    struct VerificationModel model;
    model.cpu = &cpu;
    model.main_memory = &memory;
    execute_sim(&model, 1<<20);
    return 0;
}