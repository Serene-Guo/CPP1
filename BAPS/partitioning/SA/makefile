LROOT=/home/rsch/leda6/6.0
CC = /usr/sfw/bin/g++
CCFLAGS = -O2
IFLAGS = -I${LROOT}/incl
LFLAGS = -L${LROOT}
AR = ar rvs

LIB_DIR = $(BAPHOME)/lib
INCL_DIR = $(BAPHOME)/include

LIB = $(LIB_DIR)/libSA.a

OBJS = BAPSASection.o BAPSAVessel.o BAPSAPartitioner.o

.SUFFIXES: .o .cpp

.cpp.o:
	$(CC) $(IFLAGS) $(CCFLAGS) -I$(INCL_DIR) -c $< $(LFLAGS)

$(LIB) : $(OBJS)
	$(AR) $(LIB) $?
	@echo "$(LIB) is updated"

clean:
	rm -rf $(LIB) *~ *.bak *.out *.o core


