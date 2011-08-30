ifdef MX27
CROSS:=arm-926ejs-linux-
else
CROSS:= 
endif

TOPDIR := ..

MSGDIR := ../../msg/
TARGET := nms_agent.a

CC  := $(CROSS)gcc -I/opt/dt800/include/ 
CXX := $(CROSS)g++ -I/opt/dt800/include/
LD := $(CROSS)ar

OTHERS := 
INC := 
DBG := -g 
STD	 := -D_GNU_SOURCE -D_REENTRANT 
OBJS := nms_agent.o socket.o
LIBS := $(MSGDIR)message.a


ifdef MX27
CXXFLAGS =
else
CXXFLAGS =-m32 $(STD)
endif
CXXFLAGS += -fpermissive -O2  -Wno-cast-qual $(INC)
LDFLAGS :=

.PHONY:all

all:$(TARGET)

-include $(OBJS:.o=.d)

$(TARGET):$(OBJS)
	$(LD) cqs  $@ $^

%.o: %.cpp
	$(CXX) -Wp,-MMD,"$*.d",-MQ,"$@",-MP -c $(CXXFLAGS)  -o $@ $<

clean::
	rm -f $(OBJS) $(TARGET) *.d	*~ 

