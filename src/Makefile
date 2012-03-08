CC ?= gcc

CFLAGS+=-g -Wall -Werror -DDVAN_DEBUG
LDFLAGS+=

DVAND_SOURCES := dvand_main.o dvan_server.o event_loop.o utils.o list.o dvan_server_client.o dvan_buffer.o dvan_message.o

DVAN_CLIENT_SOURCES := utils.o list.o dvan_buffer.o dvan_message.o dvan_client_main.o

all: dvand dvan_client

dvand: $(DVAND_SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

dvan_client: $(DVAN_CLIENT_SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f *.o dvan_client dvand

