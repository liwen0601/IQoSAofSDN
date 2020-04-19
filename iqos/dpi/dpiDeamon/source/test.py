import socket

host = 'localhost'
port = 9163
bufsize = 1024 
addr = (host,port)

c = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
c.connect (addr)

