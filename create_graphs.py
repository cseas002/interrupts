import os
import matplotlib.pyplot as plt

def extract_latency_data(folder_path):
    latencies = {'Average Latency': {}, '99th Percentile Latency': {}}

    for root, dirs, files in os.walk(folder_path):
        for dir_name in dirs:
            output_file = os.path.join(root, dir_name, 'output.txt')
            if os.path.exists(output_file):
                with open(output_file, 'r') as file:
                    lines = file.readlines()

                    avg_latency_line = next((line for line in lines if line.startswith("Average Latency")), None)
                    percentile_latency_line = next((line for line in lines if line.startswith("99th Percentile Latency")), None)

                    if avg_latency_line and percentile_latency_line:
                        try:
                            avg_latency = float(avg_latency_line.split()[2])
                            percentile_latency = float(percentile_latency_line.split()[3])
                            latencies['Average Latency'][f"{root}/{dir_name}"] = avg_latency
                            latencies['99th Percentile Latency'][f"{root}/{dir_name}"] = percentile_latency
                        except (ValueError, IndexError):
                            print(f"Error processing file {output_file}. Skipping.")
                            continue
                    else:
                        print(f"Lines not found in the expected format in file {output_file}. Skipping.")

    return latencies




def save_with_incremented_suffix(file_path):
    base, ext = os.path.splitext(file_path)
    count = 1
    while os.path.exists(file_path):
        file_path = f"{base} ({count}){ext}"
        count += 1
    return file_path

def create_latency_graph(latency_data, metric_name, output_filename, graph_name):
    plt.bar(latency_data.keys(), latency_data.values())
    plt.xlabel('Folder Path')
    plt.ylabel(f'{metric_name} (microseconds)')
    plt.title(f'{metric_name} {graph_name}')
    plt.xticks(rotation=80, ha='right')  # Adjust rotation angle

    # Hide x-axis labels
    plt.xticks([])
    # Increase space at the bottom to make room for the title
    plt.subplots_adjust(bottom=0.25)

    # Expand margins to make more space on the bottom
    plt.margins(x=0, y=0.3)

    # Manually set the height of the figure
    plt.gcf().set_size_inches(10, 8)  # Adjust the size as needed

   # Create a list to store text annotations
    text_annotations = []

    # Annotate each bar with the x-axis label (folder path) inside the column
    for key, value in latency_data.items():
        modified_key = key.replace(graph_name, '').strip('/')  # Remove graph_name and leading/trailing slashes
        annotation = plt.text(key, value / 2, modified_key, ha='center', va='center', rotation=90, fontsize=8)
        text_annotations.append(annotation)

    output_filename = save_with_incremented_suffix(output_filename)
    plt.savefig(output_filename)
    plt.show()

    # Clear the text
    for annotation in text_annotations:
        annotation.set_text("")



if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        print("Usage: python3 create_graphs.py <folder_path>")
        sys.exit(1)

    folder_path = sys.argv[1]
    latency_data = extract_latency_data(folder_path)

    name = folder_path.replace('/', '-')
    # Create graph for 99th Percentile Latency
    create_latency_graph(latency_data['99th Percentile Latency'], '99th Percentile Latency', f'99th_latency-{name}.png', folder_path)

    # Create graph for Average Latency
    create_latency_graph(latency_data['Average Latency'], 'Average Latency & 99th Percentile Latency', f'average-{name}.png', folder_path)