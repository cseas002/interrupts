import os
import numpy as np
import matplotlib.pyplot as plt

def extract_time_taken(times_content):
    lines = times_content.strip().split('\n')
    time_taken_values = [int(line.split(': ')[1]) for line in lines if "Time taken" in line]
    return time_taken_values

def calculate_statistics(time_taken_values):
    average = np.mean(time_taken_values)
    percentile_99th = np.percentile(time_taken_values, 99)
    return average, percentile_99th

def traverse_and_compare(base_path):
    pre_req_false_files = {}
    pre_req_true_files = {}

    for root, dirs, files in os.walk(base_path):
        for file in files:
            if file == "times.txt":
                relative_path = os.path.relpath(root, base_path)
                key = relative_path.replace("Pre-req=false", "").replace("Pre-req=true", "")
                if "Pre-req=false" in relative_path:
                    pre_req_false_files[key] = os.path.join(root, file)
                elif "Pre-req=true" in relative_path:
                    pre_req_true_files[key] = os.path.join(root, file)
    
    results = []

    for key in pre_req_false_files.keys():
        if key in pre_req_true_files:
            with open(pre_req_false_files[key], 'r') as file:
                times_content_false = file.read()
            with open(pre_req_true_files[key], 'r') as file:
                times_content_true = file.read()
            
            time_taken_false = extract_time_taken(times_content_false)
            time_taken_true = extract_time_taken(times_content_true)
            
            avg_false, p99_false = calculate_statistics(time_taken_false)
            avg_true, p99_true = calculate_statistics(time_taken_true)

            average_improvement = ((avg_false - avg_true) / avg_false) * 100
            percentile_improvement = ((p99_false - p99_true) / p99_false) * 100

            results.append({
                'path': key,
                'avg_false': avg_false,
                'avg_true': avg_true,
                'p99_false': p99_false,
                'p99_true': p99_true,
                'average_improvement': average_improvement,
                'percentile_improvement': percentile_improvement
            })
    
    return results

def plot_results(results, output_path):
    labels = []
    avg_improvements = []
    p99_improvements = []

    for result in results:
        labels.append(result['path'])
        avg_improvements.append(result['average_improvement'])
        p99_improvements.append(result['percentile_improvement'])

    x = np.arange(len(labels))
    width = 0.35

    fig, ax = plt.subplots(figsize=(15, 7))

    rects2 = ax.bar(x + width/2, p99_improvements, width, label='99th Percentile Improvement')
    rects1 = ax.bar(x - width/2, avg_improvements, width, label='Average Improvement')

    ax.set_ylabel('Improvement (%)')
    ax.set_title('Comparison of Time Taken Metrics (Pre-req=True vs Pre-req=False)')
    ax.set_xticks(x)
    ax.set_xticklabels(labels, rotation=90, ha='right', va='bottom')

    # Adjust the position of each label slightly to the left
    for label in ax.get_xticklabels():
        label.set_x(label.get_position()[0] - 0.05)

    ax.legend()

    fig.tight_layout()
    
    plt.savefig(output_path)
    plt.close()

# Base path to the exponential folder
base_path = '~/interrupts/24-05-23_again/State=enable/exponential'
# Replace '~' with the actual home directory
base_path = os.path.expanduser(base_path)

results = traverse_and_compare(base_path)
output_image_path = 'comparison_plot_exponential.png'
plot_results(results, output_image_path)

print(f"Plot saved as {output_image_path}")
