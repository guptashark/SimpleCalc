CC = g++
CPPFLAGS = -Wall -Werror -Wextra -std=c++17
SRC = main.cpp



all: 
	$(CC) $(CPPFLAGS) $(SRC)
	./a.out

