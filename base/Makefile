objs = neter.o task.o thread.o start.o

all: $(objs)
	g++ -o core $(objs) -std=c++11 -lpthread

$(objs):%.o:%.cpp
	g++ -c $^ -o $@ -std=c++11 -lpthread

clean:
	rm -rf $(objs) core
