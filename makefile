bitcode = dummymain.bc
asmcode = dummymain.s
objects = dummymain.o

build : $(objects)
	gcc $(objects) -o dummymain.out

klee: dummymain.bc
	klee dummymain.bc

dummymain.bc: dummymain.c
	clang-6.0 -c -emit-llvm dummymain.c

dummymain.s: dummymain.bc
	llc-6.0 dummymain.bc  -o dummymain.s

dummymain.o: dummymain.s
	as -o dummymain.o dummymain.s 

clean:
	rm $(bitcode) $(asmcode) $(objects) dummymain.out