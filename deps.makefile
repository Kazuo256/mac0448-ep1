# dependencies
.temp/main.o: src/main.c src/ep1.h
.temp/ep1.o: src/ep1.c src/ep1.h src/ep1/net.h src/ep1/server.h \
 src/ep1/net.h
.temp/ep1/net.o: src/ep1/net.c src/ep1/net.h
.temp/ep1/server.o: src/ep1/server.c src/ep1/server.h src/ep1/net.h
.temp/ep1/:
	mkdir .temp/ep1/
