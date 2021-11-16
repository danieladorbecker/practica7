from socket import *
serverSocket = socket(AF_INET, SOCK_STREAM)

#Prepare a server socket
#serverSocket.bind(('192.168.43.180', 8082))     #Dani
serverSocket.bind(('192.168.100.95', 8086))      #Dani Home
serverSocket.listen(1)
while True:
    print('Ready to serve2...')
    connectionSocket, addr = serverSocket.accept()
    try:
        message = connectionSocket.recv(1024)
        print(message.decode())
        print(message.split()[1].decode())
        filename = message.split()[1]
        print(filename)
        f = open(filename[1:])
        outputdata = f.read()
        f.close()
        #Send one HTP header line into socket
        connectionSocket.send('HTTP/1.0 200 ok\r\n\r\n'.encode())
        for i in range(0, len(outputdata)):
            connectionSocket.send(outputdata[i].encode())    
        #connectionSocket.close()
    except IOError:
        connectionSocket.send('404 Not Found'.encode())
        connectionSocket.close()
serverSocket.close()
