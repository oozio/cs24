CC = gcc
CFLAGS = -Wall -Werror -g -O0

VMEM_OBJS = virtualmem.o vmalloc.o matrix.o test_matrix.o
VMEM_OBJS_FIFO = virtualmem.o vmalloc.o matrix.o test_matrix_fifo.o
VMEM_OBJS_CLRU = virtualmem.o vmalloc.o matrix.o test_matrix_clru.o
VMEM_OBJS_AGING = virtualmem.o vmalloc.o matrix.o test_matrix_aging.o

# So that the binary programs can be listed in fewer places.
# You will want to add to this variable as you implement various policies.
BINARIES = test_matrix test_matrix_fifo test_matrix_clru test_matrix_aging


all: $(BINARIES)

# Compile this file with optimizations so that it accesses memory in
# reasonable ways.
matrix.o: CFLAGS += -O2

test_matrix_aging: $(VMEM_OBJS_AGING) vmpolicy_aging.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_matrix_clru: $(VMEM_OBJS_CLRU) vmpolicy_clru.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_matrix_fifo: $(VMEM_OBJS_FIFO) vmpolicy_fifo.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_matrix: $(VMEM_OBJS) vmpolicy_random.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)


clean:
	rm -f *.o *~ $(BINARIES)


.PHONY: all clean

