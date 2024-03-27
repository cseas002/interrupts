import sys

def correlation_analysis(wireshark_file, client_sent_get, requests_server_file, folder_name):
    # Read the contents of the files
    with open(wireshark_file, 'r') as f:
        wireshark_data = f.readlines()

    with open(client_sent_get, 'r') as f:
        client_data = f.readlines()

    with open(requests_server_file, 'r') as f:
        requests_server_data = f.readlines()

    output_file_name = folder_name + '/analysis.txt'   
    with open(output_file_name, 'w') as f:
        print('Wireshark sent,Client sent,Server get,Server reply,Wireshark server reply,Client read', file=f)
        for i in range(len(wireshark_data) // 2):  # This should be 1000
            if i * 2 + 1 >= len(requests_server_data) or i * 2 + 1 >= len(wireshark_data) or i * 2 + 1 >= len(client_data):
                break
            wireshark_sent =  str(int(float(wireshark_data[i * 2].split()[0]) * 10**9))
            wireshark_reply = str(int(float(wireshark_data[i * 2 + 1].split()[0]) * 10**9))
            client_sent = client_data[i * 2].split()[-1]
            client_read = client_data[i * 2 + 1].split()[-1]
            server_get =  requests_server_data[i * 2].split()[-1]
            server_reply = requests_server_data[i * 2 + 1].split()[-1]
            print(wireshark_sent + "," + client_sent + "," + server_get + "," + server_reply + "," + wireshark_reply + "," + client_read, file=f)

def main():
    if len(sys.argv) != 5:
        print("Usage: python script.py <wireshark_file> <times_file> <requests_server_file> <folder_name>")
        sys.exit(1)

    wireshark_file = sys.argv[1]
    times_file = sys.argv[2]
    requests_server_file = sys.argv[3]
    folder_name = sys.argv[4]

    correlation_analysis(wireshark_file, times_file, requests_server_file, folder_name)

if __name__ == "__main__":
    main()
