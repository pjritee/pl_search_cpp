PHONY:	all

OUTPUTFILE  = pl_search_cpp.a

all: $(OUTPUTFILE)


$(OUTPUTFILE): pvar.o p_lessthan.cpp pred.o engine.o 
	ar r $@ $^
	ranlib $@

all: $(objects)

%.o: %.cpp
	g++ -c $< -o $@ -O2 -I../include

