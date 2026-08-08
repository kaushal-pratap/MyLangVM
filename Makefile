.PHONY: all assembler vm run clean

PROGRAM ?= examples/arithmetic/add.asm

all: assembler vm

assembler:
	$(MAKE) -C assembler

vm:
	$(MAKE) -C vm

run: all
	./build/assembler/mylangasm $(PROGRAM)
	./build/vm/mylangvm ./build/bytecode/bytecode.mylangvm

clean:
	$(MAKE) -C assembler clean
	$(MAKE) -C vm clean
	rm -rf build/*