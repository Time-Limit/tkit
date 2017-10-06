.PHONY: gate clean
core_objs = ../core/base/neter.o ../core/base/thread.o ../core/base/log.o ../core/base/octets.o ../core/base/channel.o \
	    ../core/base/parser.o ../core/base/config.o

gate_objs = gate.o gatetask.o

lib_objs = ../core/lib/liblua.a

objs = $(core_objs) $(gate_objs)

inl= -I../core/base -I../core/inl -I.

gate: $(objs)
	g++ -o gate $(objs) -std=c++11 -lpthread -g -ldl $(lib_objs)

$(objs) : %.o:%.cpp
	g++ -c $^ -o $@ $(inl) -std=c++11 -lpthread -g

clean:
	rm -f $(objs) gate
