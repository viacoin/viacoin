// Copyright (c) 2009 Colin Percival, 2011 ArtForz, 2012-2013 pooler
// Copyright (c) 2026 The Viacoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include <crypto/hmac_sha256.h>
#include <crypto/scrypt.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

#if defined(USE_SSE2) && !defined(USE_SSE2_ALWAYS)
#ifdef _MSC_VER
#include <intrin.h>
#else
#include <cpuid.h>
#endif
#endif

namespace {
static inline void be32enc(void* pp, uint32_t x)
{
    auto* p = static_cast<uint8_t*>(pp);
    p[3] = x & 0xff;
    p[2] = (x >> 8) & 0xff;
    p[1] = (x >> 16) & 0xff;
    p[0] = (x >> 24) & 0xff;
}

#define ROTL(a, b) (((a) << (b)) | ((a) >> (32 - (b))))

static inline void xor_salsa8(uint32_t B[16], const uint32_t Bx[16])
{
    uint32_t x00, x01, x02, x03, x04, x05, x06, x07, x08, x09, x10, x11, x12, x13, x14, x15;

    x00 = (B[0] ^= Bx[0]);
    x01 = (B[1] ^= Bx[1]);
    x02 = (B[2] ^= Bx[2]);
    x03 = (B[3] ^= Bx[3]);
    x04 = (B[4] ^= Bx[4]);
    x05 = (B[5] ^= Bx[5]);
    x06 = (B[6] ^= Bx[6]);
    x07 = (B[7] ^= Bx[7]);
    x08 = (B[8] ^= Bx[8]);
    x09 = (B[9] ^= Bx[9]);
    x10 = (B[10] ^= Bx[10]);
    x11 = (B[11] ^= Bx[11]);
    x12 = (B[12] ^= Bx[12]);
    x13 = (B[13] ^= Bx[13]);
    x14 = (B[14] ^= Bx[14]);
    x15 = (B[15] ^= Bx[15]);

    for (int i = 0; i < 8; i += 2) {
        x04 ^= ROTL(x00 + x12, 7);  x09 ^= ROTL(x05 + x01, 7);
        x14 ^= ROTL(x10 + x06, 7);  x03 ^= ROTL(x15 + x11, 7);

        x08 ^= ROTL(x04 + x00, 9);  x13 ^= ROTL(x09 + x05, 9);
        x02 ^= ROTL(x14 + x10, 9);  x07 ^= ROTL(x03 + x15, 9);

        x12 ^= ROTL(x08 + x04, 13); x01 ^= ROTL(x13 + x09, 13);
        x06 ^= ROTL(x02 + x14, 13); x11 ^= ROTL(x07 + x03, 13);

        x00 ^= ROTL(x12 + x08, 18); x05 ^= ROTL(x01 + x13, 18);
        x10 ^= ROTL(x06 + x02, 18); x15 ^= ROTL(x11 + x07, 18);

        x01 ^= ROTL(x00 + x03, 7);  x06 ^= ROTL(x05 + x04, 7);
        x11 ^= ROTL(x10 + x09, 7);  x12 ^= ROTL(x15 + x14, 7);

        x02 ^= ROTL(x01 + x00, 9);  x07 ^= ROTL(x06 + x05, 9);
        x08 ^= ROTL(x11 + x10, 9);  x13 ^= ROTL(x12 + x15, 9);

        x03 ^= ROTL(x02 + x01, 13); x04 ^= ROTL(x07 + x06, 13);
        x09 ^= ROTL(x08 + x11, 13); x14 ^= ROTL(x13 + x12, 13);

        x00 ^= ROTL(x03 + x02, 18); x05 ^= ROTL(x04 + x07, 18);
        x10 ^= ROTL(x09 + x08, 18); x15 ^= ROTL(x14 + x13, 18);
    }

    B[0] += x00;  B[1] += x01;  B[2] += x02;  B[3] += x03;
    B[4] += x04;  B[5] += x05;  B[6] += x06;  B[7] += x07;
    B[8] += x08;  B[9] += x09;  B[10] += x10; B[11] += x11;
    B[12] += x12; B[13] += x13; B[14] += x14; B[15] += x15;
}
#undef ROTL
} // namespace

void PBKDF2_SHA256(const uint8_t* passwd,
                   size_t passwdlen,
                   const uint8_t* salt,
                   size_t saltlen,
                   uint64_t c,
                   uint8_t* buf,
                   size_t dkLen)
{
    CHMAC_SHA256 ps_hctx(passwd, passwdlen);
    ps_hctx.Write(salt, saltlen);

    for (size_t i = 0; i * CHMAC_SHA256::OUTPUT_SIZE < dkLen; ++i) {
        uint8_t ivec[4];
        be32enc(ivec, static_cast<uint32_t>(i + 1));

        auto hctx = ps_hctx;
        hctx.Write(ivec, sizeof(ivec));

        uint8_t u[CHMAC_SHA256::OUTPUT_SIZE];
        hctx.Finalize(u);

        uint8_t t[CHMAC_SHA256::OUTPUT_SIZE];
        std::memcpy(t, u, sizeof(t));

        for (uint64_t j = 2; j <= c; ++j) {
            CHMAC_SHA256 iter_hctx(passwd, passwdlen);
            iter_hctx.Write(u, sizeof(u));
            iter_hctx.Finalize(u);
            for (size_t k = 0; k < sizeof(t); ++k) t[k] ^= u[k];
        }

        size_t clen = dkLen - i * CHMAC_SHA256::OUTPUT_SIZE;
        if (clen > CHMAC_SHA256::OUTPUT_SIZE) clen = CHMAC_SHA256::OUTPUT_SIZE;
        std::memcpy(&buf[i * CHMAC_SHA256::OUTPUT_SIZE], t, clen);
    }
}

void scrypt_1024_1_1_256_sp_generic(const char* input, char* output, char* scratchpad)
{
    std::array<uint8_t, 128> B;
    std::array<uint32_t, 32> X;
    auto* V = reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(scratchpad) + 63) & ~uintptr_t{63});

    PBKDF2_SHA256(reinterpret_cast<const uint8_t*>(input), 80, reinterpret_cast<const uint8_t*>(input), 80, 1, B.data(), B.size());

    for (uint32_t k = 0; k < 32; ++k) X[k] = le32dec(&B[4 * k]);

    for (uint32_t i = 0; i < 1024; ++i) {
        std::memcpy(&V[i * 32], X.data(), 128);
        xor_salsa8(&X[0], &X[16]);
        xor_salsa8(&X[16], &X[0]);
    }
    for (uint32_t i = 0; i < 1024; ++i) {
        uint32_t j = 32 * (X[16] & 1023);
        for (uint32_t k = 0; k < 32; ++k) X[k] ^= V[j + k];
        xor_salsa8(&X[0], &X[16]);
        xor_salsa8(&X[16], &X[0]);
    }

    for (uint32_t k = 0; k < 32; ++k) le32enc(&B[4 * k], X[k]);

    PBKDF2_SHA256(reinterpret_cast<const uint8_t*>(input), 80, B.data(), B.size(), 1, reinterpret_cast<uint8_t*>(output), 32);
}

#if defined(USE_SCRYPT_AVX2)
// By default, set to generic scrypt function. This will prevent crash in case
// when scrypt_detect_avx2() wasn't called in the non-USE_SCRYPT_AVX2_ALWAYS path.
void (*scrypt_1024_1_1_256_sp_detected)(const char* input, char* output, char* scratchpad) = &scrypt_1024_1_1_256_sp_generic;

std::string scrypt_detect_avx2()
{
    std::string ret;
#if defined(USE_SCRYPT_AVX2_ALWAYS)
    ret = "scrypt: using scrypt-avx2 as built-in";
#else // USE_SCRYPT_AVX2_ALWAYS
    // 32-bit x86 — detect cpuid AVX2 feature
    unsigned int cpuid_ebx = 0;
#if defined(_MSC_VER)
    int x86cpuid[4];
    __cpuidex(x86cpuid, 7, 0);
    cpuid_ebx = static_cast<unsigned int>(x86cpuid[1]);
#else // _MSC_VER
    unsigned int eax, ecx, edx;
    __get_cpuid_count(7, 0, &eax, &cpuid_ebx, &ecx, &edx);
#endif // _MSC_VER

    if (cpuid_ebx & (1 << 5)) {
        scrypt_1024_1_1_256_sp_detected = &scrypt_1024_1_1_256_sp_avx2;
        ret = "scrypt: using scrypt-avx2 as detected";
    } else {
        scrypt_1024_1_1_256_sp_detected = &scrypt_1024_1_1_256_sp_generic;
        ret = "scrypt: using scrypt-generic, AVX2 unavailable";
    }
#endif // USE_SCRYPT_AVX2_ALWAYS
    return ret;
}
#elif defined(USE_SSE2)
// By default, set to generic scrypt function. This will prevent crash in case
// when scrypt_detect_sse2() wasn't called in the non-USE_SSE2_ALWAYS path.
void (*scrypt_1024_1_1_256_sp_detected)(const char* input, char* output, char* scratchpad) = &scrypt_1024_1_1_256_sp_generic;

std::string scrypt_detect_sse2()
{
    std::string ret;
#if defined(USE_SSE2_ALWAYS)
    ret = "scrypt: using scrypt-sse2 as built-in";
#else // USE_SSE2_ALWAYS
    // 32-bit x86 Linux or Windows — detect cpuid features
    unsigned int cpuid_edx = 0;
#if defined(_MSC_VER)
    // MSVC
    int x86cpuid[4];
    __cpuid(x86cpuid, 1);
    cpuid_edx = static_cast<unsigned int>(x86cpuid[3]);
#else // _MSC_VER
    // Linux or i686-w64-mingw32 (gcc/clang)
    unsigned int eax, ebx, ecx;
    __get_cpuid(1, &eax, &ebx, &ecx, &cpuid_edx);
#endif // _MSC_VER

    if (cpuid_edx & (1 << 26)) {
        scrypt_1024_1_1_256_sp_detected = &scrypt_1024_1_1_256_sp_sse2;
        ret = "scrypt: using scrypt-sse2 as detected";
    } else {
        scrypt_1024_1_1_256_sp_detected = &scrypt_1024_1_1_256_sp_generic;
        ret = "scrypt: using scrypt-generic, SSE2 unavailable";
    }
#endif // USE_SSE2_ALWAYS
    return ret;
}
#endif // USE_SSE2

void scrypt_1024_1_1_256(const char* input, char* output)
{
    char scratchpad[SCRYPT_SCRATCHPAD_SIZE];
    scrypt_1024_1_1_256_sp(input, output, scratchpad);
}
