CFLAGS ?= -Wall -g -O3 -fno-builtin -march=native

SRCS = main.c glibc.c musl.c simple.c neon.c

# if x86
#SRCS += musl.s memset-avx2-unaligned-erms.S memset-avx512-unaligned-erms.S

all:
	$(CC) $(CFLAGS) $(SRCS) -o a.out

clean:
	rm -f a.out


# \
-ftree-vectorize                \
-fpeel-loops                    \

# \
-fdump-tree-all                 \
-fdump-rtl-all                  \

# \
-fgcse-after-reload             \
-fipa-cp-clone                  \
-floop-interchange              \
-floop-unroll-and-jam           \
-fpredictive-commoning          \
-fsplit-paths                   \
-ftree-loop-distribute-patterns \
-ftree-loop-distribution        \
-ftree-partial-pre              \
-ftree-loop-vectorize           \
-ftree-slp-vectorize            \
-funswitch-loops                \
-fvect-cost-model               \
