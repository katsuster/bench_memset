#include "dep.h"

#if IS_IN (libc)
# define VEC_SIZE	64
# define VEC(i)		zmm##i
# define VMOVU		vmovdqu64
# define VMOVA		vmovdqa64

# define MEMSET_VDUP_TO_VEC0_AND_SET_RETURN(d, r) \
  vmovd d, %xmm0; \
  movq r, %rax; \
  vpbroadcastb %xmm0, %xmm0; \
  vpbroadcastq %xmm0, %zmm0

# define WMEMSET_VDUP_TO_VEC0_AND_SET_RETURN(d, r) \
  vmovd d, %xmm0; \
  movq r, %rax; \
  vpbroadcastd %xmm0, %xmm0; \
  vpbroadcastq %xmm0, %zmm0

# define SECTION(p)		p##.avx512
# define MEMSET_SYMBOL(p,s)	p##_avx512_##s
# define WMEMSET_SYMBOL(p,s)	p##_avx512_##s

# include "memset-vec-unaligned-erms.S"
#endif
