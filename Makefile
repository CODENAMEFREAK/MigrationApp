CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		MultiMigrate.o Logging.o

LIBS =		-lvirt

TARGET =	MultiMigrate

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
