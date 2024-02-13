# Command to clean: cat first_test/Pre-req\=true/Pre-req-interval\=50/State\=enable/exponential/Sleep\=100/Warmup-requests\=10/times.txt | grep "sleep" | awk '{print $3}'  > clean_numbers.txt
# Run: python3 distribution_graph.py clean_numbers.txt output.png

import sys
import matplotlib.pyplot as plt

def plot_numbers(file_path, output_path):
    with open(file_path, 'r') as file:
        numbers = [int(line.strip()) for line in file]
        
    numbers.sort()
    x_values = range(1, len(numbers) + 1)

    plt.scatter(x_values, numbers)
    plt.xlabel('Index')
    plt.ylabel('Values')
    plt.title('Scatter Plot of Numbers')
    plt.savefig(output_path)
    plt.close()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_file> <output_image>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_image = sys.argv[2]

    plot_numbers(input_file, output_image)
