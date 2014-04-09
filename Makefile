warmup1: my402listsort.o my402list.o
	gcc -o warmup1 -g my402listsort.o my402list.o

my402listsort.o: my402listsort.c my402listrecord.h
	gcc -g -c -Wall my402listsort.c

listtest: listtest.o my402list.o
	gcc -o listtest -g listtest.o my402list.o

listtest.o: listtest.c my402list.h
	gcc -g -c -Wall listtest.c

my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c

clean:
	rm -f *.o listtest warmup1
