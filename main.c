#include <stdio.h>
#include <string.h>
#include <sys/time.h>

void *memset_glibc(void *dest, int c, size_t n);
void *memset_musl(void *dest, int c, size_t n);
void *memset_simple(void *dest, int c, size_t n);
void *__memset_avx2_unaligned(void *dest, int c, size_t n);
void *memset_musl_asm(void *dest, int c, size_t n);

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
	{ "glibc avx2", __memset_avx2_unaligned, },
	{ "musl asm",   memset_musl_asm, },
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

int main(int argc, char *argv[])
{
	int i;

	flag_head = 0;
	flag_print = 0;
	test(tmpbuf, sizeof(tmpbuf), 1);

	flag_head = 1;
	flag_print = 1;
	test(tmpbuf, sizeof(tmpbuf), 1);

	flag_head = 0;
	for (i = 1; i < 63; i += 1) {
		test(tmpbuf, i, sizeof(tmpbuf) / i);
	}

	return 0;
}
