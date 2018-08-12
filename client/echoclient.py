#!/usr/local/bin/python3.6

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("coltrane.gnot.pluvi.us", 7))
message = ""
while(message != "exit"):
    message = input("> ")
    s.send(bytes(message, 'UTF-8'))
    data = s.recv(2048)
s.close()

print(data)
