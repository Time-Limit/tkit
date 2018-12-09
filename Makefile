cupid = ./bin/cupid
module = ./bin/libmodule.so


CC = g++

flag = -D_LITTLE_ENDIAN_
#args = -Wall -g -std=c++11 -fPIC
args = -Wall -g -std=c++11
inls = -I./core/cupid -I./lib/lualib -I./lib/pcre/include -I./core/macro -I./core/net -I./core/thread -I./core/tool
lualib = ./lib/lualib/liblua.a
pcrelib = ./lib/pcre/lib/libpcrecpp.a ./lib/pcre/lib/libpcre.a

objs = ./core/thread/callback.o ./core/thread/threadpool.o ./core/net/http_protocol.o ./core/net/neter.o ./core/net/session.o ./core/macro/base_param.o ./core/macro/enums.o ./core/tool/file.o ./core/tool/config.o ./core/tool/octets.o ./core/tool/basetool.o ./core/tool/log.o ./core/tool/md5.o ./core/cupid/cupid.o

all: $(cupid) $(module)

$(cupid): $(objs) $(module)
	$(CC) -o $(cupid) $(objs) -lpthread -ldl ${lualib} $(args) $(flag) -L ./bin -lmodule -Wl,-rpath .

$(module): $(objs)
	$(CC) -o $(module) -shared ./hls/hls.cpp ${flag} ${args} ${inls} ${pcrelib}

.cpp.o:
	$(CC) -c $^ -o $@ $(inls) -lpthread $(args) $(flag)
	

prepare:
	make -C ./core/net
	make -C ./core/tool
	make -C ./core/cupid
	make -C ./core/macro
	make -C ./core/thread

clean:
	rm -f $(objs)
	rm -f $(cupid)
	rm -f $(module)

