all: compat compat-bin

compat: compat.cc
	g++ -O3 -o compat compat.cc -I../cereal/include

compat-bin: compat.cc
	g++ -O3 -o compat-bin compat.cc -I../cereal/include -DUSE_BINARY

clean:
	rm -fv compat compat-bin