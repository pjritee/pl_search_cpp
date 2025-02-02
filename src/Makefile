PHONY:	all

OUTPUTFILE  = pl_search.a

all: $(OUTPUTFILE)


$(OUTPUTFILE): term.o pred.o engine.o 
	ar r $@ $^
	ranlib $@

all: $(objects)

%.o: %.cpp
	g++ -c $< -o $@ -O2

