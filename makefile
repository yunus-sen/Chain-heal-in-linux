CFLAGS = -O3

hepsi: libfdr.a derle

OBJS = dllist.o fields.o jval.o 

libfdr.a: $(OBJS)
	ar ru libfdr.a $(OBJS)
	ranlib libfdr.a 

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $*.c


derle:
	gcc -I ./ -o ./Dugum.o -c ./Dugum.c
	gcc -I ./ -o ./chain_heal ./Dugum.o ./chain_heal.c
