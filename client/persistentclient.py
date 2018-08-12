#!/usr/bin/env python

import socket
import sys
import random
import time

header_examples = [
    "Accept: text/html",
    "Accept-Charset: utf-8",
    "Accept-Encoding: gzip",
    "Accept-Language: en_US",
    "Accept-Datetime: Sun, 01 Jan 1970 12:00:00 UTC",
    "Cache-Control: no-cache",
    "Connection: Keep-Alive",
    "Content-Length: 808",
    "Content-Type: text/html",
    "Date: Sun, 01 Jan 1970 12:00:00 UTC",
    "Host: guaraldi.butler.edu",
    "If-Modified-Since: Sun, 01 Jan 1970 12:00:00 UTC"
]

def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('coltrane.gnot.pluvi.us', 80))
    message = "GET / HTTP/1.1\nVince: Guaraldi\n" + random.choice(header_examples) + "\n"
    print("seding " + message)
    s.sendall(message)
    s.shutdown(socket.SHUT_RDWR)
    s.close()

if __name__ == "__main__":
    main()
