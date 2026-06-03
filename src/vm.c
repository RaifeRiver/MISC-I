#include "vm.h"

#include <stdio.h>
#include <stdlib.h>

#include "instructions.h"

VMData* vm_create(const uint32_t memory, const uint32_t maxPCIeDevices) {
	VMData* vm = malloc(sizeof(VMData));
	vm->memorySize = memory;
	vm->memory = malloc(memory * sizeof(uint8_t));
	vm->pc = 0;
	vm->pcieData.maxDevices = maxPCIeDevices;
	vm->pcieData.devices = malloc(maxPCIeDevices * sizeof(PCIeDevice*));
	return vm;
}

extern inline uint8_t vm_read_address_b(const VMData* vm, uint32_t address);

extern inline void vm_write_address_b(const VMData* vm, uint32_t address, uint8_t value);

extern inline uint16_t vm_read_address_h(const VMData* vm, uint32_t address);

extern inline void vm_write_address_h(const VMData* vm, uint32_t address, uint16_t value);

extern inline uint32_t vm_read_address_w(const VMData* vm, uint32_t address);

extern inline void vm_write_address_w(const VMData* vm, uint32_t address, uint32_t value);

extern inline uint32_t vm_read_register(const VMData* vm, uint8_t r);

extern inline void vm_write_register(VMData* vm, uint8_t r, uint32_t value);

extern inline void decode_3r(const uint32_t instruction, uint8_t r[3]) {
	r[0] = instruction >> 8 & 0xF;
	r[1] = instruction >> 12 & 0xF;
	r[2] = instruction >> 16 & 0xF;
}

extern inline uint16_t decode_2r_imm16(const uint32_t instruction, uint8_t r[2]) {
	r[0] = instruction >> 8 & 0xF;
	r[1] = instruction >> 12 & 0xF;
	return instruction >> 16;
}

extern inline uint32_t decode_1r_imm20(const uint32_t instruction, uint8_t r[1]) {
	r[0] = instruction >> 8 & 0xF;
	return (int32_t) instruction >> 12;
}

extern inline uint32_t calculate_branch_address(uint32_t imm16) {
	// imm16 <<= 1;  // TODO
	if (imm16 & 0x8000) {
		imm16 |= 0xFFFF0000;
	}
	return imm16 - 4;
}

void vm_destroy(VMData *vm) {
	free(vm->memory);
	free(vm);
}

int32_t vm_add_pcie_device(const VMData *vm, const uint8_t bus, const uint8_t device, const uint8_t function, PCIeDevice *pcieDevice) {
	if (device > 31 || function > 7) {
		return -1;
	}
	int32_t freeSlot = -1;
	for (int32_t i = 0; i < vm->pcieData.maxDevices; i++) {
		const PCIeDevice* d = vm->pcieData.devices[i];
		if (d) {
			if (d->bus == bus && d->device == device && d->function == function) {
				return 1;
			}
		}
		else if (freeSlot == -1) {
			freeSlot = i;
		}
	}
	if (freeSlot == -1) {
		return 2;
	}
	pcieDevice->bus = bus;
	pcieDevice->device = device;
	pcieDevice->function = function;
	vm->pcieData.devices[freeSlot] = pcieDevice;
	return 0;
}

void vm_remove_pcie_device(const VMData *vm, const uint8_t bus, const uint8_t device, const uint8_t function) {
	for (int32_t i = 0; i < vm->pcieData.maxDevices; i++) {
		const PCIeDevice* d = vm->pcieData.devices[i];
		if (d && d->bus == bus && d->device == device && d->function == function) {
			free(vm->pcieData.devices[i]);
			vm->pcieData.devices[i] = nullptr;
			break;
		}
	}
}

void vm_step(VMData *vm) {
	const uint32_t instruction = vm_read_address_w(vm, vm->pc + MEMORY_START);
	vm->pc += 4;
	switch (instruction & 0xFF) {
		case OP_NOP:
			break;
		case OP_ADD: {
			uint8_t r[3];
			decode_3r(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) + vm_read_register(vm, r[2]));
			break;
		}
		case OP_SUB: {
			uint8_t r[3];
			decode_3r(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) - vm_read_register(vm, r[2]));
			break;
		}
		case OP_ADDI: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) + imm16);
			break;
		}
		case OP_SUBI: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) - imm16);
			break;
		}
		case OP_LB: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], (int8_t) vm_read_address_b(vm, vm_read_register(vm, r[1]) + imm16));
			break;
		}
		case OP_LH: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], (int16_t) vm_read_address_h(vm, vm_read_register(vm, r[1]) + imm16));
			break;
		}
		case OP_LW: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_address_w(vm, vm_read_register(vm, r[1]) + imm16));
			break;
		}
		case OP_LBU: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_address_b(vm, vm_read_register(vm, r[1]) + imm16));
			break;
		}
		case OP_LHU: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_address_h(vm, vm_read_register(vm, r[1]) + imm16));
			break;
		}
		case OP_SH: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_address_h(vm, vm_read_register(vm, r[1]) + imm16, vm_read_register(vm, r[0]));
			break;
		}
		case OP_SW: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_address_w(vm, vm_read_register(vm, r[1]) + imm16, vm_read_register(vm, r[0]));
			break;
		}
		case OP_LUI: {
			uint8_t r[1];
			const uint32_t imm20 = decode_1r_imm20(instruction, r);
			vm_write_register(vm, r[0], imm20 << 12);
			break;
		}
		case OP_AND: {
			uint8_t r[3];
			decode_3r(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) & vm_read_register(vm, r[2]));
			break;
		}
		case OP_ANDI: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) & (int32_t) imm16);
			break;
		}
		case OP_OR: {
			uint8_t r[3];
			decode_3r(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) | vm_read_register(vm, r[2]));
			break;
		}
		case OP_ORI: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) | (int32_t) imm16);
			break;
		}
		case OP_XOR: {
			uint8_t r[3];
			decode_3r(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) ^ vm_read_register(vm, r[2]));
			break;
		}
		case OP_XORI: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) ^ (int32_t) imm16);
			break;
		}
		case OP_BCLR: {
			uint8_t r[3];
			decode_3r(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) & ~((uint32_t) 1 << (vm_read_register(vm, r[2]) & 0x1F)));
			break;
		}
		case OP_BCLRI: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) & ~((uint32_t) 1 << (imm16 & 0x1F)));
			break;
		}
		case OP_BSET: {
			uint8_t r[3];
			decode_3r(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) | (uint32_t) 1 << (vm_read_register(vm, r[2]) & 0x1F));
			break;
		}
		case OP_BSETI: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) | (uint32_t) 1 << (imm16 & 0x1F));
			break;
		}
		case OP_BINV: {
			uint8_t r[3];
			decode_3r(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) ^ (uint32_t) 1 << (vm_read_register(vm, r[2]) & 0x1F));
			break;
		}
		case OP_BINVI: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) ^ (uint32_t) 1 << (imm16 & 0x1F));
			break;
		}
		case OP_BEXT: {
			uint8_t r[3];
			decode_3r(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) >> (vm_read_register(vm, r[2]) & 0x1F) & 1);
			break;
		}
		case OP_BEXTI: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) >> (imm16 & 0x1F) & 1);
			break;
		}
		case OP_SLL: {
			uint8_t r[3];
			decode_3r(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) << (vm_read_register(vm, r[2]) & 0x1F));
			break;
		}
		case OP_SLLI: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) << (imm16 & 0x1F));
			break;
		}
		case OP_SRL: {
			uint8_t r[3];
			decode_3r(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) >> (vm_read_register(vm, r[2]) & 0x1F));
			break;
		}
		case OP_SRLI: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], vm_read_register(vm, r[1]) >> (imm16 & 0x1F));
			break;
		}
		case OP_SRA: {
			uint8_t r[3];
			decode_3r(instruction, r);
			vm_write_register(vm, r[0], (int32_t) vm_read_register(vm, r[1]) >> (vm_read_register(vm, r[2]) & 0x1F));
			break;
		}
		case OP_SRAI: {
			uint8_t r[2];
			const uint16_t imm16 = decode_2r_imm16(instruction, r);
			vm_write_register(vm, r[0], (int32_t) vm_read_register(vm, r[1]) >> (imm16 & 0x1F));
			break;
		}
		case OP_JAL: {
			uint8_t r[1];
			int32_t imm20 = (int32_t) decode_1r_imm20(instruction, r)/* << 1*/; // TODO
			if (imm20 & 0x100000) {
				imm20 |= (int32_t) 0xFFE00000;
			}
			vm_write_register(vm, r[0], vm->pc + 4);
			vm->pc += imm20;
			break;
		}
		case OP_JALR: {
			uint8_t r[2];
			const int16_t imm16 = (int16_t) decode_2r_imm16(instruction, r);
			const uint32_t jumpAddress = vm_read_register(vm, r[1]) + imm16 & ~(uint32_t) 1;
			vm_write_register(vm, r[0], vm->pc + 4);
			vm->pc = jumpAddress;
			break;
		}
		case OP_AUIPC: {
			uint8_t r[1];
			const uint32_t imm20 = decode_1r_imm20(instruction, r) << 12;
			vm_write_register(vm, r[0], vm->pc + imm20);
			break;
		}
		case OP_BEQ: {
			uint8_t r[2];
			const int32_t imm16 = decode_2r_imm16(instruction, r);
			if (vm_read_register(vm, r[0]) == vm_read_register(vm, r[1])) {
				vm->pc += calculate_branch_address(imm16);
			}
			break;
		}
		case OP_BNE: {
			uint8_t r[2];
			int32_t imm16 = decode_2r_imm16(instruction, r);
			if (imm16 & 0x8000) {
				imm16 |= (int32_t) 0xFFFF0000;
			}
			if (vm_read_register(vm, r[0]) != vm_read_register(vm, r[1])) {
				vm->pc += calculate_branch_address(imm16);
			}
			break;
		}
		case OP_BLT: {
			uint8_t r[2];
			const int32_t imm16 = decode_2r_imm16(instruction, r);
			if ((int32_t) vm_read_register(vm, r[0]) < (int32_t) vm_read_register(vm, r[1])) {
				vm->pc += calculate_branch_address(imm16);
			}
			break;
		}
		case OP_BGE: {
			uint8_t r[2];
			const int32_t imm16 = decode_2r_imm16(instruction, r);
			if ((int32_t) vm_read_register(vm, r[0]) >= (int32_t) vm_read_register(vm, r[1])) {
				vm->pc += calculate_branch_address(imm16);
			}
			break;
		}
		case OP_BLTU: {
			uint8_t r[2];
			const int32_t imm16 = decode_2r_imm16(instruction, r);
			if (vm_read_register(vm, r[0]) < vm_read_register(vm, r[1])) {
				vm->pc += calculate_branch_address(imm16);
			}
			break;
		}
		case OP_BGEU: {
			uint8_t r[2];
			const int32_t imm16 = decode_2r_imm16(instruction, r);
			if (vm_read_register(vm, r[0]) >= vm_read_register(vm, r[1])) {
				vm->pc += calculate_branch_address(imm16);
			}
			break;
		}
		default:
			fprintf(stderr, "Unknown opcode: %02x\n", instruction & 0xFF);
			exit(1);
	}
}
