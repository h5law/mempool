.SILENT:

.PHONY: lib
lib:
	clang -c mempool.c
	ar rc libmempool.a mempool.o
	ranlib libmempool.a

.PHONY: build_test
build_test: lib
	clang memtest.c -I. -L. -lmempool -o tests

.PHONY: test
test: build_test
	./tests

.PHONY: clean
clean:
	rm tests libmempool.a mempool.o

.PHONY: all
all: test clean
