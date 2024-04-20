CC=gcc
CFLAGS=-I.
SRC_DIR1=../../lab3/src/
SRC_DIR2=../../lab4/src/

all: parallel_min_max zombie process_memory parallel_sum

parallel_min_max: $(SRC_DIR1)parallel_min_max.c find_min_max.o utils.o
	$(CC) -o parallel_min_max $(SRC_DIR1)parallel_min_max.c $(SRC_DIR1)find_min_max.o $(SRC_DIR1)utils.o $(CFLAGS)

zombie: $(SRC_DIR2)zombie.c
	$(CC) -o zombie $(SRC_DIR2)zombie.c $(CFLAGS)

process_memory: $(SRC_DIR2)process_memory.c
	$(CC) -o process_memory $(SRC_DIR2)process_memory.c $(CFLAGS)

parallel_sum: $(SRC_DIR2)parallel_sum.c $(SRC_DIR1)utils.c
	$(CC) -pthread -o parallel_sum $(SRC_DIR2)parallel_sum.c $(SRC_DIR1)utils.c -I$(SRC_DIR1) $(CFLAGS)

find_min_max.o: $(SRC_DIR1)find_min_max.c
	$(CC) -o $(SRC_DIR1)find_min_max.o -c $(SRC_DIR1)find_min_max.c $(CFLAGS)

utils.o: $(SRC_DIR1)utils.c
	$(CC) -o $(SRC_DIR1)utils.o -c $(SRC_DIR1)utils.c $(CFLAGS)

clean:
	rm -f parallel_min_max zombie process_memory parallel_sum $(SRC_DIR2)*.o $(SRC_DIR1)find_min_max.o $(SRC_DIR1)utils.o
