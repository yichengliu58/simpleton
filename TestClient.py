from socket import *
import time
import threading

def fun():
#while True:
	sock = socket(AF_INET,SOCK_STREAM,0)
	sock.connect(('127.0.0.1',12358))
	i = 1
	data = 'data'
	while True:
		if i >= 18:
			i = 1
			data = 'data'
		size = sock.send(data)
		#print data
		print(len(sock.recv(size)))
		data += data
		i += 1
		#time.sleep(10)
	sock.close()

threads = []
for i in range(8):
	t = threading.Thread(target = fun,name = 'fun')
	t.start()
	threads.append(t)
	time.sleep(0.5)
for t in threads:
	t.join()

