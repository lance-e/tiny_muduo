BUILD_DIR = ./build
CC = g++
LD = g++
LIB = -I base/ -I net/
CFLAGS =  -Wall   -c -g 
OBJS = $(BUILD_DIR)/main.o  $(BUILD_DIR)/CountDownLatch.o $(BUILD_DIR)/Thread.o	\
	   $(BUILD_DIR)/CurrentThread.o $(BUILD_DIR)/Timestamp.o $(BUILD_DIR)/EventLoop.o 	\
	   $(BUILD_DIR)/Channel.o $(BUILD_DIR)/Poller.o

	



############   compile  C     ###########

$(BUILD_DIR)/main.o: main.cpp 
	$(CC) $(CFLAGS)  $< -o $@

### base

$(BUILD_DIR)/CountDownLatch.o : base/CountDownLatch.cpp  
	$(CC) $(CFLAGS)  $< -o $@

$(BUILD_DIR)/Thread.o : base/Thread.cpp  
	$(CC) $(CFLAGS)  $< -o $@

$(BUILD_DIR)/CurrentThread.o : base/CurrentThread.cpp 
	$(CC) $(CFLAGS)  $< -o $@

$(BUILD_DIR)/Timestamp.o : base/Timestamp.cpp 
	$(CC) $(CFLAGS)  $< -o $@


### net

$(BUILD_DIR)/EventLoop.o : net/EventLoop.cpp    
	$(CC) $(CFLAGS)  $< -o $@

$(BUILD_DIR)/Channel.o : net/Channel.cpp
	$(CC) $(CFLAGS)  $< -o $@

$(BUILD_DIR)/Poller.o : net/Poller.cpp
	$(CC) $(CFLAGS)  $< -o $@




############ 	lind all object file ##############
$(BUILD_DIR)/server.bin : $(OBJS)
	$(LD)  $^ -o $@

.PHONY : mk_dir clean all

mk_dir:
	if [ ! -d $(BUILD_DIR)  ]; then mkdir $(BUILD_DIR);fi

clean:
	cd $(BUILD_DIR) && rm -r ./*

build: $(BUILD_DIR)/server.bin

all: mk_dir build 
