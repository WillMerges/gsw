# logging daemon

TARGET = gsw_logd

CXX = g++
CC = g++

OPTIONS +=

CFLAGS = -I$(GSW_HOME) -Wall -Wextra -Wpedantic -ggdb
CPPFLAGS = -I$(GSW_HOME) -Wall -Wextra -Wpedantic -ggdb
LDFLAGS = -L$(GSW_HOME)/lib/bin/ -Wl,-rpath=$(GSW_HOME)/lib/bin/

LIBS = -llogging -ltime

CPP_FILES := $(wildcard src/*.cpp)
C_FILES := $(wildcard src/*.c)

OBJS := $(CPP_FILES:.cpp=.o) $(C_FILES:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

clean:
	-rm src/*.o $(TARGET)
