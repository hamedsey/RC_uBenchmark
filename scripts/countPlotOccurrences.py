import matplotlib.pyplot as plt
from collections import Counter
import numpy as np

def calculate_and_plot_percentage_over_epochs(data, epochs):
    # Prepare a list to store percentages for each epoch
    epoch_percentages = []

    for epoch in range(epochs):
        # Count the occurrences of each number in the current epoch
        counts = Counter(data[epoch])
        total = len(data[epoch])
        percentages = [(counts.get(i, 0) / total) * 100 for i in range(12)]
        epoch_percentages.append(percentages)

    # Convert to numpy array for easy plotting
    epoch_percentages = np.array(epoch_percentages).T

    # Plot the stacked bar graph
    x = range(1, epochs + 1)  # Epoch numbers
    bottom = np.zeros(epochs)
    colors = [plt.cm.tab20(i / 11) for i in range(12)]

    for i, percentages in enumerate(epoch_percentages):
        plt.bar(x, percentages, bottom=bottom, color=colors[i], label=str(i))
        bottom += percentages

    # Set y-axis to have a max value of 100
    plt.ylim(0, 100)

    # Add labels and legend
    plt.xlabel("Epoch")
    plt.ylabel("Percentage (%)")
    plt.title("Percentage Distribution of Integers (0-11) Over Epochs")
    plt.legend(title="Integers", bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.tight_layout()
    plt.savefig('corePercentage.png', dpi=300, transparent=True)

    #plt.show()

    plt.clf()  # Clear the figure

# Example usage
# Generate example data for 100 epochs
np.random.seed(42)
data = [np.random.randint(0, 12, 100) for _ in range(100)]
calculate_and_plot_percentage_over_epochs(data, 100)