CC=/usr/bin/g++
CFLAGS=-O2 -Wall -std=c++20

all: fork_ipc message_queue

fork_ipc: ./general_ipc_concepts/fork_ipc.c
	$(CC) $(CFLAGS) -o ./bin/fork_ipc ./general_ipc_concepts/fork_ipc.c

message_queue: ./msg_queue_ipc/receiver.c ./msg_queue_ipc/sender.c
	$(CC) $(CFLAGS) -o ./bin/msg_receiver ./msg_queue_ipc/receiver.c
	$(CC) $(CFLAGS) -o ./bin/msg_sender ./msg_queue_ipc/sender.c

clean:
	rm ./bin/obj/*.*

