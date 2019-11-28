# pep9milli
/Pep9Milli/ is an attempt to verify the correctness of Pep/9’s microcoded CPU.
It uses a symbolic execution tool, /Klee/ to evaluate CPU state symbolically.By using symbolic execution instead of proof-based verification, the burden on the user is lightened.The /Pep9Milli/ project consists of three main components: millicode assembly tools, a verification framework, and a set of verification tests.

# Background
Following is a brief summary of the research accompanying the /Pep9Milli/ project. Usage instructions for the project follow in (SECTION).
## Millicode Design
Interposing an additional level between microcode and hardware (often termed /nanocode/or /millicode/) allows a more 
structured implementation of microcode.  Since verification with multiple levels of abstraction appears to be easier, verification of the Pep9 microprocessor will involve creation of a new intermediary level of /millicode/. 
Pep/9’s microcode is powerful; it is capable of implementing a Turing-complete assembly language. However, it is difficult to develop and far more difficult to debug—difficulty aggravated by the presence of residual control. Case in point, Pep9Micro’s microcode contains more comment lines than lines of executable microcode. Direct verification of microcode would be difficult, as the verification model would have implement all logic gate “blocks” in the processor. 
Alternatively, a new level of abstraction may be introduced, which will sit between the microcode and the instruction set architecture. Multi-level control systems exist in practice [4], [13], [14] and have been subjects of successful verification attempts. In terms of Pep/9, a new control level termed /millicode/, will be introduced. This millicode will be the primary target of the present verification attempt. Due to the current lack of an automated millicode translation tool, Pep9Micro’s microcode implementation will be translated by hand into millicode, which will in turn be verified using symbolic execution tools. 
The goal of millicode is to abstract away complex details of the logic gate implementation, while still retaining the same level of expressiveness and control. 
Each millicode instruction encodes one action, an optional symbolic identifier, and a branch specifying which millicode instruction will follow. The action is either a register-register computation, a load from memory, or a store to memory. Actions can have side effects, such as modifying status flags. To perform a sequence of actions, multiple lines must be used. Millicode sacrifices one power of microcode, which is the ability to perform multiple non-conflicting computations at once. The millicode language is augmented with facilities to encode assertions and assumptions, allowing a millicode programmer to “guide” the verification tool. 
Using a milli-assembler, a millicode specification can be converted to a microcode. For the purposes of verification, the milli-assembler also emits a \C\ program representing the millicode, which is fed into Klee.
At this time, the milli-assembler is a work in progress, and thus it must be translated by hand to \C\.

# Getting Started
In order to run this toolset, you need to be running Ubuntu 18.04 or newer (16.04 may work, but it was not been tested). Included in the repository is a `build-klee.sh` script, which installs the the main verification tool /Klee/, as well as its dependencies.
The executable files generated from this process are copied into `usr/local/lib` and `usr/local/bin`. While the dependencies will most likely not interfere with any other programs, the build process has only been tested on fresh Ubuntu installs, so your mileage may vary.
# Verification Environment
The verification environment, located in `simmain` contains the interface available to a program-under verification. The environment details multiple levels of abstraction, which are as follows:
* CPU data type primitives. These are describe the data types and storage locations for values in the Pep9 CPU.
    * Located in `cpu.h` and `defs.h`.
* CPU primitive operations. These operations perform simple arithmetic/logic operations on BYTE and WORD quantities. They must be chained together to perform an equivalent operation to a single line of microcode.
    * Located in `cpu.h` and `alufunc.h`
* Complex environment operations. These operations chain together multiple CPU or memory primitives, effectively encapsulating one or multiple microcode lines. 
    * Located in `cpufunc.h` and `memory.h`
All operations performed by the CPU can be encoded at the environment level, and it is the preferred way to encode millicode instructions in /C/. 
In order for a program to be verified, it must implement all methods described in `sim.h`. See `tests/*` for examples for how to fulfill this interface.

This code is not meant to be executed or compiled on its own. Executing `make ...` in this directory will most likely fail.
Instead, it is meant to be linked against a test case, and invoked via the test case.
See the documentation on *Test Cases* for more details
# Milli-assembler
At this time, the milli-assembler is a work in progress. Millicode must be translated by hand to microcode and /C/.
# Test Cases
Located under `tests/`, there are multiple verification tests on the Pep9 CPU. The simplest of these tests is `nop-sled`, which is the “hello world” of this verification project. Two sample projects verify the ability of the processor to perform byte and word arithmetic/logic operations—`register-register-fibonacci` and `dynamic-fibonacci`. The `shift-add` example demonstrates self-looping millicode and complex conditional control. Production Pep9 microcode was translated to millicode for `instruction-fetch` and `instr-opr-fetch`, which implement the instruction & operand fetch aspects of the von Neumann cycle. If you want to create your own tests, I recommend duplicating the `dynamic-fibonacci` directory, as it contains the shortest and most powerful example of millicode.

To execute the the verification tool, `cd` to a particular test. Execute `make` followed by `make klee`, which will compile the test case and verification environment before launching a verification run. If you modify any source files in either the verification framework (`simmain`) or the test case, execute `make clean` before executing other make steps. Due to bugs with the build environment, failure to execute `make clean` will prevent verification code from seeing updates made in source files.
# Results
Microcode for the combine instruction-operand fetch unit is attempted in `instr-opr-fetch`. Pre- and post-conditions were  taken from the Pep9 RTL. Pep9’s microcode was translated by hand to millicode, which was again translated by hand to C. This C program was run through /Klee/, and verification succeeded with no errors.

This result does not preclude errors in Pep9’s spec, errors in translation to millicode, or errors in translation to microcode. However, this successful result provides a great degree of confidence in the Pep9 instruction-operand fetch unit.

Continued verification will explore the operand decode and instruction execution units. For now, 2/4 of the CPU’s microcode has been verified, which is a significant upgrade over the total lack of verification present before Pep9Milli.

# Future Work
Assertions and assumptions are elided from the microcode translation, as these features do not exist at this level of abstraction. Only when targeting the C implementation are these directives ac- knowledged. A process known as /compaction/[31] re-arranges the sub-optimal microcode generated by the milli-assembler into microcode more amenable for production use by overlap- ping and re-ordering microcode instructions for optimal CPU utilization. The millicode language is designed such that any valid millicode program has a corresponding valid microcode program. This prevents flawed analysis where the CPU must perform impossible operations (e.g. solve the halting problem) to perform the actions specified by the millicode. 
