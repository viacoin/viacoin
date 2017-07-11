// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_BLOCKHEADER_H
#define BITCOIN_PRIMITIVES_BLOCKHEADER_H

#include <memory.h>

#include "versionbits.h"
#include "auxpow/consensus.h"
#include "auxpow/serialize.h"
#include "tinyformat.h"


class CBlockHeader
{
public:
    // header
    static const auto CURRENT_VERSION=0;
    int32_t nVersion;
    uint256 hashPrevBlock;
    uint256 hashMerkleRoot;
    uint32_t nTime;
    uint32_t nBits;
    uint32_t nNonce;
    std::shared_ptr<CAuxPow> auxpow;

    inline CBlockHeader()
    {
        SetNull();
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(this->nVersion);
        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);
        nVersion = this->nVersion;
        // the auxpow block is not serialized as part of the hash
        if ((!(nType & SER_GETHASH)) && this->IsAuxPow())
            READWRITE(auxpow);
    }

    inline void SetNull()
    {
        nVersion = 0;
        hashPrevBlock.SetNull();
        hashMerkleRoot.SetNull();
        nTime = 0;
        nBits = 0;
        nNonce = 0;
        auxpow = 0;
    }

    inline bool IsNull() const
    {
        return (nBits == 0);
    }

    uint256 GetHash() const;

    uint256 GetPoWHash() const;

    inline int64_t GetBlockTime() const
    {
        return (int64_t)nTime;
    }

    inline int GetChainID() const
    {
        return nVersion / AuxPow::BLOCK_VERSION_CHAIN_START;
    }

    inline bool IsAuxPow() const
    {
            return nVersion & AuxPow::BLOCK_VERSION_AUXPOW;
    }

    void SetAuxPow(CAuxPow*);

    std::string ToString() const
    {
        std::string str ="\nCBlockHeader(\n";
        str += strprintf("                nVersion=%d (0x%08x)\n", nVersion, nVersion);
        str += strprintf("                nTime=%d (0x%08x), nBits=%d (0x%08x), nNonce=%d (0x%08x)\n",
                         nTime, nTime, nBits, nBits, nNonce, nNonce);
        str += strprintf("                hashBlock=%s\n",    GetHash().ToString());
        str += strprintf("                MerkleRoot=%s\n",   hashMerkleRoot.ToString());
        str += strprintf("                hashPrev=%s\n",     hashPrevBlock.ToString());
        str += strprintf("                PoWHash=%s\n",      GetPoWHash().ToString());
        str += strprintf("                auxpow=0x%08x )\n", auxpow);
        return str;
    }

};


#endif // BITCOIN_PRIMITIVES_BLOCKHEADER_H
