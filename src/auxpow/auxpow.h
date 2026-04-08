// Copyright (c) 2011 Vince Durham
// Copyright (c) 2014-2026 The Viacoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef BITCOIN_AUXPOW_AUXPOW_H
#define BITCOIN_AUXPOW_AUXPOW_H

#include <hash.h>
#include <primitives/blockheader.h>
#include <primitives/transaction.h>
#include <uint256.h>

#include <vector>

class CAuxPow
{
public:
    CTransactionRef tx;
    std::vector<uint256> vMerkleBranch;
    int nIndex{0};
    std::vector<uint256> vChainMerkleBranch;
    unsigned int nChainIndex{0};
    CBlockHeader parentBlockHeader;

    CAuxPow() = default;

    template <typename Stream>
    CAuxPow(deserialize_type, Stream& s)
    {
        Unserialize(s);
    }

    SERIALIZE_METHODS(CAuxPow, obj)
    {
        READWRITE(TX_WITH_WITNESS(obj.tx), obj.vMerkleBranch, obj.nIndex, obj.vChainMerkleBranch, obj.nChainIndex, obj.parentBlockHeader);
    }

    uint256 CheckMerkleBranch(const uint256& hash, const std::vector<uint256>& merkle_branch, int index) const
    {
        if (index == -1) {
            return {};
        }

        uint256 current = hash;
        for (const auto& branch_hash : merkle_branch) {
            current = (index & 1) ? Hash(branch_hash, current) : Hash(current, branch_hash);
            index >>= 1;
        }
        return current;
    }

    uint256 GetParentBlockHash() const
    {
        return parentBlockHeader.GetPoWHash();
    }
};

inline void CBlockHeader::SetAuxPow(CAuxPow* pow)
{
    if (pow != nullptr) {
        nVersion |= AuxPow::BLOCK_VERSION_AUXPOW;
    } else {
        nVersion &= ~AuxPow::BLOCK_VERSION_AUXPOW;
    }
    auxpow.reset(pow);
}

#endif // BITCOIN_AUXPOW_AUXPOW_H
