// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2026 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_BLOCKHEADER_H
#define BITCOIN_PRIMITIVES_BLOCKHEADER_H

#include <auxpow/consensus.h>
#include <serialize.h>
#include <uint256.h>
#include <util/time.h>

#include <memory>

class CAuxPow;

class CBlockHeader
{
public:
    int32_t nVersion;
    uint256 hashPrevBlock;
    uint256 hashMerkleRoot;
    uint32_t nTime;
    uint32_t nBits;
    uint32_t nNonce;
    std::shared_ptr<CAuxPow> auxpow;

    CBlockHeader()
    {
        SetNull();
    }

    // NOLINTNEXTLINE(misc-no-recursion)
    SERIALIZE_METHODS(CBlockHeader, obj)
    {
        READWRITE(obj.nVersion, obj.hashPrevBlock, obj.hashMerkleRoot, obj.nTime, obj.nBits, obj.nNonce);
        if (obj.IsAuxPow()) {
            // Safe shared_ptr serialization: on read, allocate before
            // deserializing into it.  On write, the auxpow data is
            // always present for fully-constructed headers (from P2P or
            // disk); for headers returned by CBlockIndex::GetBlockHeader()
            // (no auxpow in memory), IsAuxPow() is true but auxpow is
            // null -- the 6 basic fields are serialized, auxpow omitted.
            SER_READ(obj, obj.auxpow = std::make_shared<CAuxPow>());
            assert(obj.auxpow != nullptr);
            READWRITE(*obj.auxpow);
        } else {
            SER_READ(obj, obj.auxpow.reset());
        }
    }

    void SetNull()
    {
        nVersion = 0;
        hashPrevBlock.SetNull();
        hashMerkleRoot.SetNull();
        nTime = 0;
        nBits = 0;
        nNonce = 0;
        auxpow.reset();
    }

    bool IsNull() const
    {
        return nBits == 0;
    }

    uint256 GetHash() const;
    uint256 GetPoWHash() const;

    NodeSeconds Time() const
    {
        return NodeSeconds{std::chrono::seconds{nTime}};
    }

    int64_t GetBlockTime() const
    {
        return static_cast<int64_t>(nTime);
    }

    int GetChainID() const
    {
        return nVersion / AuxPow::BLOCK_VERSION_CHAIN_START;
    }

    bool IsAuxPow() const
    {
        return static_cast<bool>(nVersion & AuxPow::BLOCK_VERSION_AUXPOW);
    }

    void SetAuxPow(CAuxPow* pow);
};

#endif // BITCOIN_PRIMITIVES_BLOCKHEADER_H
