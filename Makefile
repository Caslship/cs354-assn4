# Linux makefile for CS 354 skeleton code

CC= g++

ifneq (,$(findstring WINDOWS,$(PATH)))
    UNAME := Windows
else
    UNAME := $(shell uname -s)
endif

ifeq ($(UNAME),Linux)
    LIBS= -lXext -lX11 -lglui -lGL -lGLU -lglut
endif
ifeq ($(UNAME),Windows)
    LIBS= -lglui -lopengl32 -lglu32 -lfreeglut
endif

CFLAGS= -std=c++11 -O2 -Wno-unused-result
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

