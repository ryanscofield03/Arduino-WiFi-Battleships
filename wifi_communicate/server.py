import socket

HOST = "0.0.0.0"
PORT = 5006

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((HOST, PORT))

clients = []

print("Server running on", HOST, PORT)

while True:
    data, addr = sock.recvfrom(1024)
    print(f"Received {data} from {addr}")

    if addr not in clients:
        clients.append(addr)

    for client in clients:
        if client != addr:
            sock.sendto(data, client)