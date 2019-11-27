
export BUILDDIR = ../tests/build/instruction-fetch
export LOCALDIR = ../build/instruction-fetch
bitcode := $(LOCALDIR)/sim.bc
asmcode := $(LOCALDIR)/sim.s
objects := $(LOCALDIR)/sim.o

$(info $(shell mkdir -p $(LOCALDIR)))


build : $(objects)
	$(MAKE) -C ../../simmain

# Successive fibonnaci numbers in memory.
$(LOCALDIR)/sim.bc: instructionfetch.c ../../simmain/sim.h
	clang-6.0 -I ../../simmain -fPIC -g -c -emit-llvm instructionfetch.c -o $(LOCALDIR)/sim.bc

$(LOCALDIR)/%.bc: %.c;
	clang-6.0 -fPIC -g -c -emit-llvm $^ -o $@

$(LOCALDIR)/%.s: $(LOCALDIR)/%.bc;
	llc-6.0 -relocation-model=pic $^ -o $@

$(LOCALDIR)/%.o: $(LOCALDIR)/%.s;
	as -o $@ $^  

klee: $(bitcode)
	$(MAKE) -C ../../simmain klee
clean:
	$(MAKE) -C ../../simmain clean

