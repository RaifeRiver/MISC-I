#pragma once

#include <stdint.h>

#define PCIE_ECAM_BASE (uint32_t) 0x30000000
#define PCIE_ECAM_SIZE (uint32_t) 0x10000000

#define PCIE_BAR_BASE (uint32_t) 0x40000000
#define PCIE_BAR_SIZE (uint32_t) 0x40000000

typedef struct PCIeDevice PCIeDevice;

struct PCIeDevice {
	uint8_t bus;
	uint8_t device;
	uint8_t function;

	uint8_t config[4096];
	uint32_t barAddresses[6];

	uint32_t barSizes[6];

	void* state;

	uint8_t (*read_b)(PCIeDevice* device, uint8_t bar, uint32_t address);
	void (*write_b)(PCIeDevice* device, uint8_t bar, uint32_t address, uint8_t value);

	uint16_t (*read_h)(PCIeDevice* device, uint8_t bar, uint32_t address);
	void (*write_h)(PCIeDevice* device, uint8_t bar, uint32_t address, uint16_t value);

	uint32_t (*read_w)(PCIeDevice* device, uint8_t bar, uint32_t address);
	void (*write_w)(PCIeDevice* device, uint8_t bar, uint32_t address, uint32_t value);
};

typedef struct {
	uint32_t maxDevices;
	PCIeDevice** devices;
} PCIeData;

uint8_t pcie_ecam_read_b(const PCIeData* pcieData, uint32_t address);

void pcie_ecam_write_b(const PCIeData* pcieData, uint32_t address, uint8_t value);

uint16_t pcie_ecam_read_h(const PCIeData* pcieData, uint32_t address);

void pcie_ecam_write_h(const PCIeData* pcieData, uint32_t address, uint16_t value);

uint32_t pcie_ecam_read_w(const PCIeData* pcieData, uint32_t address);

void pcie_ecam_write_w(const PCIeData* pcieData, uint32_t address, uint32_t value);

uint8_t pcie_bar_read_b(const PCIeData* pcieData, uint32_t address);

void pcie_bar_write_b(const PCIeData* pcieData, uint32_t address, uint8_t value);

uint16_t pcie_bar_read_h(const PCIeData* pcieData, uint32_t address);

void pcie_bar_write_h(const PCIeData* pcieData, uint32_t address, uint16_t value);

uint32_t pcie_bar_read_w(const PCIeData* pcieData, uint32_t address);

void pcie_bar_write_w(const PCIeData* pcieData, uint32_t address, uint32_t value);