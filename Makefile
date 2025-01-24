CC = gcc
# Added -Wextra, -O2, and include path. -fPIC not needed for main executable
CFLAGS = -Wall -Wextra -O2 -g -I./include  
# Fixed libverbs syntax and added pthread
LIBS = -libverbs -lpthread -ldl  

SOURCES = common.c \
          send-receive/send_receive.c \
          rdma-write/rdma_write.c \
          rdma-read/rdma_read.c \
          lambda/lambda_client.c \
          lambda/lambda_server.c \
          tests.c

OBJECTS = $(SOURCES:.c=.o)
TARGET = rdma
TEST_LIB = libmyfunc.so

all: $(TARGET) $(TEST_LIB)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Fixed source file name and added CFLAGS
$(TEST_LIB): libmyfunc.c  
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_LIB)