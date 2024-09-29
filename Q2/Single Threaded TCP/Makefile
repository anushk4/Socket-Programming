MAKEFLAGS += -s

all:
	gcc server.c -o server
	gcc client.c -o client

clean:
	@rm -f server client
	@rm -f *~ *.o *.exe *perf*@