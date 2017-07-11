// Copyright (c) 2009-2010 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_AUXPOW_H
#define BITCOIN_AUXPOW_H

#include <memory.h>
#include "versionbits.h"
#include "consensus/params.h"
#include "wallet/wallet.h"
#include "primitives/blockheader.h"
#include "auxpow/consensus.h"
#include "serialize.h"


class CAuxPow : public CMerkleTx
{
public:
    CAuxPow(const CTransaction& txIn) : CMerkleTx(txIn)
    {
    }

    CAuxPow() :CMerkleTx()
    {
    }

    // Merkle branch with root vchAux
    // root must be present inside the coinbase
    std::vector<uint256> vChainMerkleBranch;

    // Index of chain in chains merkle tree
    unsigned int nChainIndex;
    CBlockHeader parentBlockHeader;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(*(CMerkleTx*)this);
        READWRITE(vChainMerkleBranch);
        READWRITE(nChainIndex);
        // Always serialize the saved parent block as header so that the size of CAuxPow
        // is consistent.
        READWRITE(parentBlockHeader);
    }

    uint256 CheckMerkleBranch(const uint256& hash, const std::vector<uint256>& vMerkleBranch, int nIndex) const;
    bool Check(const uint256& hashAuxBlock, int nChainID, const Consensus::Params& params) const;

    inline uint256 GetParentBlockHash()
    {
            return parentBlockHeader.GetPoWHash();
//            return uint256();
    }
};

template<typename Stream> void SerReadWrite(Stream& s, std::shared_ptr<CAuxPow>& pobj, int nType, int nVersion, CSerActionSerialize ser_action)
{
    if (nVersion & AuxPow::BLOCK_VERSION_AUXPOW){
        ::Serialize(s, *pobj, nType, nVersion);
    }
}

template<typename Stream> void SerReadWrite(Stream& s, std::shared_ptr<CAuxPow>& pobj, int nType, int nVersion, CSerActionUnserialize ser_action)
{
    if (nVersion & AuxPow::BLOCK_VERSION_AUXPOW){
        pobj.reset(new CAuxPow());
        ::Unserialize(s, *pobj, nType, nVersion);
    } else
        pobj.reset();
}


extern void RemoveMergedMiningHeader(std::vector<unsigned char>& vchAux);
extern int GetAuxPowStartBlock(const Consensus::Params& params);
extern CKeyID GetAuxpowMiningKey(void);

#endif
