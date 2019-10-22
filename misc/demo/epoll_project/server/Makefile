
SRC := $(wildcard ./*.c)
#INC := $(wildcard ./Includes)
SRC += $(wildcard ./alg/*.c)
INC := $(wildcard ./include)
#INC := $(wildcard ./alg)

CC      = gcc
TARGET	= main.o
all:$(OBJS)
	$(CC) -I$(INC) -g  $(SRC)  -o $(TARGET)  -lpthread -L /usr/lib  -lmiracl -L/usr/local/ssl -lcrypto -lssl -ldl
