#include "pcie.h"

#include <string.h>

uint8_t pcie_ecam_read_b(const PCIeData *pcieData, uint32_t address) {
	const uint32_t offset = address - PCIE_ECAM_BASE;

	const uint8_t bus = offset >> 20 & 0xFF;
	const uint8_t device = offset >> 15 & 0x1F;
	const uint8_t function = offset >> 12 & 0x07;
	const uint16_t configAddress = offset & 0xFFF;

	for (uint32_t i = 0; i < pcieData->maxDevices; i++) {
		const PCIeDevice* d = pcieData->devices[i];
		if (d && d->bus == bus && d->device == device && d->function == function) {
			return d->config[configAddress];
		}
	}

	return 0xFF;
}

void pcie_ecam_write_b(const PCIeData *pcieData, const uint32_t address, uint8_t value) {
	const uint32_t offset = address - PCIE_ECAM_BASE;

	const uint8_t bus = offset >> 20 & 0xFF;
	const uint8_t device = offset >> 15 & 0x1F;
	const uint8_t function = offset >> 12 & 0x07;
	const uint16_t configAddress = offset & 0xFFF;

	for (uint32_t i = 0; i < pcieData->maxDevices; i++) {
		PCIeDevice* d = pcieData->devices[i];
		if (d && d->bus == bus && d->device == device && d->function == function) {
			d->config[configAddress] = value;
			break;
		}
	}
}

uint16_t pcie_ecam_read_h(const PCIeData *pcieData, uint32_t address) {
	const uint32_t offset = address - PCIE_ECAM_BASE;

	const uint8_t bus = offset >> 20 & 0xFF;
	const uint8_t device = offset >> 15 & 0x1F;
	const uint8_t function = offset >> 12 & 0x07;
	const uint16_t configAddress = offset & 0xFFF;

	for (uint32_t i = 0; i < pcieData->maxDevices; i++) {
		const PCIeDevice* d = pcieData->devices[i];
		if (d && d->bus == bus && d->device == device && d->function == function) {
			uint16_t value;
			memcpy(&value, &d->config[configAddress], sizeof(uint16_t));
			return value;
		}
	}

	return 0xFFFF;
}

void pcie_ecam_write_h(const PCIeData *pcieData, const uint32_t address, const uint16_t value) {
	const uint32_t offset = address - PCIE_ECAM_BASE;

	const uint8_t bus = offset >> 20 & 0xFF;
	const uint8_t device = offset >> 15 & 0x1F;
	const uint8_t function = offset >> 12 & 0x07;
	const uint16_t configAddress = offset & 0xFFF;

	for (uint32_t i = 0; i < pcieData->maxDevices; i++) {
		PCIeDevice* d = pcieData->devices[i];
		if (d && d->bus == bus && d->device == device && d->function == function) {
			memcpy(&d->config[configAddress], &value, sizeof(uint16_t));
			break;
		}
	}
}

uint32_t pcie_ecam_read_w(const PCIeData* pcieData, const uint32_t address) {
	const uint32_t offset = address - PCIE_ECAM_BASE;

	const uint8_t bus = offset >> 20 & 0xFF;
	const uint8_t device = offset >> 15 & 0x1F;
	const uint8_t function = offset >> 12 & 0x07;
	const uint16_t configAddress = offset & 0xFFF;

	for (uint32_t i = 0; i < pcieData->maxDevices; i++) {
		const PCIeDevice* d = pcieData->devices[i];
		if (d && d->bus == bus && d->device == device && d->function == function) {
			uint32_t value;
			memcpy(&value, &d->config[configAddress], sizeof(uint32_t));
			return value;
		}
	}

	return 0xFFFFFFFF;
}

void pcie_ecam_write_w(const PCIeData *pcieData, const uint32_t address, const uint32_t value) {
	const uint32_t offset = address - PCIE_ECAM_BASE;

	const uint8_t bus = offset >> 20 & 0xFF;
	const uint8_t device = offset >> 15 & 0x1F;
	const uint8_t function = offset >> 12 & 0x07;
	const uint16_t configAddress = offset & 0xFFF;

	for (uint32_t i = 0; i < pcieData->maxDevices; i++) {
		PCIeDevice* d = pcieData->devices[i];
		if (d && d->bus == bus && d->device == device && d->function == function) {
			if (configAddress >= 0x10 && configAddress <= 0x24) {
				const uint32_t barIndex = (configAddress - 0x10) / 4;
				if (value == 0xFFFFFFFF) {
					const uint32_t barMask = ~(d->barSizes[barIndex] - 1);
					memcpy(&d->config[configAddress], &barMask, sizeof(uint32_t));
				}
				else {
					d->barAddresses[barIndex] = value & 0xFFFFFFF0;
					memcpy(&d->config[configAddress], &value, sizeof(uint32_t));
				}
				break;
			}
			memcpy(&d->config[configAddress], &value, sizeof(uint32_t));
			break;
		}
	}
}

uint8_t pcie_bar_read_b(const PCIeData *pcieData, const uint32_t address) {
	for (uint32_t i = 0; i < pcieData->maxDevices; i++) {
		PCIeDevice* device = pcieData->devices[i];
		if (device) {
			for (uint8_t j = 0; j < 6; j++) {
				const uint32_t start = device->barAddresses[j];
				const uint32_t end = start + device->barSizes[j];
				if (address >= start && address < end) {
					const uint32_t offset = address - start;
					return device->read_b(device, j, offset);
				}
			}
		}
	}
	return 0xFF;
}

void pcie_bar_write_b(const PCIeData *pcieData, const uint32_t address, const uint8_t value) {
	for (uint32_t i = 0; i < pcieData->maxDevices; i++) {
		PCIeDevice* device = pcieData->devices[i];
		if (device) {
			for (uint8_t j = 0; j < 6; j++) {
				const uint32_t start = device->barAddresses[j];
				const uint32_t end = start + device->barSizes[j];
				if (address >= start && address < end) {
					const uint32_t offset = address - start;
					device->write_b(device, j, offset, value);
				}
			}
		}
	}
}

uint16_t pcie_bar_read_h(const PCIeData *pcieData, const uint32_t address) {
	for (uint32_t i = 0; i < pcieData->maxDevices; i++) {
		PCIeDevice* device = pcieData->devices[i];
		if (device) {
			for (uint8_t j = 0; j < 6; j++) {
				const uint32_t start = device->barAddresses[j];
				const uint32_t end = start + device->barSizes[j];
				if (address >= start && address < end) {
					const uint32_t offset = address - start;
					return device->read_h(device, j, offset);
				}
			}
		}
	}
	return 0xFFFF;
}

void pcie_bar_write_h(const PCIeData *pcieData, const uint32_t address, const uint16_t value) {
	for (uint32_t i = 0; i < pcieData->maxDevices; i++) {
		PCIeDevice* device = pcieData->devices[i];
		if (device) {
			for (uint8_t j = 0; j < 6; j++) {
				const uint32_t start = device->barAddresses[j];
				const uint32_t end = start + device->barSizes[j];
				if (address >= start && address < end) {
					const uint32_t offset = address - start;
					device->write_h(device, j, offset, value);
				}
			}
		}
	}
}

uint32_t pcie_bar_read_w(const PCIeData *pcieData, const uint32_t address) {
	for (uint32_t i = 0; i < pcieData->maxDevices; i++) {
		PCIeDevice* device = pcieData->devices[i];
		if (device) {
			for (uint8_t j = 0; j < 6; j++) {
				const uint32_t start = device->barAddresses[j];
				const uint32_t end = start + device->barSizes[j];
				if (address >= start && address < end) {
					const uint32_t offset = address - start;
					return device->read_w(device, j, offset);
				}
			}
		}
	}
	return 0xFFFFFFFF;
}

void pcie_bar_write_w(const PCIeData *pcieData, const uint32_t address, const uint32_t value) {
	for (uint32_t i = 0; i < pcieData->maxDevices; i++) {
		PCIeDevice* device = pcieData->devices[i];
		if (device) {
			for (uint8_t j = 0; j < 6; j++) {
				const uint32_t start = device->barAddresses[j];
				const uint32_t end = start + device->barSizes[j];
				if (address >= start && address < end) {
					const uint32_t offset = address - start;
					device->write_w(device, j, offset, value);
				}
			}
		}
	}
}