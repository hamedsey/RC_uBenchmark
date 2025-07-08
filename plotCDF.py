import os
import numpy as np
import matplotlib.pyplot as plt

def extract_latency_values(file_path):
    latencies = []
    with open(file_path, 'r') as file:
        for line in file:
            try:
                # Parse latency values (assume each line contains a number)
                latencies.append(float(line.strip()))
            except ValueError:
                # Ignore lines that can't be converted to float
                continue
    return latencies

def plot_cdfs():
    # Collect all .result files in the current directory
    result_files = [f for f in os.listdir('.') if f.endswith('.result')]
    
    # Set up the plot
    plt.figure(figsize=(10, 6))
    
    for file in result_files:
        latencies = extract_latency_values(file)
        
        if latencies:
            # Sort latencies for CDF
            sorted_latencies = np.sort(latencies)
            y_vals = np.arange(1, len(sorted_latencies) + 1) / len(sorted_latencies)
            
            # Plot CDF for this file
            plt.plot(sorted_latencies, y_vals, label=file)
    
    # Configure plot settings
    plt.xlabel('Latency')
    plt.ylabel('CDF')
    plt.title('CDF of Latency Values from .result Files')
    plt.legend()
    plt.grid(True)
    
    # Display the plot
    plt.show()

if __name__ == '__main__':
    plot_cdfs()
