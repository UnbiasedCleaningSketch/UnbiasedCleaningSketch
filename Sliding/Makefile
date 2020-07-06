GCC = g++
CFLAGS = -O2 -w -std=c++14 
SSEFLAGS =   -march=native
FILES = test.out 

all: $(FILES) 

test.out: test.cpp
	$(GCC) $(CFLAGS) $(SSEFLAGS) -o test.out test.cpp

.PHONY: clean
clean:
	-rm -f *.out $(objs)
