from socket import *
serverSocket = socket(AF_INET, SOCK_STREAM)

#Prepare a server socket
#serverSocket.bind(('192.168.43.180', 8081))     #Dani
serverSocket.bind(('192.168.100.95', 8081))      #Dani Home
serverSocket.listen(1)
while True:
	print('Ready to serve...')
	connectionSocket, addr = serverSocket.accept()
	try:
		message = connectionSocket.recv(1024)
		print(message.decode())
		print(message.split()[1].decode())
		connectionSocket.send('HTTP/1.0 200 OK\r\n\r\n'.encode())
		connectionSocket.send('<html>\r\n\r\n'.encode())
		connectionSocket.send('    <head>\r\n\r\n'.encode())
		connectionSocket.send('        <title> mi </title>\r\n\r\n'.encode())
		connectionSocket.send('    </head>\r\n\r\n'.encode())
		connectionSocket.send('<body>Hello World</body>\r\n\r\n'.encode())
		connectionSocket.close()
	except IOError:
		connectionSocket.send('404 Not Founds'.encode())
		connectionSocket.close()
serverSocket.close()
