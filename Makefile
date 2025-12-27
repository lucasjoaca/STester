CC = gcc
CFLAGS = -I. -ITesters -Wall
LIBS = -lpthread

# Lista de fișiere sursă
SRCS = server.c \
       Testers/latency_tester.c \
       Testers/http_tester.c \
       Testers/ftp_tester.c \
       Testers/echo_tester.c

# Numele executabilului final
TARGET = stester_server

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET) logs.txt