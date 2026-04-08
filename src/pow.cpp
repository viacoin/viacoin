// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <auxpow/check.h>
#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>
#include <util/check.h>

static unsigned int CalculateNextWorkRequired_V1(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting) {
        return pindexLast->nBits;
    }

    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan / 4) {
        nActualTimespan = params.nPowTargetTimespan / 4;
    }
    if (nActualTimespan > params.nPowTargetTimespan * 4) {
        nActualTimespan = params.nPowTargetTimespan * 4;
    }

    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    const bool fShift = bnNew.bits() > bnPowLimit.bits() - 1;
    if (fShift) {
        bnNew >>= 1;
    }
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;
    if (fShift) {
        bnNew <<= 1;
    }

    if (bnNew > bnPowLimit) {
        bnNew = bnPowLimit;
    }

    return bnNew.GetCompact();
}

static unsigned int GetNextWorkRequired_V1(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    const unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    if ((pindexLast->nHeight + 1) % params.DifficultyAdjustmentInterval() != 0) {
        if (params.fPowAllowMinDifficultyBlocks) {
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing * 2) {
                return nProofOfWorkLimit;
            }
            const CBlockIndex* pindex = pindexLast;
            while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit) {
                pindex = pindex->pprev;
            }
            return pindex->nBits;
        }
        return pindexLast->nBits;
    }

    int blockstogoback = params.DifficultyAdjustmentInterval() - 1;
    if ((pindexLast->nHeight + 1) != params.DifficultyAdjustmentInterval()) {
        blockstogoback = params.DifficultyAdjustmentInterval();
    }

    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < blockstogoback; ++i) {
        pindexFirst = pindexFirst->pprev;
    }
    assert(pindexFirst != nullptr);

    return CalculateNextWorkRequired_V1(pindexLast, pindexFirst->GetBlockTime(), params);
}

static unsigned int AntiGravityWave(int64_t version, const CBlockIndex* pindexLast, const CBlockHeader*, const Consensus::Params& params)
{
    const CBlockIndex* blockLastSolved = pindexLast;
    const CBlockIndex* blockReading = pindexLast;
    int64_t nActualTimespan = 0;
    int64_t lastBlockTime = 0;
    int64_t pastBlocksMin = 24;
    int64_t pastBlocksMax = 24;
    const unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    if (version == 2) {
        pastBlocksMin = 72;
        pastBlocksMax = 72;
    }

    int64_t countBlocks = 0;
    arith_uint256 pastDifficultyAverage;
    arith_uint256 pastDifficultyAveragePrev;

    if (blockLastSolved == nullptr || blockLastSolved->nHeight == 0 || blockLastSolved->nHeight < pastBlocksMin) {
        return nProofOfWorkLimit;
    }

    for (unsigned int i = 1; blockReading && blockReading->nHeight > 0; ++i) {
        if (pastBlocksMax > 0 && i > pastBlocksMax) {
            break;
        }
        countBlocks++;

        if (countBlocks <= pastBlocksMin) {
            if (countBlocks == 1) {
                pastDifficultyAverage.SetCompact(blockReading->nBits);
            } else {
                pastDifficultyAverage = ((pastDifficultyAveragePrev * countBlocks) + arith_uint256{}.SetCompact(blockReading->nBits)) / (countBlocks + 1);
            }
            pastDifficultyAveragePrev = pastDifficultyAverage;
        }

        if (lastBlockTime > 0) {
            nActualTimespan += (lastBlockTime - blockReading->GetBlockTime());
        }
        lastBlockTime = blockReading->GetBlockTime();

        if (blockReading->pprev == nullptr) {
            break;
        }
        blockReading = blockReading->pprev;
    }

    arith_uint256 bnNew{pastDifficultyAverage};
    if (version == 2) {
        --countBlocks;
    }

    int64_t nTargetTimespan = countBlocks * params.nPowTargetSpacing;
    int64_t div = version == 2 ? 2 : 3;

    if (nActualTimespan < nTargetTimespan / div) {
        nActualTimespan = nTargetTimespan / div;
    }
    if (nActualTimespan > nTargetTimespan * div) {
        nActualTimespan = nTargetTimespan * div;
    }

    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    if (bnNew > bnPowLimit) {
        bnNew = bnPowLimit;
    }

    return bnNew.GetCompact();
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    if (params.fPowNoRetargeting) {
        return pindexLast->nBits;
    }

    if (pindexLast->nHeight + 1 >= 451000 || (params.fPowAllowMinDifficultyBlocks && pindexLast->nHeight + 1 >= 300000)) {
        return AntiGravityWave(2, pindexLast, pblock, params);
    }
    if (pindexLast->nHeight + 1 >= 3600) {
        return AntiGravityWave(1, pindexLast, pblock, params);
    }
    return GetNextWorkRequired_V1(pindexLast, pblock, params);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    return CalculateNextWorkRequired_V1(pindexLast, nFirstBlockTime, params);
}

// Check that on difficulty adjustments, the new difficulty does not increase
// or decrease beyond the permitted limits.
bool PermittedDifficultyTransition(const Consensus::Params& params, int64_t height, uint32_t old_nbits, uint32_t new_nbits)
{
    if (params.fPowAllowMinDifficultyBlocks) return true;

    if (height % params.DifficultyAdjustmentInterval() == 0) {
        int64_t smallest_timespan = params.nPowTargetTimespan/4;
        int64_t largest_timespan = params.nPowTargetTimespan*4;

        const arith_uint256 pow_limit = UintToArith256(params.powLimit);
        arith_uint256 observed_new_target;
        observed_new_target.SetCompact(new_nbits);

        // Calculate the largest difficulty value possible:
        arith_uint256 largest_difficulty_target;
        largest_difficulty_target.SetCompact(old_nbits);
        largest_difficulty_target *= largest_timespan;
        largest_difficulty_target /= params.nPowTargetTimespan;

        if (largest_difficulty_target > pow_limit) {
            largest_difficulty_target = pow_limit;
        }

        // Round and then compare this new calculated value to what is
        // observed.
        arith_uint256 maximum_new_target;
        maximum_new_target.SetCompact(largest_difficulty_target.GetCompact());
        if (maximum_new_target < observed_new_target) return false;

        // Calculate the smallest difficulty value possible:
        arith_uint256 smallest_difficulty_target;
        smallest_difficulty_target.SetCompact(old_nbits);
        smallest_difficulty_target *= smallest_timespan;
        smallest_difficulty_target /= params.nPowTargetTimespan;

        if (smallest_difficulty_target > pow_limit) {
            smallest_difficulty_target = pow_limit;
        }

        // Round and then compare this new calculated value to what is
        // observed.
        arith_uint256 minimum_new_target;
        minimum_new_target.SetCompact(smallest_difficulty_target.GetCompact());
        if (minimum_new_target > observed_new_target) return false;
    } else if (old_nbits != new_nbits) {
        return false;
    }
    return true;
}

// Bypasses the actual proof of work check during fuzz testing with a simplified validation checking whether
// the most significant bit of the last byte of the hash is set.
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    if (EnableFuzzDeterminism()) return (hash.data()[31] & 0x80) == 0;
    return CheckProofOfWorkImpl(hash, nBits, params);
}

std::optional<arith_uint256> DeriveTarget(unsigned int nBits, const uint256 pow_limit)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(pow_limit))
        return {};

    return bnTarget;
}

bool CheckProofOfWorkImpl(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    auto bnTarget{DeriveTarget(nBits, params.powLimit)};
    if (!bnTarget) return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}

bool CheckBlockProofOfWork(const CBlockHeader& block, const Consensus::Params& params)
{
    if (block.auxpow) {
        if (!CheckAuxpow(block.auxpow, block.GetHash(), block.GetChainID(), params)) {
            return false;
        }
        return CheckProofOfWork(block.auxpow->GetParentBlockHash(), block.nBits, params);
    }

    return CheckProofOfWork(block.GetPoWHash(), block.nBits, params);
}

bool CheckAuxPowValidity(const CBlockHeader& block, const Consensus::Params& params)
{
    if (!params.fPowAllowMinDifficultyBlocks && block.GetChainID() != AuxPow::CHAIN_ID) {
        return false;
    }
    return true;
}
