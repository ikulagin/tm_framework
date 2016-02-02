all:
		gcc -Wall -c queue.c
		gcc -Wall -c actor.c
		gcc -Wall -c messages.c
		gcc -Wall -c actor_consumer.c
		gcc -Wall -c actor_producer.c
		gcc -Wall -c exm.c
		gcc -Wall -pthread -o exm.out exm.o actor_producer.o actor_consumer.o messages.o actor.o queue.o
#		gcc -Wall -pthread -rdynamic -o queue_cyclic.out queue_cyclic.c
clean:
		rm -f *.out *.o
