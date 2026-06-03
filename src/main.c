#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "vm.h"

int main() {
	FILE* file = fopen("../test.bin", "rb");
	if (file == NULL) {
		perror("fopen");
		return 1;
	}
	fseek(file, 0, SEEK_END);
	const uint32_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	uint8_t* memory = malloc(size);
	fread(memory, size, 1, file);
	fclose(file);
	VMData* vm = vm_create(8192, 16);
	for (uint32_t i = 0; i < size; i += 4) {
		vm_write_address_b(vm, i + MEMORY_START + 3, memory[i]);
		vm_write_address_b(vm, i + MEMORY_START + 2, memory[i + 1]);
		vm_write_address_b(vm, i + MEMORY_START + 1, memory[i + 2]);
		vm_write_address_b(vm, i + MEMORY_START, memory[i + 3]);
	}
	free(memory);
	int instructionCount = 0;

	const clock_t start = clock();
	while (vm_read_address_w(vm, vm->pc + MEMORY_START)) {
		vm_step(vm);
		instructionCount++;
	}
	const clock_t end = clock();
	const double time = (double) (end - start) / CLOCKS_PER_SEC * 1000;
	printf("r0: %i, r1: %i, r2: %i, r3: %i, r4: %i, r5: %i, r6: %i, r7: %i, r8: %i, r9: %i, r10: %i, r11: %i, r12: %i, r13: %i, r14: %i, r15: %i\n", vm_read_register(vm, 0), vm_read_register(vm, 1), vm_read_register(vm, 2), vm_read_register(vm, 3), vm_read_register(vm, 4), vm_read_register(vm, 5), vm_read_register(vm, 6), vm_read_register(vm, 7), vm_read_register(vm, 8), vm_read_register(vm, 9), vm_read_register(vm, 10), vm_read_register(vm, 11), vm_read_register(vm, 12), vm_read_register(vm, 13), vm_read_register(vm, 14), vm_read_register(vm, 1));
	printf("Ran %i instructions in %fms, pc: %i\n", instructionCount, time, vm->pc);
	vm_destroy(vm);
}