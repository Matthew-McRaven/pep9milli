sources = main.c alufunc.c cpu.c memory.c
bitcode = main.bc alufunc.bc cpu.bc cpufunc.bc memory.bc sim.bc
asmcode = main.s alufunc.s cpu.s cpufunc.s memory.s sim.s 
objects = main.o alufunc.o cpu.o cpufunc.o memory.o sim.o

build : $(objects)
	gcc $(objects) -fPIC -o main.out \
	-lkleeBasic -lkleeCore -lkleaverExpr -lkleaverSolver \
	-lkleeModule -lkleeRuntest -lkleeSupport

klee: main.bcl
	klee  --libc=uclibc --posix-runtime -external-calls=all main.bcl
	#See https://klee.github.io/docs/options/ for ideas on linking in external files.
	#klee dummymain.bcl
	
main.bcl: $(bitcode)
	# The RDF-DUMP flag is undocumented, but it preserves all
	# debug information in the linked bitcode.
	# This is valuable for visualization of test coverage.
	#clang-6.0 -I /home/matthewmcraven/klee/klee/include/ -c -g -emit-llvm $(sources) -o dummymain.bc;
	llvm-link-6.0 -rdf-dump $(bitcode) -o main.bcl

main.bc: main.c defs.h alufunc.h cpu.h sim.h
	clang-6.0 -fPIC -I /home/matthewmcraven/klee/klee/include/ -c -g -emit-llvm main.c

main.s: main.bc alufunc.s
	llc-6.0 -relocation-model=pic main.bc  -o main.s

main.o: main.s
	as -o main.o main.s 


alufunc.bc: alufunc.c defs.h
	clang-6.0 -fPIC -I /home/matthewmcraven/klee/klee/include/ -g -c -emit-llvm alufunc.c

alufunc.s: alufunc.bc
	llc-6.0 -relocation-model=pic alufunc.bc  -o alufunc.s

alufunc.o: alufunc.s
	as -o alufunc.o alufunc.s 


cpu.bc: cpu.c defs.h
	clang-6.0 -fPIC -I /home/matthewmcraven/klee/klee/include/ -g -c -emit-llvm cpu.c

cpu.s: cpu.bc
	llc-6.0 -relocation-model=pic cpu.bc  -o cpu.s

cpu.o: cpu.s
	as -o cpu.o cpu.s 

cpufunc.bc: cpufunc.c cpufunc.h
	clang-6.0 -fPIC -I /home/matthewmcraven/klee/klee/include/ -g -c -emit-llvm cpufunc.c

cpufunc.s: cpufunc.bc
	llc-6.0 -relocation-model=pic cpufunc.bc  -o cpufunc.s

cpufunc.o: cpufunc.s
	as -o cpufunc.o cpufunc.s

memory.bc: memory.c memory.h
	clang-6.0 -fPIC -I /home/matthewmcraven/klee/klee/include/ -g -c -emit-llvm memory.c

memory.s: memory.bc
	llc-6.0 -relocation-model=pic memory.bc  -o memory.s

memory.o: memory.s
	as -o memory.o memory.s 

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
sim.bc: simcfibmembyte.c
	clang-6.0 -fPIC -I /home/matthewmcraven/klee/klee/include/ -g -c -emit-llvm simcfibmembyte.c -o sim.bc

sim.s: sim.bc
	llc-6.0 -relocation-model=pic sim.bc  -o sim.s

sim.o: sim.s
	as -o sim.o sim.s 

clean:
	rm $(bitcode) $(asmcode) $(objects) main.out main.bcl 
	rm -rf klee-*/
	@
