// Copyright (c) 2009-2010 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_AUXPOW_AUXPOW_H
#define BITCOIN_AUXPOW_AUXPOW_H

#include "versionbits.h"
#include "consensus/params.h"
#include "wallet/wallet.h"
#include "primitives/blockheader.h"
#include "auxpow/consensus.h"


class CAuxPow : public CMerkleTx
{
public:
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
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(*(CMerkleTx*)this);
        READWRITE(vChainMerkleBranch);
        READWRITE(nChainIndex);
        // Always serialize the saved parent block as header so that the size of CAuxPow
        // is consistent.
        READWRITE(parentBlockHeader);
    }

    uint256 CheckMerkleBranch(const uint256& hash, const std::vector<uint256>& vMerkleBranch, int nIndex) const;

    inline uint256 GetParentBlockHash()
    {
        return parentBlockHeader.GetPoWHash();
    }
};

template<typename Stream> void SerReadWrite(Stream& s, std::shared_ptr<CAuxPow>& pobj, CSerActionSerialize _)
{
    ::Serialize(s, *pobj);
}

template<typename Stream> void SerReadWrite(Stream& s, std::shared_ptr<CAuxPow>& pobj, CSerActionUnserialize _)
{
    pobj.reset(new CAuxPow());
    ::Unserialize(s, *pobj);
}

#endif // BITCOIN_AUXPOW_AUXPOW_H
