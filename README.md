A simple chat program for client and server

Github link: https://github.com/xuraylei/Simple_Chat_Room.git

How to build:

	>cd Simple_Chat_Room

	>make


How to use:

	client side:

	./client <client name> <server ip> <server port>

	server side:

	./server <server ip> <server port> <maximum clients>


example:

	./server 127.0.0.1 5000 10

	./client bob 127.0.0.1 5000
	./client alice 127.0.0.1 5000
