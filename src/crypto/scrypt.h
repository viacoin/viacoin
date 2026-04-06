// Copyright (c) 2026 The Viacoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef BITCOIN_CRYPTO_SCRYPT_H
#define BITCOIN_CRYPTO_SCRYPT_H

#include <cstddef>
#include <cstdint>

inline constexpr int SCRYPT_SCRATCHPAD_SIZE = 131072 + 63;

void scrypt_1024_1_1_256(const char* input, char* output);
void scrypt_1024_1_1_256_sp_generic(const char* input, char* output, char* scratchpad);

void PBKDF2_SHA256(const uint8_t* passwd,
                   size_t passwdlen,
                   const uint8_t* salt,
                   size_t saltlen,
                   uint64_t c,
                   uint8_t* buf,
                   size_t dkLen);

static inline uint32_t le32dec(const void* pp)
{
    const auto* p = static_cast<const uint8_t*>(pp);
    return (uint32_t{p[0]}) |
           (uint32_t{p[1]} << 8) |
           (uint32_t{p[2]} << 16) |
           (uint32_t{p[3]} << 24);
}

static inline void le32enc(void* pp, uint32_t x)
{
    auto* p = static_cast<uint8_t*>(pp);
    p[0] = x & 0xff;
    p[1] = (x >> 8) & 0xff;
    p[2] = (x >> 16) & 0xff;
    p[3] = (x >> 24) & 0xff;
}

#endif // BITCOIN_CRYPTO_SCRYPT_H
