.PHONY: cupid clean test
CC = g++

core_objs = ../core/base/neter.o ../core/base/log.o ../core/base/octets.o \
	    ../core/base/threadpool.o ../core/base/md5.o ../core/base/config.o \
	    ../core/base/file.o ../core/base/http_protocol.o ../core/inl/enums.o \
	    ../core/base/session.o ../core/inl/basetool.o

cupid_objs = cupid.o cupidbase.o

lib_objs = ../core/lib/lua/liblua.a

objs = $(core_objs) $(cupid_objs)

inl = -I../core/base -I../core/lib/lua -I../core/inl -I.

flag = -D_LITTLE_ENDIAN_

args = -Wall -g -std=c++11

cupid: $(objs) 
	$(CC) -o cupid $^ -lpthread -ldl $(lib_objs) $(args) $(flag)

$(objs) : %.o : %.cpp
	$(CC) -c $^ -o $@ $(inl) -lpthread $(args) $(flag)

clean:
	rm -f $(objs) cupid
