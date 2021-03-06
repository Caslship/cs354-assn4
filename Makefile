#
# Assignment #4
# Name: Jason Palacios
# UT EID: jap4839
# UTCS: jason777
#

# Linux makefile for CS 354 skeleton code
ifneq (,$(findstring WINDOWS,$(PATH)))
	UNAME := Windows
else
	UNAME := $(shell uname -s)
endif

ifeq ($(UNAME),Linux)
	CC= clang++
	LIBS= -L./lib -L/usr/lib/nvidia-340-updates -L/usr/lib/nvidia-340 -lXext -lX11 -lglui -lGL -lGLU -lglut
endif
ifeq ($(UNAME),Windows)
	CC= g++
	LIBS= -lglui -lopengl32 -lglu32 -lfreeglut
endif

CFLAGS= -std=c++11 -O2
PROD= scenegraphviewer

INCLUDES= -isystem ./include
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
	rm -f $(PROD).exe
	rm -f *.o