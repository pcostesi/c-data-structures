IDIR = src/include
CFLAGS = -I$(IDIR)
CC = clang

SRC= src/
TEST= test/

LIBS=-lcunit

%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

all: lib test

test: $(SRC)bst.o $(SRC)ht.o $(SRC)ll.o $(SRC)sd.o $(TEST)bst_test.o $(TEST)ht_test.o $(TEST)ll_test.o $(TEST)sd_test.o
	$(CC) -o $(TEST)test $(TEST)main.c $(SRC)bst.o $(SRC)ht.o $(SRC)ll.o $(SRC)sd.o $(TEST)sd_test.o $(TEST)bst_test.o $(TEST)ht_test.o $(TEST)ll_test.o $(LIBS)  -I $(SRC)include

lib: $(SRC)bst.o $(SRC)ht.o $(SRC)ll.o $(SRC)sd.o
	ar rcs ds.a $(SRC)bst.o $(SRC)ht.o $(SRC)ll.o

.PHONY: clean
 
clean:
	- rm $(SRC)*.o $(TEST)*.o ds.a $(TEST)test

runtest:
	test/test

