CC = g++
CFLAGS = -Wall -O3 --std=c++11

main.exe : main.o random.o wave_function.o data_blocking.o
	$(CC) random.o main.o -o main.exe
main.o : main.cpp
	$(CC) -c main.cpp -o main.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
wave_function.o : wave_function.hpp
	$(CC) -c wave_function.hpp -o wave_function.o $(CFLAGS)
data_blocking.o : data_blocking.hpp
	$(CC) -c data_blocking.hpp -o data_blocking.o $(CFLAGS)
clean :
	rm *.o main.exe seed.out
