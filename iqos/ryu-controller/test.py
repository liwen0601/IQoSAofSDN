from socket import *
dpi_server = socket(AF_INET,SOCK_STREAM)
server_ip="127.0.0.1"
server_port=9163
try:
    dpi_server.connect(server_ip,server_port)
    print("connected")
except:
    a=0
