CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -std=c11
INCLUDE := -Iinclude

TCP_BROKER := build/broker_tcp
TCP_PUB    := build/publisher_tcp
TCP_SUB    := build/subscriber_tcp

UDP_BROKER := build/broker_udp
UDP_PUB    := build/publisher_udp
UDP_SUB    := build/subscriber_udp

all: tcp udp
tcp: $(TCP_BROKER) $(TCP_PUB) $(TCP_SUB)
udp: $(UDP_BROKER) $(UDP_PUB) $(UDP_SUB)

$(TCP_BROKER): tcp/broker_tcp.c include/common.h
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@
$(TCP_PUB): tcp/publisher_tcp.c include/common.h
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@
$(TCP_SUB): tcp/subscriber_tcp.c include/common.h
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@
$(UDP_BROKER): udp/broker_udp.c include/common.h
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@
$(UDP_PUB): udp/publisher_udp.c include/common.h
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@
$(UDP_SUB): udp/subscriber_udp.c include/common.h
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@
clean:
	rm -rf build/*
.PHONY: all tcp udp clean
