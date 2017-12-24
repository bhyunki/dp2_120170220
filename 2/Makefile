CC = gcc
OPT = -g -Wall
RWL = pth_ll_rwl.c my_rand.c
OWN = pth_ll_own.c my_rand.c
LDLIBS = -lpthread

all:
	$(CC) $(OPT) -o pth_ll_rwl $(RWL) $(LDLIBS)
	$(CC) $(OPT) -o pth_ll_own $(OWN) $(LDLIBS)	

clean:
	rm -rf pth_ll_rwl
	rm -rf pth_ll_own
