all:
		gcc -Wall -c queue.c
		gcc -Wall -c actor.c
		gcc -Wall -c exm.c
		gcc -Wall -pthread -o exm.out exm.o actor.o queue.o
#		gcc -Wall -pthread -rdynamic -o queue_cyclic.out queue_cyclic.c
clean:
		rm -f *.out
