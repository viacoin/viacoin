// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_BLOCK_H
#define BITCOIN_PRIMITIVES_BLOCK_H

#include <memory>
#include "primitives/transaction.h"
#include "primitives/blockheader.h"
#include "auxpow/auxpow.h"
#include "auxpow/consensus.h"
#include "auxpow/serialize.h"
#include "serialize.h"
#include "uint256.h"
#include "versionbits.h"


/** Nodes collect new transactions into a block, hash them into a hash tree,
 * and scan through nonce values to make the block's hash satisfy proof-of-work
 * requirements.  When they solve the proof-of-work, they broadcast the block
 * to everyone and the block is added to the block chain.  The first transaction
 * in the block is a special one that creates a new coin owned by the creator
 * of the block.
 *
 */


/*
 * A block consists of a BlockHeader followed by Block data (the transaction list)
 */

class CBlock : public CBlockHeader
{
public:
    // network and disk
    std::vector<CTransaction> vtx;

    // memory only
    mutable bool fChecked;

    // TODO LED .. fix this. The MerkleTree functions in class CBlock
    // TODO LED have been ported from 0.10 to 0.13, but 0.13 does things
    // TODO LED differently now.. see the new MerkleTree handling and
    // TODO LED associated classes for a hint on a proper fix. This code
    // TODO LED as is probably won't work. vMerkleTree was replaced with
    // TODO LED a bool fChecked. So grafting vMerkleTree back in just
    // TODO LED to be used by the auxpow tree is most likely wrong. 
    mutable std::vector<uint256> vMerkleTree;

    CBlock()
    {
        SetNull();
    }

    CBlock(const CBlockHeader &header)
    {
        SetNull();
        *((CBlockHeader*)this) = header;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(*(CBlockHeader*)this);
        READWRITE(vtx);
    }

    void SetNull()
    {
        CBlockHeader::SetNull();
        vtx.clear();
        fChecked = false;
    }

    CBlockHeader GetBlockHeader() const
    {
        CBlockHeader block;
        block.nVersion       = nVersion;
        block.hashPrevBlock  = hashPrevBlock;
        block.hashMerkleRoot = hashMerkleRoot;
        block.nTime          = nTime;
        block.nBits          = nBits;
        block.nNonce         = nNonce;
        block.auxpow         = auxpow;
                
        return block;
    }

    // TODO LED these two Merkle functions are temporary.
    // TODO LED BuildMerkleTree, GetMerkleBranch and CheckMerkleBranch
    // TODO LED The callers of these functions (in auxpow.cpp) need
    // TODO LED to be ported to the new style Merkle classes
    uint256 BuildMerkleTree(bool* mutated = NULL) const;
    std::vector<uint256> GetMerkleBranch(int nIndex) const;

    std::string ToString() const;
};

/** Describes a place in the block chain to another node such that if the
 * other node doesn't have the same branch, it can find a recent common trunk.
 * The further back it is, the further before the fork it may be.
 */
struct CBlockLocator
{
    std::vector<uint256> vHave;

    CBlockLocator() {}

    CBlockLocator(const std::vector<uint256>& vHaveIn)
    {
        vHave = vHaveIn;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vHave);
    }

    void SetNull()
    {
        vHave.clear();
    }

    bool IsNull() const
    {
        return vHave.empty();
    }
};

/** Compute the consensus-critical block weight (see BIP 141). */
int64_t GetBlockWeight(const CBlock& tx);

#endif // BITCOIN_PRIMITIVES_BLOCK_H
