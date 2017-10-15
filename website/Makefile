.PHONY: website clean
core_objs = ../core/base/neter.o ../core/base/thread.o ../core/base/log.o ../core/base/octets.o ../core/base/channel.o \
	    ../core/base/parser.o ../core/base/config.o ../core/base/task.o ../core/inl/enums.o

website_objs = website.o websitetask.o websiteparser.o

lib_objs = ../core/lib/lua/liblua.a

objs = $(core_objs) $(website_objs)

inl= -I../core/base -I../core/lib/lua -I../core/inl -I.

website: $(objs)
	g++ -o website $(objs) -std=c++11 -lpthread -g -ldl $(lib_objs)

$(objs) : %.o:%.cpp
	g++ -c $^ -o $@ $(inl) -std=c++11 -lpthread -g

clean:
	rm -f $(objs) website
