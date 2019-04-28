#ifndef VIACOIN_BLOCKHEADER_H
#define VIACOIN_BLOCKHEADER_H

#include <primitives/transaction.h>
#include <auxpow/consensus.h>
#include <auxpow/serialize.h>

/** Nodes collect new transactions into a block, hash them into a hash tree,
 * and scan through nonce values to make the block's hash satisfy proof-of-work
 * requirements.  When they solve the proof-of-work, they broadcast the block
 * to everyone and the block is added to the block chain.  The first transaction
 * in the block is a special one that creates a new coin owned by the creator
 * of the block.
 */
class CBlockHeader
{
public:
    // header
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

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(this->nVersion);
        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);
        // the auxpow block is not serialized as part of the hash
        if ((!(s.GetType() & SER_GETHASH)) && this->IsAuxPow()) {
            READWRITE(auxpow);
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
    }

    bool IsNull() const
    {
        return (nBits == 0);
    }

    uint256 GetHash() const;

    uint256 GetPoWHash() const;

    int64_t GetBlockTime() const
    {
        return (int64_t)nTime;
    }

    inline int GetChainID() const
    {
        return nVersion / AuxPow::BLOCK_VERSION_CHAIN_START;
    }

    inline bool IsAuxPow() const
    {
        return static_cast<bool>(nVersion & AuxPow::BLOCK_VERSION_AUXPOW);
    }

    void SetAuxPow(CAuxPow*);
};


#endif //VIACOIN_BLOCKHEADER_H
