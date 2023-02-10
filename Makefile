
CC=/usr/bin/gcc

all: fork_ipc

fork_ipc: ./general_ipc_concepts/fork_ipc.c
	$(CC) -c ./general_ipc_concepts/fork_ipc.c -o ./bin/fork_ipc.o

clean:
	rm ./bin/obj/*.*

