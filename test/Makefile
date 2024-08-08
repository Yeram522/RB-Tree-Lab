.PHONY: test

CFLAGS=-I ../src -Wall -g -DSENTINEL

test: test-rbtree
	./test-rbtree
	valgrind ./test-rbtree

test-rbtree: test-rbtree.o ../src/rbtree.o

../src/rbtree.o:
	$(MAKE) -C ../src rbtree.o

debug: test-rbtree

clean:
	rm -f test-rbtree *.o
