CC ?= gcc
OBJFILES := dvand_main.o dvan_server.o event_loop.o utils.o list.o dvan_server_client.o dvan_buffer.o dvan_message.o
CFLAGS+=-g -Wall -Werror -DDVAN_DEBUG
LDFLAGS+=

dvand: $(OBJFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJFILES)


%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJFILES) list_test dvand


list_test: list_test.c list.c
