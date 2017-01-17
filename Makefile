CC = mpixlcxx_r

INCDIR := include
OBJDIR := build
SRCDIR := src
 
HFILES := $(shell find $(INCDIR) -type f -name \*.h)
SOURCES := $(shell find $(SRCDIR) -type f -name \*.cpp)
OBJECTS := $(patsubst %.o,%.cpp,$(SOURCES))
CFLAGS=  -qsmp=omp  -I$(INCDIR)  

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HFILES)
	$(CC) -d -c -o $@ $< $(CFLAGS)

full_sort: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCDIR)/*~ 

