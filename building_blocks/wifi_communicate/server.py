import asyncio

clients = []

async def handle_client(reader, writer):
    # handle new clients
    clients.append(writer)
    addr = writer.get_extra_info('peername')
    print(f"Client {addr} connected")

    # handle incoming data and forwarding to other clients
    try:
        while True:
            data = await reader.read(1024)
            print(data)
            if not data:
                break
            for other in clients:
                if other != writer:
                    other.write(data)
                    await other.drain()
    except ConnectionResetError:
        pass
    finally:
        clients.remove(writer)
        writer.close()
        await writer.wait_closed()
        print(f"Client {addr} disconnected")

async def main():
    # TODO we will want to create pairs of players who send "READY" messages
    #  and then have a state for their game stored in RAM - could even record history, store this in a DB, and then
    #  present this to the user afterwards + a scoreboard

    # TODO after creating pairs, hold state of game (current state (placing ships,
    #  shooting ships (and whose turn it is), and end screen / score))

    server = await asyncio.start_server(handle_client, '0.0.0.0', 5006)
    async with server:
        await server.serve_forever()

asyncio.run(main())
