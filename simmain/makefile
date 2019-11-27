ifndef BUILDDIR
BUILDDIR := build
endif
#$(info $(shell mkdir -p $(BUILDDIR)))
sources := alufunc.c cpu.c cpufunc.c memory.c main.c 
bitcode := $(addprefix $(BUILDDIR)/,$(patsubst %.c,%.bc,$(sources))) 
asmcode := $(addprefix $(BUILDDIR)/,$(patsubst %.c,%.s,$(sources)))
objects := $(addprefix $(BUILDDIR)/,$(patsubst %.c,%.o,$(sources))) 
#bitcode := $(patsubst %.c,%.bc,$(sources))
#asmcode := $(patsubst %.c,%.s,$(sources))
#objects := $(patsubst %.c,%.o,$(sources))

build : $(objects)
	gcc $(BUILDDIR)/sim.o $(objects) -fPIC -o $(BUILDDIR)/main.out \
	-lkleeBasic -lkleeCore -lkleaverExpr -lkleaverSolver \
	-lkleeModule -lkleeRuntest -lkleeSupport



klee: $(BUILDDIR)/main.bcl
	klee  --libc=uclibc --posix-runtime -external-calls=all $(BUILDDIR)/main.bcl
	#See https://klee.github.io/docs/options/ for ideas on linking in external files.
	#klee dummymain.bcl
	
$(BUILDDIR)/main.bcl: $(bitcode)
	# The RDF-DUMP flag is undocumented, but it preserves all
	# debug information in the linked bitcode.
	# This is valuable for visualization of test coverage.
	#clang-6.0 -I /home/matthewmcraven/klee/klee/include/ -c -g -emit-llvm $(sources) -o dummymain.bc;
	llvm-link-6.0 -rdf-dump $(BUILDDIR)/sim.bc $(bitcode) -o $(BUILDDIR)/main.bcl

# NOP simulation
#sim.bc: nopsim.c sim.h
	#clang-6.0 -fPIC -I /home/matthewmcraven/klee/klee/include/ -g -c -emit-llvm nopsim.c -o sim.bc

# Simple arithmetic rountine test
#sim.bc: simbytetest.c
	#clang-6.0 -fPIC -I /home/matthewmcraven/klee/klee/include/ -g -c -emit-llvm simbytetest.c -o sim.bc

# Successive fibonnaci numbers in registers.
#sim.bc: simcfibregbyte.c sim.h
	#clang-6.0 -fPIC -I /home/matthewmcraven/klee/klee/include/ -g -c -emit-llvm simcfibregbyte.c -o sim.bc

# Successive fibonnaci numbers in memory.

$(BUILDDIR)/%.bc: %.c;
	clang-6.0 -fPIC -g -c -emit-llvm $^ -o $@

$(BUILDDIR)/%.s: $(BUILDDIR)/%.bc;
	llc-6.0 -relocation-model=pic $^  -o $@

$(BUILDDIR)/%.o: $(BUILDDIR)/%.s;
	as -o $@ $^  

clean:
	rm $(bitcode) $(asmcode) $(objects) $(BUILDDIR)/main.out $(BUILDDIR)/main.bcl 
	rm -rf klee-*/
	@
