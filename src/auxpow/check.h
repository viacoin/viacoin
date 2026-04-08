// Copyright (c) 2009-2010 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef BITCOIN_AUXPOW_CHECK_H
#define BITCOIN_AUXPOW_CHECK_H

#include <auxpow/auxpow.h>
#include <consensus/params.h>
#include <crypto/common.h>
#include <script/script.h>

#include <algorithm>

namespace AuxPow {
inline constexpr unsigned char MERGED_MINING_HEADER[] = {0xfa, 0xbe, 'm', 'm'};
}

inline bool CheckAuxpow(const std::shared_ptr<const CAuxPow>& auxpow, const uint256& hash_aux_block, int chain_id, const Consensus::Params& params)
{
    if (!auxpow || !auxpow->tx || auxpow->tx->vin.empty()) {
        return false;
    }
    if (auxpow->nIndex != 0) {
        return false;
    }
    if (!params.fPowAllowMinDifficultyBlocks && auxpow->parentBlockHeader.GetChainID() == chain_id) {
        return false;
    }
    if (auxpow->vChainMerkleBranch.size() > 30) {
        return false;
    }

    const uint256 chain_root = auxpow->CheckMerkleBranch(hash_aux_block, auxpow->vChainMerkleBranch, auxpow->nChainIndex);
    std::vector<unsigned char> chain_root_bytes(chain_root.begin(), chain_root.end());
    std::reverse(chain_root_bytes.begin(), chain_root_bytes.end());

    if (auxpow->CheckMerkleBranch(auxpow->tx->GetHash().ToUint256(), auxpow->vMerkleBranch, auxpow->nIndex) != auxpow->parentBlockHeader.hashMerkleRoot) {
        return false;
    }

    const CScript& script = auxpow->tx->vin[0].scriptSig;
    const auto header_pos = std::search(script.begin(), script.end(), std::begin(AuxPow::MERGED_MINING_HEADER), std::end(AuxPow::MERGED_MINING_HEADER));
    const auto root_pos = std::search(script.begin(), script.end(), chain_root_bytes.begin(), chain_root_bytes.end());

    if (header_pos == script.end() || root_pos == script.end()) {
        return false;
    }
    if (std::search(header_pos + 1, script.end(), std::begin(AuxPow::MERGED_MINING_HEADER), std::end(AuxPow::MERGED_MINING_HEADER)) != script.end()) {
        return false;
    }
    if (header_pos + sizeof(AuxPow::MERGED_MINING_HEADER) != root_pos) {
        return false;
    }

    auto cursor = root_pos + chain_root_bytes.size();
    if (script.end() - cursor < 8) {
        return false;
    }

    const uint32_t tree_size = ReadLE32(&cursor[0]);
    if (tree_size != (1U << auxpow->vChainMerkleBranch.size())) {
        return false;
    }
    const uint32_t nonce = ReadLE32(&cursor[4]);

    uint32_t slot = nonce;
    slot = slot * 1103515245 + 12345;
    slot += chain_id;
    slot = slot * 1103515245 + 12345;

    return auxpow->nChainIndex == (slot % tree_size);
}

#endif // BITCOIN_AUXPOW_CHECK_H
