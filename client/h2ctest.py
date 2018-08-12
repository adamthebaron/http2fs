#!/usr/bin/env python

import socket
import sys

def main(argv):
    server = argv[0]
    port = argv[1]
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('coltrane.gnot.pluvi.us', 80))
    message = ("GET / HTTP/1.1\nVince: Guaraldi\nHost: coltrane.gnot.pluvi.us\nConnection: Upgrade\nUpgrade: h2c\n\n")
    s.sendall(message)
    resp = s.recv(2048)
    print("received: {}".format(resp.decode('UTF-8')))
    s.shutdown(socket.SHUT_RDWR)
    s.close()

if __name__ == "__main__":
    main(['coltrane.gnot.pluvi.us', 80])
