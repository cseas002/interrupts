import sys

def correlation_analysis(wireshark_file, client_sent_get, requests_server_file, folder_name):
    # Read the contents of the files
    with open(wireshark_file, 'r') as f:
        wireshark_data = f.readlines()

    with open(client_sent_get, 'r') as f:
        client_data = f.readlines()

    with open(requests_server_file, 'r') as f:
        requests_server_data = f.readlines()

    output_file_name = folder_name + '/analysis.csv'   
    with open(output_file_name, 'w') as f:
        print('Client sent,Wireshark get,Server get,Server reply,Wireshark server reply sent,Client read', file=f)
        for i in range(len(wireshark_data) // 2):  # This should be 1000
            if i * 2 + 1 >= len(requests_server_data) or i * 2 + 1 >= len(wireshark_data) or i * 2 + 1 >= len(client_data):
                break
            wireshark_get =  str(int(float(wireshark_data[i * 2].split()[0]) * 10**9))  # Even (to port 8081)
            wireshark_reply = str(int(float(wireshark_data[i * 2 + 1].split()[0]) * 10**9))  # Odd (from port 8081)
            client_sent = client_data[i * 2].split()[-1]  # Sent request, even
            client_read = client_data[i * 2 + 1].split()[-1]  # Read reply, odd
            server_get =  requests_server_data[i * 2].split()[-1]
            server_reply = requests_server_data[i * 2 + 1].split()[-1]
            print(client_sent + "," + wireshark_get + "," + server_get + "," + server_reply + "," + wireshark_reply + "," + client_read, file=f)

def main():
    if len(sys.argv) != 5:
        print(f"Usage: python3 {sys.argv[0]} <wireshark_file> <times_file> <requests_server_file> <folder_name>.\n \
              e.g., python3 {sys.argv[0]} testing/wireshark.txt testing/sent_get_client.txt testing/requests_server.txt testing")
        sys.exit(1)

    wireshark_file = sys.argv[1]
    times_file = sys.argv[2]
    requests_server_file = sys.argv[3]
    folder_name = sys.argv[4]

    correlation_analysis(wireshark_file, times_file, requests_server_file, folder_name)

if __name__ == "__main__":
    main()
