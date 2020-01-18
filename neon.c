#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __ARM_NEON
#include <arm_neon.h>

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

void *memset_neon(void *dest, int c, size_t n)
{
	unsigned char *s = (unsigned char *)dest;
	unsigned char *e = s + n;
	uint8x16_t uv;

	if (unlikely(n == 0))
		return dest;

	uv = vdupq_n_u8(c);
	if (unlikely(n <= 2)) {
		uint8_t uc = vgetq_lane_u8(uv, 0);
		*((uint8_t *)(s)) = uc;
		*((uint8_t *)(e - 1)) = uc;
	} else if (unlikely(n <= 4)) {
		uint16x8_t uvl = vreinterpretq_u16_u8(uv);
		uint16_t uc = vgetq_lane_u16(uvl, 0);
		*((uint16_t *)(s)) = uc;
		*((uint16_t *)(e - 2)) = uc;
	} else if (unlikely(n <= 8)) {
		uint32x4_t uvl = vreinterpretq_u32_u8(uv);
		uint32_t uc = vgetq_lane_u32(uvl, 0);
		*((uint32_t *)(s)) = uc;
		*((uint32_t *)(e - 4)) = uc;
	} else if (unlikely(n <= 16)) {
		uint64x2_t uvl = vreinterpretq_u64_u8(uv);
		uint64_t uc = vgetq_lane_u64(uvl, 0);
		*((uint64_t *)(s)) = uc;
		*((uint64_t *)(e - 8)) = uc;
	} else {
		for (; likely(s < e - 32); s += 32) {
			vst1q_u8(s, uv);
			vst1q_u8(s + 16, uv);
		}
		n = e - s;
		if (unlikely(n <= 16)) {
			vst1q_u8(e - 16, uv);
		} else {
			vst1q_u8(s, uv);
			vst1q_u8(e - 16, uv);
		}
	}

	return dest;
}
#else //__ARM_NEON
void *memset_neon(void *dest, int c, size_t n)
{
	return memset(dest, c, n);
}
#endif //__ARM_NEON
