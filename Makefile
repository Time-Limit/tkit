cupid = ./bin/cupid

CC = g++

flag = -D_LITTLE_ENDIAN_
args = -Wall -g -std=c++11
inls = -I./core/cupid -I./lib/lualib -I./lib/pcre/include -I./core/macro -I./core/net -I./core/thread -I./core/tool
lualib = ./lib/lualib/liblua.a
pcrelib = ./lib/pcre/lib/libpcrecpp.a ./lib/pcre/lib/libpcre.a

baseobjs = ./core/thread/callback.o ./core/thread/threadpool.o ./core/net/http_protocol.o ./core/net/neter.o ./core/net/session.o ./core/macro/base_param.o ./core/macro/enums.o ./core/tool/file.o ./core/tool/config.o ./core/tool/octets.o ./core/tool/basetool.o ./core/tool/log.o ./core/tool/md5.o ./core/cupid/cupid.o

hlsobjs = ./hls/hls.o

all: $(cupid)

$(cupid): $(baseobjs) $(hlsobjs)
	$(CC) -o $(cupid) $(baseobjs) ${hlsobjs} -lpthread -ldl ${lualib} ${pcrelib} $(args) $(flag)

.cpp.o:
	$(CC) -c $^ -o $@ $(inls) $(args) $(flag)

clean:
	rm -f $(baseobjs)
	rm -f $(hlsobjs)
	rm -f $(cupid)

