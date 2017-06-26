objs = neter.o task.o thread.o start.o log.o octets.o netertask.o

all: $(objs)
	g++ -o core $(objs) -std=c++11 -lpthread -g

$(objs) : %.o:%.cpp
	g++ -c $^ -o $@ -std=c++11 -lpthread -g

clean:
	rm -rf $(objs) core
