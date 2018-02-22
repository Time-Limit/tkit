.PHONY: website clean
CC = g++
core_objs = ../core/base/neter.o ../core/base/thread.o ../core/base/log.o ../core/base/octets.o \
	    ../core/base/config.o ../core/base/task.o ../core/inl/enums.o ../core/base/file.o \
	    ../core/base/md5.o ../core/base/session.o  ../core/base/channel.o ../core/base/protocol.o \
	    ../core/base/httpssession.o

website_objs = website.o websitetask.o websitebase.o

lib_objs = ../core/lib/lua/liblua.a

objs = $(core_objs) $(website_objs)

inl = -I../core/base -I../core/lib/lua -I../core/inl -I.

flag = -D_LITTLE_ENDIAN_

args = -Wall -g -std=c++11

website: $(objs) 
	$(CC) -o website $^ -lpthread -ldl $(lib_objs) $(args) $(flag)

$(objs) : %.o : %.cpp
	$(CC) -c $^ -o $@ $(inl) -lpthread $(args) $(flag)

clean:
	rm -f $(objs) website
