// Copyright (c) 2009 Colin Percival, 2011 ArtForz, 2012-2013 pooler
// Copyright (c) 2026 The Viacoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

// AVX2-optimised scrypt: uses 256-bit SIMD for V-array stores/loads/XOR
// (the memory-bandwidth-bound portion), while the Salsa20/8 core stays
// in 128-bit SSE2 form — VEX-encoded when compiled with -mavx2, so there
// is no SSE/AVX transition penalty.  The two xor_salsa8 calls per loop
// iteration are sequential (the second uses the output of the first), so
// they cannot be folded into one 256-bit Salsa20 pass.

#if defined(USE_SCRYPT_AVX2)

#include <crypto/hmac_sha256.h>
#include <crypto/scrypt.h>

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <emmintrin.h>   // __m128i  (SSE2 — always available on AVX2 CPUs)
#include <immintrin.h>   // __m256i  (AVX2)

/* ------------------------------------------------------------------ */
/*  Salsa20/8 core — identical algorithm to scrypt-sse2.cpp, but      */
/*  compiled with -mavx2 so every _mm_* intrinsic is VEX-encoded.     */
/* ------------------------------------------------------------------ */
static inline void xor_salsa8_sse2(__m128i B[4], const __m128i Bx[4])
{
    __m128i X0, X1, X2, X3;
    __m128i T;

    X0 = B[0] = _mm_xor_si128(B[0], Bx[0]);
    X1 = B[1] = _mm_xor_si128(B[1], Bx[1]);
    X2 = B[2] = _mm_xor_si128(B[2], Bx[2]);
    X3 = B[3] = _mm_xor_si128(B[3], Bx[3]);

    for (int i = 0; i < 8; i += 2) {
        /* Operate on "columns". */
        T = _mm_add_epi32(X0, X3);
        X1 = _mm_xor_si128(X1, _mm_slli_epi32(T, 7));
        X1 = _mm_xor_si128(X1, _mm_srli_epi32(T, 25));
        T = _mm_add_epi32(X1, X0);
        X2 = _mm_xor_si128(X2, _mm_slli_epi32(T, 9));
        X2 = _mm_xor_si128(X2, _mm_srli_epi32(T, 23));
        T = _mm_add_epi32(X2, X1);
        X3 = _mm_xor_si128(X3, _mm_slli_epi32(T, 13));
        X3 = _mm_xor_si128(X3, _mm_srli_epi32(T, 19));
        T = _mm_add_epi32(X3, X2);
        X0 = _mm_xor_si128(X0, _mm_slli_epi32(T, 18));
        X0 = _mm_xor_si128(X0, _mm_srli_epi32(T, 14));

        /* Rearrange data. */
        X1 = _mm_shuffle_epi32(X1, 0x93);
        X2 = _mm_shuffle_epi32(X2, 0x4E);
        X3 = _mm_shuffle_epi32(X3, 0x39);

        /* Operate on "rows". */
        T = _mm_add_epi32(X0, X1);
        X3 = _mm_xor_si128(X3, _mm_slli_epi32(T, 7));
        X3 = _mm_xor_si128(X3, _mm_srli_epi32(T, 25));
        T = _mm_add_epi32(X3, X0);
        X2 = _mm_xor_si128(X2, _mm_slli_epi32(T, 9));
        X2 = _mm_xor_si128(X2, _mm_srli_epi32(T, 23));
        T = _mm_add_epi32(X2, X3);
        X1 = _mm_xor_si128(X1, _mm_slli_epi32(T, 13));
        X1 = _mm_xor_si128(X1, _mm_srli_epi32(T, 19));
        T = _mm_add_epi32(X1, X2);
        X0 = _mm_xor_si128(X0, _mm_slli_epi32(T, 18));
        X0 = _mm_xor_si128(X0, _mm_srli_epi32(T, 14));

        /* Rearrange data. */
        X1 = _mm_shuffle_epi32(X1, 0x39);
        X2 = _mm_shuffle_epi32(X2, 0x4E);
        X3 = _mm_shuffle_epi32(X3, 0x93);
    }

    B[0] = _mm_add_epi32(B[0], X0);
    B[1] = _mm_add_epi32(B[1], X1);
    B[2] = _mm_add_epi32(B[2], X2);
    B[3] = _mm_add_epi32(B[3], X3);
}

/* ------------------------------------------------------------------ */
/*  scrypt_1024_1_1_256_sp_avx2                                       */
/*                                                                    */
/*  V-array is stored as __m256i (4 registers per entry instead of    */
/*  8 __m128i), halving the number of stores/loads/XORs in the two    */
/*  main loops — this is where the real bandwidth win comes from.     */
/* ------------------------------------------------------------------ */
void scrypt_1024_1_1_256_sp_avx2(const char* input, char* output, char* scratchpad)
{
    uint8_t B[128];
    union {
        __m128i i128[8];
        uint32_t u32[32];
    } X;
    auto* V = reinterpret_cast<__m256i*>(
        (reinterpret_cast<uintptr_t>(scratchpad) + 63) & ~uintptr_t{63});

    PBKDF2_SHA256(reinterpret_cast<const uint8_t*>(input), 80,
                  reinterpret_cast<const uint8_t*>(input), 80, 1, B, 128);

    for (uint32_t k = 0; k < 2; k++) {
        for (uint32_t i = 0; i < 16; i++) {
            X.u32[k * 16 + i] = le32dec(&B[(k * 16 + (i * 5 % 16)) * 4]);
        }
    }

    for (uint32_t i = 0; i < 1024; i++) {
        /* Store X into V using 256-bit writes (4 instead of 8). */
        _mm256_store_si256(&V[i * 4 + 0], _mm256_loadu_si256(reinterpret_cast<__m256i*>(&X.i128[0])));
        _mm256_store_si256(&V[i * 4 + 1], _mm256_loadu_si256(reinterpret_cast<__m256i*>(&X.i128[2])));
        _mm256_store_si256(&V[i * 4 + 2], _mm256_loadu_si256(reinterpret_cast<__m256i*>(&X.i128[4])));
        _mm256_store_si256(&V[i * 4 + 3], _mm256_loadu_si256(reinterpret_cast<__m256i*>(&X.i128[6])));

        xor_salsa8_sse2(&X.i128[0], &X.i128[4]);
        xor_salsa8_sse2(&X.i128[4], &X.i128[0]);
    }

    for (uint32_t i = 0; i < 1024; i++) {
        uint32_t j = 4 * (X.u32[16] & 1023);

        /* XOR mix using 256-bit loads (4 instead of 8). */
        for (uint32_t k = 0; k < 4; k++) {
            __m256i vk = _mm256_load_si256(&V[j + k]);
            __m256i xk = _mm256_loadu_si256(reinterpret_cast<__m256i*>(&X.i128[k * 2]));
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(&X.i128[k * 2]),
                                _mm256_xor_si256(xk, vk));
        }

        xor_salsa8_sse2(&X.i128[0], &X.i128[4]);
        xor_salsa8_sse2(&X.i128[4], &X.i128[0]);
    }

    for (uint32_t k = 0; k < 2; k++) {
        for (uint32_t i = 0; i < 16; i++) {
            le32enc(&B[(k * 16 + (i * 5 % 16)) * 4], X.u32[k * 16 + i]);
        }
    }

    PBKDF2_SHA256(reinterpret_cast<const uint8_t*>(input), 80, B, 128, 1,
                  reinterpret_cast<uint8_t*>(output), 32);
}

#endif // USE_SCRYPT_AVX2
