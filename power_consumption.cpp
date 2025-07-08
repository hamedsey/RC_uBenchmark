#include <iostream>
#include <fstream>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>

#define MSR_RAPL_POWER_UNIT    0x606
#define MSR_PKG_ENERGY_STATUS  0x611

// Helper function to read MSR values
uint64_t read_msr(int cpu_id, uint32_t msr_id) {
    char msr_path[128];
    sprintf(msr_path, "/dev/cpu/%d/msr", cpu_id);

    int fd = open(msr_path, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error opening MSR device for CPU " << cpu_id << std::endl;
        return -1;
    }

    uint64_t value;
    if (pread(fd, &value, sizeof(value), msr_id) != sizeof(value)) {
        std::cerr << "Error reading MSR register " << msr_id << std::endl;
        close(fd);
        return -1;
    }

    close(fd);
    return value;
}

int main() {
    int cpu_id = 0;  // Modify to the CPU you want to measure
    uint32_t msr_pkg_energy_status = MSR_PKG_ENERGY_STATUS;

    // Get scaling factors from MSR_RAPL_POWER_UNIT
    uint64_t power_unit = read_msr(cpu_id, MSR_RAPL_POWER_UNIT);
    uint8_t power_scaling_factor = (power_unit & 0xF);  // Bits 0-3
    uint8_t energy_scaling_factor = ((power_unit >> 4) & 0xF);  // Bits 4-7

    std::cout << "Power scaling factor: " << power_scaling_factor << std::endl;
    std::cout << "Energy scaling factor: " << energy_scaling_factor << std::endl;

    // Get initial energy reading
    uint64_t start_energy = read_msr(cpu_id, msr_pkg_energy_status);
    if (start_energy == (uint64_t)-1) return 1;

    // Sleep for a short period (e.g., 1 second) to simulate running the application
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    sleep(1);

    // Get energy reading after the sleep
    uint64_t end_energy = read_msr(cpu_id, msr_pkg_energy_status);
    if (end_energy == (uint64_t)-1) return 1;

    // Calculate the energy consumed (in raw units)
    uint64_t energy_diff = end_energy - start_energy;
    
    // Convert the raw energy to Joules
    double energy_in_joules = energy_diff / (1 << energy_scaling_factor);

    // Calculate power in Watts (Power = Energy / Time)
    double elapsed_time_sec = 1.0;  // Time difference in seconds
    double power_in_watts = energy_in_joules / elapsed_time_sec;

    std::cout << "Energy difference: " << energy_in_joules << " Joules" << std::endl;
    std::cout << "Power consumption: " << power_in_watts << " Watts" << std::endl;

    return 0;
}
