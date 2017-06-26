
core_objs = ../core/base/neter.o ../core/base/thread.o ../core/base/log.o ../core/base/octets.o
gate_objs = gatetask.o gate.o

objs = $(core_objs) $(gate_objs)

inl= -I../core/base -I../inl -I.

gate: $(objs)
	g++ -o gate $(objs) -std=c++11 -lpthread -g

$(objs) : %.o:%.cpp
	g++ -c $^ -o $@ $(inl) -std=c++11 -lpthread -g

clean:
	rm -f $(objs) gate
