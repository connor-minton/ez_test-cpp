CXXFLAGS = --std=c++03

.PHONY: clean all

all: test_simple

clean:
	rm -f test_simple

test_simple: ez_test.h test_simple.cpp
	g++ $(CXXFLAGS) test_simple.cpp -o test_simple
