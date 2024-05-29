all:
	#gcc uc_pingpong.c -libverbs -Wall -lm -lpthread -O3 -o UC
	#gcc rc_pingpong_from_uc.c -libverbs -Wall -lm -lpthread -O3 -o RC

	#g++ rc_pingpong_from_uc.cpp -libverbs -Wall -lm -lpthread -O3 -std=gnu++11 -o RCC
	g++ rc_pingpong_from_uc_client.cpp -libverbs -Wall -lm -lpthread -O3 -std=gnu++11 -o RCC_Client
	g++ rc_pingpong_from_uc_server.cpp -libverbs -Wall -lm -lpthread -O3 -std=gnu++11 -o RCC_Server

clean:
	rm UC