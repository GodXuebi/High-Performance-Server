SOURCE  := $(wildcard *.cpp)
OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))

TARGET  := main
CC      := g++
LIBS    := -lpthread 
INCLUDE:= 
CFLAGS  := -std=c++11 -g -Wall -O3 $(INCLUDE) -D_PTHREADS
CXXFLAGS:= $(CFLAGS)

.PHONY : objs clean veryclean rebuild all
all : $(TARGET)
objs : $(OBJS)
rebuild: veryclean all
clean :
	rm -fr *.o
veryclean : clean
	rm -rf $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
