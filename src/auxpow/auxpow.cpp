// Copyright (c) 2011 Vince Durham
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.


#include "init.h"
#include "primitives/block.h"
#include "chainparams.h"
#include "util.h"
#include "base58.h"
#include "auxpow.h"

using namespace std;
using namespace boost;

/** Global dirty block merged mining entries. */
map<uint256, std::shared_ptr<CAuxPow> > mapDirtyAuxPow;

unsigned char pchMergedMiningHeader[] = { 0xfa, 0xbe, 'm', 'm' } ;

int GetAuxPowStartBlock(const Consensus::Params& params)
{
    return params.nAuxPowStartHeight;
}

void RemoveMergedMiningHeader(vector<unsigned char>& vchAux)
{
    if (vchAux.begin() != std::search(vchAux.begin(), vchAux.end(), UBEGIN(pchMergedMiningHeader), UEND(pchMergedMiningHeader)))
        throw runtime_error("merged mining aux too short");
    vchAux.erase(vchAux.begin(), vchAux.begin() + sizeof(pchMergedMiningHeader));
}

uint256 CAuxPow::CheckMerkleBranch(const uint256& hash, const std::vector<uint256>& vMerkleBranch, int nIndex) const
{
  uint256 thash = hash;
  if (nIndex == -1)
    return uint256();
  for (std::vector<uint256>::const_iterator it(vMerkleBranch.begin()); it != vMerkleBranch.end(); ++it)
  {
    if (nIndex & 1)
      thash = Hash(BEGIN(*it), END(*it), BEGIN(thash), END(thash));
    else
      thash = Hash(BEGIN(thash), END(thash), BEGIN(*it), END(*it));
    nIndex >>= 1;
  }
  return thash;
}

bool CAuxPow::Check(const uint256& hashAuxBlock, int nChainID, const Consensus::Params& params) const
{
    if (nIndex != 0)
        return error("AuxPow is not a generate");
//    LogPrintf("CAuxPow::Check(): nChainID = %02x\n", nChainID); // LED TMP
//    LogPrintf("%s\n", parentBlockHeader.ToString()); // LED TMP
    if (!params.fPowAllowMinDifficultyBlocks && parentBlockHeader.GetChainID() == nChainID)
        return error("Aux POW parent has our chain ID");

//    LogPrintf("vChainMerkleBranch.size() = %d\n", vChainMerkleBranch.size()); // LED TMP
    if (vChainMerkleBranch.size() > 30)
        return error("Aux POW chain merkle branch too long");

    // Check that the chain merkle root is in the coinbase
    const uint256 nRootHash = CheckMerkleBranch(hashAuxBlock, vChainMerkleBranch, nChainIndex);
    vector<unsigned char> vchRootHash(nRootHash.begin(), nRootHash.end());
    std::reverse(vchRootHash.begin(), vchRootHash.end()); // correct endian

    // Check that we are in the parent block merkle tree
    if (CheckMerkleBranch(GetHash(), vMerkleBranch, nIndex) != parentBlockHeader.hashMerkleRoot)

        return error("Aux POW merkle root incorrect");

    const CScript script = vin[0].scriptSig;

    // Check that the same work is not submitted twice to our chain.
    //

    CScript::const_iterator pcHead =
        std::search(script.begin(), script.end(), UBEGIN(pchMergedMiningHeader), UEND(pchMergedMiningHeader));

    CScript::const_iterator pc =
        std::search(script.begin(), script.end(), vchRootHash.begin(), vchRootHash.end());

    if (pcHead == script.end())
        return error("MergedMiningHeader missing from parent coinbase");

    if (pc == script.end())
        return error("Aux POW missing chain merkle root in parent coinbase");

    if (pcHead != script.end())
    {
        // Enforce only one chain merkle root by checking that a single instance of the merged
        // mining header exists just before.
        if (script.end() != std::search(pcHead + 1, script.end(), UBEGIN(pchMergedMiningHeader), UEND(pchMergedMiningHeader)))
            return error("Multiple merged mining headers in coinbase");
        if (pcHead + sizeof(pchMergedMiningHeader) != pc)
            return error("Merged mining header is not just before chain merkle root");
    }
    else
    {
        // For backward compatibility.
        // Enforce only one chain merkle root by checking that it starts early in the coinbase.
        // 8-12 bytes are enough to encode extraNonce and nBits.
        if (pc - script.begin() > 20)
            return error("Aux POW chain merkle root must start in the first 20 bytes of the parent coinbase");
    }


    // Ensure we are at a deterministic point in the merkle leaves by hashing
    // a nonce and our chain ID and comparing to the index.
    pc += vchRootHash.size();
    if (script.end() - pc < 8)
        return error("Aux POW missing chain merkle tree size and nonce in parent coinbase");

    int nSize;
    memcpy(&nSize, &pc[0], 4);
    if (nSize != (1 << vChainMerkleBranch.size()))
        return error("Aux POW merkle branch size does not match parent coinbase");

    int nNonce;
    memcpy(&nNonce, &pc[4], 4);

    // Choose a pseudo-random slot in the chain merkle tree
    // but have it be fixed for a size/nonce/chain combination.
    //
    // This prevents the same work from being used twice for the
    // same chain while reducing the chance that two chains clash
    // for the same slot.
    unsigned int rand = nNonce;
    rand = rand * 1103515245 + 12345;
    rand += nChainID;
    rand = rand * 1103515245 + 12345;

    if (nChainIndex != (rand % nSize))
        return error("Aux POW wrong index");

    return true;
}

void CBlockHeader::SetAuxPow(CAuxPow* pow)
{
    if (pow != nullptr)
        nVersion |= AuxPow::BLOCK_VERSION_AUXPOW;
    else
        nVersion &= ~AuxPow::BLOCK_VERSION_AUXPOW;
    auxpow.reset(pow);
}

CKeyID GetAuxpowMiningKey(void)
{
    CKeyID result;
    CBitcoinAddress auxminingaddr(GetArg("-auxminingaddr", ""));
    if (!auxminingaddr.GetKeyID(result)) {
        CReserveKey reservekey(pwalletMain);
        CPubKey pubkey;
        reservekey.GetReservedKey(pubkey);
        result = pubkey.GetID();
    }
    return result;
}
