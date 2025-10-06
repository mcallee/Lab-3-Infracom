# Lab-3 Infracom — Pub/Sub TCP vs UDP (C)

## Requisitos
- gcc (Xcode Command Line Tools en macOS: `xcode-select --install`)
- macOS/Linux

## Build
```bash
make tcp     # binarios TCP
make udp     # binarios UDP
make clean   # limpia build/

Ejecutar (TCP)

En 3 terminales:

# A) Broker
./build/broker_tcp 5001

# B) Subscriber
./build/subscriber_tcp 127.0.0.1 5001 AvsB

# C) Publisher
./build/publisher_tcp 127.0.0.1 5001 AvsB 10

Ejecutar (UDP)
# A) Broker
./build/broker_udp 5002

# B) Subscriber
./build/subscriber_udp 127.0.0.1 5002 AvsB

# C) Publisher
./build/publisher_udp 127.0.0.1 5002 AvsB 20
