sources = dummymain.c alufunc.c cpu.c
bitcode = dummymain.bc alufunc.bc cpu.bc
asmcode = dummymain.s alufunc.s cpu.s
objects = dummymain.o alufunc.o cpu.o

build : $(objects)
	gcc -L/home/matthewmcraven/klee/klee/build/lib \
	$(objects) -o dummymain.out \
	-lkleeBasic -lkleeCore -lkleaverExpr -lkleaverSolver \
	-lkleeModule -lkleeRuntest -lkleeSupport

klee: dummymain.bcl
	klee  --libc=uclibc --posix-runtime dummymain.bcl
	#See https://klee.github.io/docs/options/ for ideas on linking in external files.
	#klee dummymain.bcl
	
dummymain.bcl: $(bitcode)
	# The RDF-DUMP flag is undocumented, but it preserves all
	# debug information in the linked bitcode.
	# This is valuable for visualization of test coverage.
	#clang-6.0 -I /home/matthewmcraven/klee/klee/include/ -c -g -emit-llvm $(sources) -o dummymain.bc;
	llvm-link-6.0 -rdf-dump $(bitcode) -o dummymain.bcl

dummymain.bc: dummymain.c defs.h alufunc.h cpu.h
	clang-6.0 -I /home/matthewmcraven/klee/klee/include/ -c -g -emit-llvm dummymain.c

dummymain.s: dummymain.bc alufunc.s
	llc-6.0 dummymain.bc  -o dummymain.s

dummymain.o: dummymain.s
	as -o dummymain.o dummymain.s 


alufunc.bc: alufunc.c defs.h
	clang-6.0 -I /home/matthewmcraven/klee/klee/include/ -g -c -emit-llvm alufunc.c

alufunc.s: alufunc.bc
	llc-6.0 alufunc.bc  -o alufunc.s

alufunc.o: alufunc.s
	as -o alufunc.o alufunc.s 


cpu.bc: cpu.c defs.h
	clang-6.0 -I /home/matthewmcraven/klee/klee/include/ -g -c -emit-llvm cpu.c

cpu.s: cpu.bc
	llc-6.0 cpu.bc  -o cpu.s

cpu.o: cpu.s
	as -o cpu.o cpu.s 
	

clean:
	rm $(bitcode) $(asmcode) $(objects) dummymain.out dummymain.bcl 
	rm -rf klee-*/
	@
