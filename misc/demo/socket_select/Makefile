CC		= gcc
TARGET 	= main.o
SRCS	= SDF_Interface.c socket_client.c main.c -lpthread

all:$(OBJS)
	$(CC) -g $(SRCS) -o $(TARGET)  

clean:
	rm -f *.o 
	rm -f *.so 
