#ESTO COMPILA EL PROGRAMA PRINCIPAL
# solo usar make -B
server: httpServer.c
	#gcc -Wall LotteryScheduling.c -o programa -lm
	gcc httpServer.c -o server.o -lpthread
	./server.o 4

client: Client.c
	#gcc -Wall LotteryScheduling.c -o programa -lm
	gcc Client.c -o client.o -lpthread
	./client.o 1 10 index.html 127.0.0.1 6995

clientj: Client.c
	#gcc -Wall LotteryScheduling.c -o programa -lm
	gcc Client.c -o client.o -lpthread
	./client.o 1 50 index.html 186.177.110.87 6995