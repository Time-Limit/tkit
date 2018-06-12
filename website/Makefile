.PHONY: website clean test
CC = g++
core_objs = ../core/base/neter.o ../core/base/thread.o ../core/base/log.o ../core/base/octets.o \
	    ../core/base/config.o ../core/base/task.o ../core/inl/enums.o ../core/base/file.o \
	    ../core/base/md5.o ../core/base/session.o ../core/base/protocol.o

website_objs = website.o websitetask.o websitebase.o

lib_objs = ../core/lib/lua/liblua.a -lssl -lcrypto

objs = $(core_objs) $(website_objs)

inl = -I../core/base -I../core/lib/lua -I../core/inl -I.

flag = -D_LITTLE_ENDIAN_ -D_NORMAL_THREAD_COUNT_=8

args = -Wall -g -std=c++11

website: $(objs) 
	$(CC) -o website $^ -lpthread -ldl $(lib_objs) $(args) $(flag)

$(objs) : %.o : %.cpp
	$(CC) -c $^ -o $@ $(inl) -lpthread $(args) $(flag)

test.o : test.cpp
	$(CC) -c test.cpp -o test.o $(inl) -lpthread ${args} $(flag)

test: $(core_objs) test.o
	$(CC) -o test $^ -lpthread -ldl $(lib_objs) $(args) $(flag)

clean:
	rm -f $(objs) website test test.o
