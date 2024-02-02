import sys
import numpy as np

def generate_poisson_numbers(k, average):
    return np.random.poisson(average, k)

def main():
    # Get command-line arguments
    if len(sys.argv) != 3:
        print("Usage: python your_script.py <repetitions> <sleep_usecs>")
        sys.exit(1)

    repetitions = int(sys.argv[1])
    sleep_usecs = int(sys.argv[2])

    # Generate Poisson numbers
    poisson_numbers = generate_poisson_numbers(repetitions, sleep_usecs)

    # Save Poisson numbers to a file
    with open("poisson_numbers.txt", "w") as f:
        for number in poisson_numbers:
            f.write(f"{number}\n")

if __name__ == "__main__":
    main()
