#ifndef SIM_H
#define SIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>

#include "cpu.h"
#include "defs.h"
#include "model.h"

// Initialize the CPU to contain sensible values for the verification run.
// Apply any pre-conditions to the model.
// Must also init tables (if used).
// These tables include: is_unary_decoder, addressing_mode_decoder.
void init_model(struct VerificationModel *model);

// Check any post-conditions on the CPU model, and log any
// failures to the console.
FLAG test_model(struct VerificationModel *model);

// Return the line of microcode at the microprogram counter.
// Using a per-simulation method to generate this value
// avoids out-of-bounds memory access encountered in earlier versions.
MicrocodeLine fetch_current_line(struct CPU *cpu);

#ifdef __cplusplus
}
#endif

#endif
