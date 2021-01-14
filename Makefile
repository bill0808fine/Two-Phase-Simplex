SRCS := $(wildcard ./source/*.cpp)
OBJS := $(patsubst %.cpp, ./%.o , $(notdir $(SRCS)))
#notdir : remove dir
#######


CC = g++ -std=c++11
CCFLAGS = -O3 -Wall

TARGET = simplex

INCLUDE = $(HEADER_INC) $(SRC_DIR) 

HEADER_INC = -I ./header
SRC_DIR = -I ./source



$(TARGET): $(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o $(TARGET)

./%.o: ./source/%.cpp
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@



.PHONY:clean
clean:
	rm -f $(TARGET) source/*.o *.o core *~