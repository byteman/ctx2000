ifdef MX27
CROSS:=arm-926ejs-linux-
else
CROSS:= 
endif

TOPDIR := ..

MSGDIR := ../../msg/
TARGET := gps.a

CC  := $(CROSS)gcc -I/usr/local/include/ 
CXX := $(CROSS)g++ -I/usr/local/include/
LD := $(CROSS)ar

OTHERS := 
INC := 
DBG := -g 
STD	 := -D_GNU_SOURCE -D_REENTRANT 
OBJS := Gps.o
LIBS := $(MSGDIR)message.a


ifeq ($(CROSS),arm-926ejs-linux-)
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

