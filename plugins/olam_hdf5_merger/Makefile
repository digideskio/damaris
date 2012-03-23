CPP = mpicxx
CC  = gcc

INCLUDE = -I/usr/lib/include -I../../include -I../../

TARGET = olam_hdf5_merger

plugin: $(TARGET).cpp
	$(CPP) -fPIC -c $(TARGET).cpp $(INCLUDE)
	$(CC) -shared -Wl,-soname,$(TARGET).so -o $(TARGET).so -lhdf5 $(TARGET).o

clean:
	rm -rf $(TARGET).o $(TARGET).so
