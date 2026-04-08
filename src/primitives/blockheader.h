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
    std::shared_ptr<const CAuxPow> auxpow;

    CBlockHeader()
    {
        SetNull();
    }

    SERIALIZE_METHODS(CBlockHeader, obj)
    {
        READWRITE(obj.nVersion, obj.hashPrevBlock, obj.hashMerkleRoot, obj.nTime, obj.nBits, obj.nNonce);
        if (obj.IsAuxPow()) {
            READWRITE(obj.auxpow);
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
