#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __ARM_NEON
#include <arm_neon.h>

void *memset_neon(void *dest, int c, size_t n)
{
	unsigned char *s = (unsigned char *)dest;
	unsigned char *e = s + n;
	uint8x16_t uv;
	size_t r = n;

	uv = vdupq_n_u8(c);
	if (n >= 16) {
		for (; r >= 32; r -= 32, s += 32) {
			vst1q_u8(s, uv);
			vst1q_u8(s + 16, uv);
		}
		if (r <= 8) {
			uint64x2_t uvl = vreinterpretq_u64_u8(uv);
			uint64_t uc = vgetq_lane_u64(uvl, 0);
			*((uint64_t *)(e - 8)) = uc;
		} else if (r <= 16) {
			vst1q_u8(e - 16, uv);
		} else {
			vst1q_u8(s, uv);
			vst1q_u8(e - 16, uv);
		}
	} else {
		if (n == 0)
			return dest;
		uint64x2_t uvl = vreinterpretq_u64_u8(uv);
		uint64_t uc = vgetq_lane_u64(uvl, 0);
		if (n <= 2) {
			//uint8_t uc = vgetq_lane_u8(uv, 0);
			*((uint8_t *)(s)) = uc;
			*((uint8_t *)(e - 1)) = uc;
		} else if (n <= 4) {
			//uint16x8_t uvl = vreinterpretq_u16_u8(uv);
			//uint16_t uc = vgetq_lane_u16(uvl, 0);
			*((uint16_t *)(s)) = uc;
			*((uint16_t *)(e - 2)) = uc;
		} else if (n <= 8) {
			//uint32x4_t uvl = vreinterpretq_u32_u8(uv);
			//uint32_t uc = vgetq_lane_u32(uvl, 0);
			*((uint32_t *)(s)) = uc;
			*((uint32_t *)(e - 4)) = uc;
		} else {
			*((uint64_t *)(s)) = uc;
			*((uint64_t *)(e - 8)) = uc;
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
