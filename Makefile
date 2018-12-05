cupid = ./bin/cupid
module = ./bin/libmodule.so


CC = g++

flag = -D_LITTLE_ENDIAN_
args = -Wall -g -std=c++11 -fPIC
inls = -I./core/cupid -I./core/lualib -I./core/macro -I./core/net -I./core/thread -I./core/tool

objs = ./core/thread/callback.o ./core/thread/threadpool.o ./core/net/http_protocol.o ./core/net/neter.o ./core/net/session.o ./core/macro/base_param.o ./core/macro/enums.o ./core/tool/file.o ./core/tool/config.o ./core/tool/octets.o ./core/tool/basetool.o ./core/tool/log.o ./core/tool/md5.o ./core/cupid/cupid.o

all: $(cupid) $(module)

$(cupid): $(objs) $(module)
	$(CC) -o $(cupid) $(objs) -lpthread -ldl ./core/lualib/liblua.a $(args) $(flag) -L ./bin -lmodule -Wl,-rpath .

$(module): $(objs)
	$(CC) -o $(module) -shared ./core/cupid/example_module.cpp -fPIC

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

