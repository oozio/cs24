OBJS = c_except.o my_setjmp.o ptr_vector.o divider.o

CFLAGS=-g
ASFLAGS=-g

check: test
	./test
test: test_setjmp.o my_setjmp.o	
	$(CC) $(CFLAGS) -o test test_setjmp.o my_setjmp.o
test_setjmp.o: test_setjmp.c my_setjmp.h
my_setjmp.o: my_setjmp.s


divider : $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

ptr_vector.c : ptr_vector.h
c_except.c : c_except.h my_setjmp.h
divider.c : c_except.h my_setjmp.h
clean :
	rm -f *.o *~ divider divider.exe

