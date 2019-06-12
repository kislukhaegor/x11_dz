# Project Makefile for xsort
# MAKE format:
# make -f xsort.m [fig=xfrag]
# where fig=xfrag:
# fig=xseg - Line segment
# fig=xrec - Isotenth Rectangle
# fig=xring - Ring & Circle
# fig=xoval - Oval & Ellipse

# Default Figure number

fig=xrectangle

# Exec file name

EXEC=${fig}

# Include path

INCLUDE_PATH=/opt/X11/include/

LIB_PATH=/opt/X11/lib

# Linking libraries

LIBS= -lX11 -lm

# Header files

INCLUDES=xsort.h

# cc invoking

CC=cc

compile_flags=-Wall -Wextra

# object file names

OBJECTS=${fig}.o xsort0.o xsort1.o xsort2.o

# project dependence
all : $(EXEC)
$(EXEC) : $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LIBS) $(addprefix  -L, $(LIB_PATH))

%.o : %.c
	$(CC) -o $@ -c $< $(compile_flags) $(build_flags) $(addprefix -I, $(INCLUDE_PATH))


.PHONY: clean
clean:
	rm -f *.o $(EXEC)

