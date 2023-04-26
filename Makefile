MAKEFLAGS += --silent
# location of swi-prolog/include in the device
INCLUDES = -I /usr/lib/swi-prolog/include/ 
# 1st portion is the path of libtinfo.so, 2nd portion is the path of libswipl.so in the device
LI = -L=/usr/lib/x86_64-linux-gnu/ -L/usr/lib/swi-prolog/lib/x86_64-linux/
# the path here is the path where libtinfo.so is
DL = -Wl,-rpath-link,/usr/lib/x86_64-linux-gnu
# find the path of libswipl.so and libtinfo.so in the device
PRELOAD=/usr/lib/swi-prolog/lib/x86_64-linux/libswipl.so:/usr/lib/x86_64-linux-gnu/libtinfo.so

all: Caller

Caller: Caller.cpp
	g++ -Wno-unused-result $(INCLUDES) $(LI) $(DL) -o Caller Caller.cpp -lrt -lswipl

run:
	 LD_PRELOAD=$(PRELOAD) ./Caller twitter.csv
clean:
	rm Caller info.pl