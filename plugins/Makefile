CPP = g++
CC  = gcc

INCLUDE = -I$(HOME)/deploy/include -I..

TARGET = damaris2visit

plugin: $(TARGET).cpp
	$(CPP) -fPIC -c $(TARGET).cpp $(INCLUDE)
	$(CC) -shared -Wl,-soname,$(TARGET).so -o $(TARGET).so $(TARGET).o

clean:
	rm -rf $(TARGET).o $(TARGET).so
