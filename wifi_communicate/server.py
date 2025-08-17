import socket

HOST = "0.0.0.0"
PORT = 5006

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen(2)

clients = []

print(f"server started on {HOST}:{PORT}")

while True:
    conn, addr = server.accept()
    clients.append(conn)
    print(f"client {addr} connected")

    # TODO we will want to create pairs of players who send "READY" messages
    #  and then have a state for their game stored in RAM - could even record history, store this in a DB, and then
    #  present this to the user afterwards + a scoreboard

    # TODO after creating pairs, hold state of game (current state (placing ships,
    #  shooting ships (and whose turn it is), and end screen / score))

    try:
        for client in clients:
            data = client.recv(1024)
            if not data:
                clients.remove(client)
                continue
            else:
                for other in clients:
                    if other != client:
                        other.sendall(data.encode())
    except ConnectionResetError:
        clients.remove(client)