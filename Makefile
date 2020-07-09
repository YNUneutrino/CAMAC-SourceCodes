# Makefile
CC = gcc
CXX = g++
CFLAGS = -O -Wall -I/usr/include #-DDEBUG

# CAMLIBDIR = /usr/local/camdrv
CAMLIBDIR = /home/ynu/daq/kinoko/drv/camdrv
LIBS = $(CAMLIBDIR)/camlib.o
INCS = -I. -I$(CAMLIBDIR)

all: 2249w c009 c-ts105 rpc-061 2249w_c-ts105 2249w_rpc-061 c009_c-ts105 c004 scaler_1

scaler_1: scaler_1.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(INCS)
	cp scaler_1 ../bin

2249w: 2249w.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(INCS)
	cp 2249w ../bin

c009: c009.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(INCS)
	cp c009 ../bin

c-ts105: c-ts105.o
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS) $(INCS)
	cp c-ts105 ../bin

rpc-061: rpc-061.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(INCS)
	cp rpc-061 ../bin

2249w_c-ts105: 2249w_c-ts105.o
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS) $(INCS)
	cp 2249w_c-ts105 ../bin

2249w_rpc-061: 2249w_rpc-061.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(INCS)
	cp 2249w_rpc-061 ../bin

c009_c-ts105: c009_c-ts105.o
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS) $(INCS)
	cp c009_c-ts105 ../bin

c004: c004.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(INCS)
	cp c004 ../bin

.c.o:
	$(CC) $(INCS) $(CFLAGS) -c -o $@ $<

.cc.o:
	$(CXX) $(INCS) $(CFLAGS) -c -o $@ $<

clean:
	/bin/rm -f *.o *.exe *.c~
