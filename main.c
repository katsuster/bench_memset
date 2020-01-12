#include <stdio.h>
#include <string.h>
#include <sys/time.h>

void *memset_glibc(void *dest, int c, size_t n);
void *memset_musl(void *dest, int c, size_t n);
void *memset_simple(void *dest, int c, size_t n);
void *memset_neon(void *dest, int c, size_t n);
#ifdef __AVX2__
void *__memset_avx2_unaligned(void *dest, int c, size_t n);
#endif
#ifdef __x86__
void *memset_musl_asm(void *dest, int c, size_t n);
#endif

typedef void * (*memset_func_t)(void *, int, size_t);

union result {
	struct timeval tv;
	int n;
};

struct testcase {
	const char *name;
	memset_func_t func;

	union result start;
	union result end;
};

int flag_print;
int flag_head;

void print_timeval(struct testcase cases[])
{
	struct timeval elapse;
	int i;

	if (!flag_print)
		return;

	if (flag_head) {
		for (i = 0; cases[i].name; i++) {
			printf("%10s |", cases[i].name);
		}
		printf("\n");
	}

	for (i = 0; cases[i].name; i++) {
		if (strcmp(cases[i].name, "size") == 0 ||
			strcmp(cases[i].name, "cnt") == 0) {
			printf("%10d |",
				cases[i].end.n);
		} else {
			timersub(&cases[i].end.tv, &cases[i].start.tv, &elapse);
			printf("%3d.%06d |",
				(int)elapse.tv_sec, (int)elapse.tv_usec);
		}
	}
	printf("\n");
}

char tmpbuf[1024 * 1024 * 1024];

struct testcase cases[] = {
	{ "libc",       memset, },
#ifdef __AVX2__
	{ "glibc avx2", __memset_avx2_unaligned, },
#endif
#ifdef __x86__
	{ "musl asm",   memset_musl_asm, },
#endif
	{ "neon",       memset_neon, },
	{ "glibc C",    memset_glibc, },
	{ "musl C",     memset_musl, },
	{ "simple",     memset_simple, },
	{ "size",       0, },
	{ "cnt",        0, },
	{ 0, 0, },
};

void test(char *p, size_t size, int cnt)
{
	int pos, i;

	for (pos = 0; cases[pos].func; pos++) {
		gettimeofday(&cases[pos].start.tv, NULL);
		for (i = 0; i < cnt; i++) {
			cases[pos].func(p, i, size);
		}
		gettimeofday(&cases[pos].end.tv, NULL);
	}

	cases[pos].start.n = 0;
	cases[pos].end.n = size;
	pos++;

	cases[pos].start.n = 0;
	cases[pos].end.n = cnt;
	pos++;

	print_timeval(cases);
}

int verify(char *p, size_t size)
{
	char buf[1024 + 128];
	int res = 0, pos, i;

	if (size > 1024) {
		printf("%d too large\n", (int)size);
	}

	for (pos = 0; cases[pos].func; pos++) {
		for (i = 0; i < 256; i++) {
			//canary
			memset(buf, i - 1, size + 128);
			memset(p, i - 1, size + 128);

			memset(buf, i, size);
			cases[pos].func(p, i, size);
			if (memcmp(buf, p, size + 128) != 0) {
				printf("NG: %10s: data:%d, size:%d\n",
					cases[pos].name, i, (int)size);
				res = 1;
			}
		}
	}

	return res;
}

int main(int argc, char *argv[])
{
	const int off = 0;
	size_t modsize = sizeof(tmpbuf) - off;
	int i;

	for (i = 1; i < 1024; i += 1) {
		verify(&tmpbuf[off], i);
	}

	flag_head = 0;
	flag_print = 0;
	test(&tmpbuf[off], modsize, 1);

	flag_head = 1;
	flag_print = 1;
	test(&tmpbuf[off], modsize, 1);

	flag_head = 0;
	for (i = 1; i < 63; i += 1) {
		test(&tmpbuf[off], i, modsize / i);
	}

	return 0;
}
