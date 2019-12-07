
export BUILDDIR = ../tests/build/fail-bad-neg
export LOCALDIR = ../build/fail-bad-neg
bitcode := $(LOCALDIR)/sim.bc
asmcode := $(LOCALDIR)/sim.s
objects := $(LOCALDIR)/sim.o

$(info $(shell mkdir -p $(LOCALDIR)))


build : $(objects)
	$(MAKE) -C ../../simmain

# Successive fibonnaci numbers in memory.
$(LOCALDIR)/sim.bc: badneg.c ../../simmain/sim.h
	clang-6.0 -I ../../simmain -fPIC -g -c -emit-llvm badneg.c -o $(LOCALDIR)/sim.bc

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

