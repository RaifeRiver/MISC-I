#pragma once

#include <stdint.h>
#include <string.h>

#include "pcie.h"

#define MEMORY_START 0x80000000

typedef struct {
	uint8_t* memory;
	uint32_t memorySize;
	uint32_t pc;
	uint32_t registers[15];

	PCIeData pcieData;
} VMData;

VMData* vm_create(uint32_t memory, uint32_t maxPCIeDevices);

void vm_destroy(VMData* vm);

inline uint8_t vm_read_address_b(const VMData* vm, const uint32_t address) {
	if (address >= MEMORY_START && address < MEMORY_START + vm->memorySize) {
		return vm->memory[address - MEMORY_START];
	}
	if (address >= PCIE_ECAM_BASE && address < PCIE_ECAM_BASE + PCIE_ECAM_SIZE) {
		return pcie_ecam_read_b(&vm->pcieData, address);
	}
	if (address >= PCIE_BAR_BASE && address < PCIE_BAR_BASE + PCIE_BAR_SIZE) {
		return pcie_bar_read_b(&vm->pcieData, address);
	}
	return 0;
}

inline void vm_write_address_b(const VMData* vm, const uint32_t address, const uint8_t value) {
	if (address >= MEMORY_START && address < MEMORY_START + vm->memorySize) {
		vm->memory[address - MEMORY_START] = value;
	}
	if (address >= PCIE_ECAM_BASE && address < PCIE_ECAM_BASE + PCIE_ECAM_SIZE) {
		pcie_ecam_write_b(&vm->pcieData, address, value);
	}
	if (address >= PCIE_BAR_BASE && address < PCIE_BAR_BASE + PCIE_BAR_SIZE) {
		pcie_bar_write_b(&vm->pcieData, address, value);
	}
}

inline uint16_t vm_read_address_h(const VMData* vm, const uint32_t address) {
	if (address >= MEMORY_START && address < MEMORY_START + vm->memorySize) {
		uint16_t value;
		memcpy(&value, &vm->memory[address - MEMORY_START], sizeof(uint16_t));
		return value;
	}
	if (address >= PCIE_ECAM_BASE && address < PCIE_ECAM_BASE + PCIE_ECAM_SIZE) {
		return pcie_ecam_read_h(&vm->pcieData, address);
	}
	if (address >= PCIE_BAR_BASE && address < PCIE_BAR_BASE + PCIE_BAR_SIZE) {
		return pcie_bar_read_h(&vm->pcieData, address);
	}
	return 0;
}

inline void vm_write_address_h(const VMData* vm, const uint32_t address, const uint16_t value) {
	if (address >= MEMORY_START && address < MEMORY_START + vm->memorySize) {
		memcpy(&vm->memory[address - MEMORY_START], &value, sizeof(uint16_t));
	}
	if (address >= PCIE_ECAM_BASE && address < PCIE_ECAM_BASE + PCIE_ECAM_SIZE) {
		pcie_ecam_write_h(&vm->pcieData, address, value);
	}
	if (address >= PCIE_BAR_BASE && address < PCIE_BAR_BASE + PCIE_BAR_SIZE) {
		pcie_bar_write_h(&vm->pcieData, address, value);
	}
}

inline uint32_t vm_read_address_w(const VMData* vm, const uint32_t address) {
	if (address >= MEMORY_START && address < MEMORY_START + vm->memorySize) {
		uint32_t value;
		memcpy(&value, &vm->memory[address - MEMORY_START], sizeof(uint32_t));
		return value;
	}
	if (address >= PCIE_ECAM_BASE && address < PCIE_ECAM_BASE + PCIE_ECAM_SIZE) {
		return pcie_ecam_read_w(&vm->pcieData, address);
	}
	if (address >= PCIE_BAR_BASE && address < PCIE_BAR_BASE + PCIE_BAR_SIZE) {
		return pcie_bar_read_w(&vm->pcieData, address);
	}
	return 0;
}

inline void vm_write_address_w(const VMData* vm, const uint32_t address, const uint32_t value) {
	if (address >= MEMORY_START && address < MEMORY_START + vm->memorySize) {
		memcpy(&vm->memory[address - MEMORY_START], &value, sizeof(uint32_t));
	}
	if (address >= PCIE_ECAM_BASE && address < PCIE_ECAM_BASE + PCIE_ECAM_SIZE) {
		pcie_ecam_write_w(&vm->pcieData, address, value);
	}
	if (address >= PCIE_BAR_BASE && address < PCIE_BAR_BASE + PCIE_BAR_SIZE) {
		pcie_bar_write_w(&vm->pcieData, address, value);
	}
}

inline uint32_t vm_read_register(const VMData* vm, const uint8_t r) {
	if (r == 0) {
		return 0;
	}
	return vm->registers[r - 1];
}

inline void vm_write_register(VMData* vm, const uint8_t r, const uint32_t value) {
	if (r != 0) {
		vm->registers[r - 1] = value;
	}
}

int32_t vm_add_pcie_device(const VMData* vm, uint8_t bus, uint8_t device, uint8_t function, PCIeDevice* pcieDevice);

void vm_remove_pcie_device(const VMData* vm, uint8_t bus, uint8_t device, uint8_t function);

void vm_step(VMData* vm);