# Linux makefile for CS 354 skeleton code

CC= g++
CFLAGS= -std=c++11 -O2 -Wno-unused-result
LIBS= -lopengl32 -lglu32 -lfreeglut
PROD= obj_viewer

HDRS= $(shell ls *.h)
SRCS= $(shell ls *.cpp)
OBJS= $(patsubst %.cpp, %.o, $(SRCS))

all: $(PROD)

$(OBJS): %.o: %.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $< -o $@

$(PROD): $(OBJS)
	$(CC) -o $(PROD) $^ $(LIBS)

clean:
	rm -f $(PROD)
	rm -f *.o

