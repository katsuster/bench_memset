
all:
	$(CC) $(CFLAGS) main.c glibc.c musl.c simple.c -o a.out \
		musl.s \
		memset-avx2-unaligned-erms.S \
		memset-avx512-unaligned-erms.S

clean:
	rm -f a.out
