// Copyright (c) 2015-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <auxpow/auxpow.h>
#include <auxpow/check.h>
#include <chain.h>
#include <chainparams.h>
#include <consensus/consensus.h>
#include <crypto/common.h>
#include <node/blockstorage.h>
#include <pow.h>
#include <primitives/block.h>
#include <script/script.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>
#include <util/chaintype.h>
#include <validation.h>

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(pow_tests, BasicTestingSetup)

static Consensus::Params BitcoinLikePowParams(const Consensus::Params& base)
{
    Consensus::Params params{base};
    params.powLimit = uint256{"00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
    params.nPowTargetTimespan = 14 * 24 * 60 * 60;
    params.nPowTargetSpacing = 10 * 60;
    params.fPowAllowMinDifficultyBlocks = false;
    params.enforce_BIP94 = false;
    params.fPowNoRetargeting = false;
    return params;
}

/* Test calculation of next difficulty target with no constraints applying */
BOOST_AUTO_TEST_CASE(get_next_work)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = BitcoinLikePowParams(chainParams->GetConsensus());
    int64_t nLastRetargetTime = 1261130161; // Block #30240
    CBlockIndex pindexLast;
    pindexLast.nHeight = 32255;
    pindexLast.nTime = 1262152739;  // Block #32255
    pindexLast.nBits = 0x1d00ffff;

    // Here (and below): expected_nbits is calculated in
    // CalculateNextWorkRequired(); redoing the calculation here would be just
    // reimplementing the same code that is written in pow.cpp. Rather than
    // copy that code, we just hardcode the expected result.
    unsigned int expected_nbits = 0x1d00d86aU;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, consensus), expected_nbits);
    BOOST_CHECK(PermittedDifficultyTransition(consensus, pindexLast.nHeight + 1, pindexLast.nBits, expected_nbits));
}

/* Test the constraint on the upper bound for next work */
BOOST_AUTO_TEST_CASE(get_next_work_pow_limit)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = BitcoinLikePowParams(chainParams->GetConsensus());
    int64_t nLastRetargetTime = 1231006505; // Block #0
    CBlockIndex pindexLast;
    pindexLast.nHeight = 2015;
    pindexLast.nTime = 1233061996;  // Block #2015
    pindexLast.nBits = 0x1d00ffff;
    unsigned int expected_nbits = 0x1d00ffffU;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, consensus), expected_nbits);
    BOOST_CHECK(PermittedDifficultyTransition(consensus, pindexLast.nHeight + 1, pindexLast.nBits, expected_nbits));
}

/* Test the constraint on the lower bound for actual time taken */
BOOST_AUTO_TEST_CASE(get_next_work_lower_limit_actual)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = BitcoinLikePowParams(chainParams->GetConsensus());
    int64_t nLastRetargetTime = 1279008237; // Block #66528
    CBlockIndex pindexLast;
    pindexLast.nHeight = 68543;
    pindexLast.nTime = 1279297671;  // Block #68543
    pindexLast.nBits = 0x1c05a3f4;
    unsigned int expected_nbits = 0x1c0168fdU;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, consensus), expected_nbits);
    BOOST_CHECK(PermittedDifficultyTransition(consensus, pindexLast.nHeight + 1, pindexLast.nBits, expected_nbits));
    unsigned int invalid_nbits = expected_nbits - 1;
    BOOST_CHECK(!PermittedDifficultyTransition(consensus, pindexLast.nHeight + 1, pindexLast.nBits, invalid_nbits));
}

/* Test the constraint on the upper bound for actual time taken */
BOOST_AUTO_TEST_CASE(get_next_work_upper_limit_actual)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = BitcoinLikePowParams(chainParams->GetConsensus());
    int64_t nLastRetargetTime = 1263163443; // NOTE: Not an actual block time
    CBlockIndex pindexLast;
    pindexLast.nHeight = 46367;
    pindexLast.nTime = 1269211443;  // Block #46367
    pindexLast.nBits = 0x1c387f6f;
    unsigned int expected_nbits = 0x1d00e1fdU;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, consensus), expected_nbits);
    BOOST_CHECK(PermittedDifficultyTransition(consensus, pindexLast.nHeight + 1, pindexLast.nBits, expected_nbits));
    unsigned int invalid_nbits = expected_nbits + 1;
    BOOST_CHECK(!PermittedDifficultyTransition(consensus, pindexLast.nHeight + 1, pindexLast.nBits, invalid_nbits));
}

static Consensus::Params ViacoinLegacyPowParams(const Consensus::Params& base)
{
    Consensus::Params params{base};
    params.powLimit = uint256{"000001ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
    params.nPowTargetTimespan = 14 * 24 * 60 * 60;
    params.nPowTargetSpacing = 24;
    params.fPowAllowMinDifficultyBlocks = false;
    params.enforce_BIP94 = false;
    params.fPowNoRetargeting = false;
    return params;
}

BOOST_AUTO_TEST_CASE(viacoin_legacy_v1_vector_280223)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = ViacoinLegacyPowParams(chainParams->GetConsensus());

    int64_t nLastRetargetTime = 1358118740; // Block #278207
    CBlockIndex pindexLast;
    pindexLast.nHeight = 280223;
    pindexLast.nTime = 1358378777;  // Block #280223
    pindexLast.nBits = 0x1c0ac141;

    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, consensus), 0x1c02b050U);
}

BOOST_AUTO_TEST_CASE(viacoin_legacy_v1_vector_2015)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = ViacoinLegacyPowParams(chainParams->GetConsensus());

    int64_t nLastRetargetTime = 1317972665; // Block #0
    CBlockIndex pindexLast;
    pindexLast.nHeight = 2015;
    pindexLast.nTime = 1318480354;  // Block #2015
    pindexLast.nBits = 0x1e0ffff0;

    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, consensus), 0x1e01ffffU);
}

BOOST_AUTO_TEST_CASE(viacoin_legacy_v1_vector_578591)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = ViacoinLegacyPowParams(chainParams->GetConsensus());

    int64_t nLastRetargetTime = 1401682934; // legacy note: not an actual block time
    CBlockIndex pindexLast;
    pindexLast.nHeight = 578591;
    pindexLast.nTime = 1401757934;
    pindexLast.nBits = 0x1b075cf1;

    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, consensus), 0x1b01d73cU);
}

BOOST_AUTO_TEST_CASE(viacoin_legacy_v1_vector_1001951)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = ViacoinLegacyPowParams(chainParams->GetConsensus());

    int64_t nLastRetargetTime = 1463690315; // legacy note: not an actual block time
    CBlockIndex pindexLast;
    pindexLast.nHeight = 1001951;
    pindexLast.nTime = 1464900315;
    pindexLast.nBits = 0x1b015318;

    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, consensus), 0x1b015334U);
}

static unsigned int LegacyGetNextWorkRequiredV1(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params)
{
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
    BOOST_REQUIRE(pindexFirst != nullptr);
    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

static unsigned int LegacyAntiGravityWave(int64_t version, const CBlockIndex* pindexLast, const Consensus::Params& params)
{
    const CBlockIndex* block_last_solved = pindexLast;
    const CBlockIndex* block_reading = pindexLast;
    int64_t nActualTimespan = 0;
    int64_t last_block_time = 0;
    int64_t past_blocks_min = 24;
    int64_t past_blocks_max = 24;
    const unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    if (version == 2) {
        past_blocks_min = 72;
        past_blocks_max = 72;
    }

    int64_t count_blocks = 0;
    arith_uint256 past_difficulty_average;
    arith_uint256 past_difficulty_average_prev;

    if (block_last_solved == nullptr || block_last_solved->nHeight == 0 || block_last_solved->nHeight < past_blocks_min) {
        return nProofOfWorkLimit;
    }

    for (unsigned int i = 1; block_reading && block_reading->nHeight > 0; ++i) {
        if (past_blocks_max > 0 && i > past_blocks_max) break;
        count_blocks++;

        if (count_blocks <= past_blocks_min) {
            if (count_blocks == 1) {
                past_difficulty_average.SetCompact(block_reading->nBits);
            } else {
                past_difficulty_average = ((past_difficulty_average_prev * count_blocks) + (arith_uint256().SetCompact(block_reading->nBits))) / (count_blocks + 1);
            }
            past_difficulty_average_prev = past_difficulty_average;
        }

        if (last_block_time > 0) {
            nActualTimespan += (last_block_time - block_reading->GetBlockTime());
        }
        last_block_time = block_reading->GetBlockTime();

        if (block_reading->pprev == nullptr) break;
        block_reading = block_reading->pprev;
    }

    arith_uint256 bnNew(past_difficulty_average);
    if (version == 2) --count_blocks;

    int64_t nTargetTimespan = count_blocks * params.nPowTargetSpacing;
    int64_t div = version == 2 ? 2 : 3;

    if (nActualTimespan < nTargetTimespan / div) nActualTimespan = nTargetTimespan / div;
    if (nActualTimespan > nTargetTimespan * div) nActualTimespan = nTargetTimespan * div;

    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    if (bnNew > bnPowLimit) bnNew = bnPowLimit;
    return bnNew.GetCompact();
}

static unsigned int LegacyViacoinGetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting) return pindexLast->nBits;

    if (pindexLast->nHeight + 1 >= 451000 || (params.fPowAllowMinDifficultyBlocks && pindexLast->nHeight + 1 >= 300000)) {
        return LegacyAntiGravityWave(2, pindexLast, params);
    } else if (pindexLast->nHeight + 1 >= 3600) {
        return LegacyAntiGravityWave(1, pindexLast, params);
    }
    return LegacyGetNextWorkRequiredV1(pindexLast, pblock, params);
}

static std::vector<CBlockIndex> BuildSyntheticHistory(int last_height, uint32_t nBits, int64_t start_time, int64_t first_step, int64_t later_step)
{
    std::vector<CBlockIndex> chain(last_height + 1);
    for (int i = 0; i <= last_height; ++i) {
        if (i > 0) chain[i].pprev = &chain[i - 1];
        chain[i].nHeight = i;
        chain[i].nBits = nBits;
        if (i == 0) {
            chain[i].nTime = start_time;
        } else if (i == 1) {
            chain[i].nTime = start_time + first_step;
        } else {
            chain[i].nTime = chain[i - 1].nTime + later_step;
        }
    }
    return chain;
}

static void CheckSyntheticViacoinAgwCase(const Consensus::Params& consensus, int last_height, uint32_t nBits, int64_t start_time, int64_t first_step, int64_t later_step, uint32_t expected_nbits)
{
    auto chain = BuildSyntheticHistory(last_height, nBits, start_time, first_step, later_step);

    CBlockHeader block;
    block.nTime = chain.back().nTime + consensus.nPowTargetSpacing;

    const auto current_nbits = GetNextWorkRequired(&chain.back(), &block, consensus);
    const auto legacy_nbits = LegacyViacoinGetNextWorkRequired(&chain.back(), &block, consensus);

    BOOST_CHECK_EQUAL(legacy_nbits, expected_nbits);
    BOOST_CHECK_EQUAL(current_nbits, expected_nbits);
}

BOOST_AUTO_TEST_CASE(viacoin_agw_v1_dispatch_boundary_red)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = ViacoinLegacyPowParams(chainParams->GetConsensus());
    CheckSyntheticViacoinAgwCase(consensus, 3599, 0x1c0ac141U, 1'350'000'000, 24 * 20, 24 * 20, 0x1c2043c3U);
}

BOOST_AUTO_TEST_CASE(viacoin_agw_v2_dispatch_boundary_red)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = ViacoinLegacyPowParams(chainParams->GetConsensus());
    CheckSyntheticViacoinAgwCase(consensus, 450999, 0x1b015318U, 1'460'000'000, 24 * 18, 24 * 18, 0x1b02a630U);
}

BOOST_AUTO_TEST_CASE(viacoin_agw_v1_clamp_floor_exact)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = ViacoinLegacyPowParams(chainParams->GetConsensus());
    CheckSyntheticViacoinAgwCase(consensus, 3600, 0x1c0ac141U, 1'350'500'000, 1, 1, 0x1c0395c0U);
}

BOOST_AUTO_TEST_CASE(viacoin_agw_v1_clamp_ceiling_exact)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = ViacoinLegacyPowParams(chainParams->GetConsensus());
    CheckSyntheticViacoinAgwCase(consensus, 3600, 0x1c0ac141U, 1'351'000'000, 100, 100, 0x1c2043c3U);
}

BOOST_AUTO_TEST_CASE(viacoin_agw_v2_clamp_floor_exact)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = ViacoinLegacyPowParams(chainParams->GetConsensus());
    CheckSyntheticViacoinAgwCase(consensus, 451000, 0x1b015318U, 1'460'500'000, 1, 1, 0x1b00a98cU);
}

BOOST_AUTO_TEST_CASE(viacoin_agw_v2_clamp_ceiling_exact)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto consensus = ViacoinLegacyPowParams(chainParams->GetConsensus());
    CheckSyntheticViacoinAgwCase(consensus, 451000, 0x1b015318U, 1'461'000'000, 100, 100, 0x1b02a630U);
}

BOOST_AUTO_TEST_CASE(viacoin_mainnet_subsidy_schedule_red)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);

    struct {
        int height;
        CAmount subsidy;
    } subsidyinfo[] = {
        {0, 0 * COIN},
        {1, 10000000 * COIN},
        {2, 0 * COIN},
        {10001, 0 * COIN},
        {10002, 10 * COIN},
        {20801, 10 * COIN},
        {20802, 7 * COIN},
        {31601, 7 * COIN},
        {31602, 6 * COIN},
        {42401, 6 * COIN},
        {42402, 5 * COIN},
        {500000, 5 * COIN},
        {1971000, 5 * COIN},
        {1971001, 250000000},
        {2627999, 250000000},
        {2628000, 125000000},
        {3284999, 125000000},
        {3285000, 62500000},
        {3941999, 62500000},
        {3942000, 31250000},
        {4598999, 31250000},
        {4599000, 15625000},
        {5255999, 15625000},
        {5256000, 7812500},
    };

    for (const auto& s : subsidyinfo) {
        BOOST_CHECK_EQUAL(GetBlockSubsidy(s.height, chainParams->GetConsensus()), s.subsidy);
    }
}

BOOST_AUTO_TEST_CASE(viacoin_mainnet_subsidy_limit_red)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    CAmount nSum = 0;
    CAmount max_subsidy = 0;
    for (int nHeight = 0; nHeight < 1971000; nHeight += 1000) {
        const CAmount nSubsidy = GetBlockSubsidy(nHeight, chainParams->GetConsensus());
        max_subsidy = std::max(max_subsidy, nSubsidy);
        nSum += nSubsidy * 1000;
        BOOST_REQUIRE(MoneyRange(nSum));
    }
    BOOST_CHECK_EQUAL(max_subsidy, 10 * COIN);
    BOOST_CHECK_EQUAL(nSum, CAmount{988300000000000ULL});
}

BOOST_AUTO_TEST_CASE(viacoin_mainnet_halving_interval_red)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    BOOST_CHECK_EQUAL(chainParams->GetConsensus().nSubsidyHalvingInterval, 657000);
}

BOOST_AUTO_TEST_CASE(viacoin_mainnet_chainparams_identity_red)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto& consensus = chainParams->GetConsensus();
    BOOST_CHECK_EQUAL(consensus.powLimit.ToString(), "000001ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    BOOST_CHECK_EQUAL(consensus.nPowTargetTimespan, 14 * 24 * 60 * 60);
    BOOST_CHECK_EQUAL(consensus.nPowTargetSpacing, 24);
    BOOST_CHECK_EQUAL(chainParams->GenesisBlock().GetHash().ToString(), "4e9b54001f9976049830128ec0331515eaabe35a70970d79971da1539a400ba1");
    BOOST_CHECK_EQUAL(chainParams->GenesisBlock().hashMerkleRoot.ToString(), "0317d32e01a2adf6f2ac6f58c7cdaab6c656edc6fdb45986c739290053275200");
    BOOST_REQUIRE_EQUAL(chainParams->GenesisBlock().vtx.size(), 1U);
    BOOST_CHECK_EQUAL(chainParams->GenesisBlock().vtx[0]->vout[0].nValue, 0 * COIN);
}

BOOST_AUTO_TEST_CASE(viacoin_testnet_chainparams_identity_red)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::TESTNET);
    const auto& consensus = chainParams->GetConsensus();
    BOOST_CHECK_EQUAL(consensus.powLimit.ToString(), "00001fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    BOOST_CHECK_EQUAL(consensus.nPowTargetTimespan, static_cast<int64_t>(3.5 * 24 * 60 * 60));
    BOOST_CHECK_EQUAL(consensus.nPowTargetSpacing, 24);
    BOOST_CHECK_EQUAL(chainParams->GenesisBlock().GetHash().ToString(), "770aa712aa08fdcbdecc1c8df1b3e2d4e17a7cf6e63a28b785b32e74c96cb27d");
    BOOST_REQUIRE_EQUAL(chainParams->GenesisBlock().vtx.size(), 1U);
    BOOST_CHECK_EQUAL(chainParams->GenesisBlock().vtx[0]->vout[0].nValue, 0 * COIN);
}

BOOST_AUTO_TEST_CASE(viacoin_regtest_chainparams_identity_red)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::REGTEST);
    const auto& consensus = chainParams->GetConsensus();
    BOOST_CHECK_EQUAL(consensus.powLimit.ToString(), "efffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    BOOST_CHECK_EQUAL(consensus.nPowTargetTimespan, 14 * 24 * 60 * 60);
    BOOST_CHECK_EQUAL(consensus.nPowTargetSpacing, 24);
    BOOST_CHECK_EQUAL(chainParams->GenesisBlock().GetHash().ToString(), "f0dae070f24fbc35311533a22aa85c0a616c84a1f22881612304d802acda286f");
    BOOST_REQUIRE_EQUAL(chainParams->GenesisBlock().vtx.size(), 1U);
    BOOST_CHECK_EQUAL(chainParams->GenesisBlock().vtx[0]->vout[0].nValue, 0 * COIN);
}

BOOST_AUTO_TEST_CASE(viacoin_consensus_constants_red)
{
    BOOST_CHECK_EQUAL(MAX_BLOCK_SERIALIZED_SIZE, 240000U);
    BOOST_CHECK_EQUAL(MAX_BLOCK_WEIGHT, 240000U);
    BOOST_CHECK_EQUAL(MAX_BLOCK_SIGOPS_COST, 8000);
    BOOST_CHECK_EQUAL(COINBASE_MATURITY, 3600);
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_negative_target)
{
    const auto consensus = CreateChainParams(*m_node.args, ChainType::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits;
    nBits = UintToArith256(consensus.powLimit).GetCompact(true);
    hash = uint256{1};
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_overflow_target)
{
    const auto consensus = CreateChainParams(*m_node.args, ChainType::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits{~0x00800000U};
    hash = uint256{1};
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_too_easy_target)
{
    const auto consensus = CreateChainParams(*m_node.args, ChainType::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits;
    arith_uint256 nBits_arith = UintToArith256(consensus.powLimit);
    nBits_arith *= 2;
    nBits = nBits_arith.GetCompact();
    hash = uint256{1};
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_biger_hash_than_target)
{
    const auto consensus = CreateChainParams(*m_node.args, ChainType::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits;
    arith_uint256 hash_arith = UintToArith256(consensus.powLimit);
    nBits = hash_arith.GetCompact();
    hash_arith *= 2; // hash > nBits
    hash = ArithToUint256(hash_arith);
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_zero_target)
{
    const auto consensus = CreateChainParams(*m_node.args, ChainType::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits;
    arith_uint256 hash_arith{0};
    nBits = hash_arith.GetCompact();
    hash = ArithToUint256(hash_arith);
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

static std::vector<unsigned char> SerializeUint32(uint32_t value)
{
    std::array<unsigned char, 4> bytes;
    WriteLE32(bytes.data(), value);
    return {bytes.begin(), bytes.end()};
}

static CBlockHeader MinePowOnlyHeader(const CChainParams& chain_params)
{
    const auto& consensus = chain_params.GetConsensus();

    CBlockHeader header;
    header.nVersion = chain_params.GenesisBlock().nVersion;
    header.hashPrevBlock = chain_params.GenesisBlock().GetHash();
    header.hashMerkleRoot = uint256{1};
    header.nTime = chain_params.GenesisBlock().nTime + 1;
    header.nNonce = 0;

    for (int divisor : {2, 4, 8, 16, 32, 64, 128, 256, 512}) {
        arith_uint256 candidate_target = UintToArith256(consensus.powLimit);
        candidate_target /= divisor;
        header.nBits = candidate_target.GetCompact();
        header.nNonce = 0;

        for (uint32_t tries = 0; tries < 200000; ++tries) {
            if (CheckProofOfWork(header.GetPoWHash(), header.nBits, consensus) &&
                !CheckProofOfWork(header.GetHash(), header.nBits, consensus)) {
                return header;
            }
            ++header.nNonce;
            BOOST_REQUIRE(header.nNonce != 0);
        }
    }

    BOOST_FAIL("failed to mine pow-only header");
    return header;
}

static CBlockHeader MakeAuxpowHeaderTemplate(const CChainParams& chain_params)
{
    CBlockHeader header;
    header.nVersion = chain_params.GenesisBlock().nVersion | AuxPow::BLOCK_VERSION_AUXPOW |
        (AuxPow::CHAIN_ID * AuxPow::BLOCK_VERSION_CHAIN_START);
    header.hashPrevBlock = chain_params.GenesisBlock().GetHash();
    header.hashMerkleRoot = uint256{9};
    header.nTime = chain_params.GenesisBlock().nTime + 2;
    header.nNonce = 1;
    header.nBits = UintToArith256(chain_params.GetConsensus().powLimit).GetCompact();
    return header;
}

static CBlockHeader MakeValidAuxpowHeader(const CChainParams& chain_params)
{
    const auto& consensus = chain_params.GetConsensus();
    CBlockHeader header = MakeAuxpowHeaderTemplate(chain_params);

    auto auxpow = std::make_shared<CAuxPow>();
    auxpow->nIndex = 0;
    auxpow->vMerkleBranch.clear();
    auxpow->vChainMerkleBranch.clear();
    auxpow->nChainIndex = 0;
    auxpow->parentBlockHeader.nVersion = 1;
    auxpow->parentBlockHeader.hashPrevBlock = uint256{21};
    auxpow->parentBlockHeader.hashMerkleRoot.SetNull();
    auxpow->parentBlockHeader.nTime = header.nTime + 1;
    auxpow->parentBlockHeader.nBits = header.nBits;
    auxpow->parentBlockHeader.nNonce = 0;

    const uint256 aux_block_hash = header.GetHash();
    std::vector<unsigned char> root_bytes(aux_block_hash.begin(), aux_block_hash.end());
    std::reverse(root_bytes.begin(), root_bytes.end());
    std::vector<unsigned char> payload{0xfa, 0xbe, 'm', 'm'};
    payload.insert(payload.end(), root_bytes.begin(), root_bytes.end());
    const auto tree_size = SerializeUint32(1);
    payload.insert(payload.end(), tree_size.begin(), tree_size.end());
    const auto nonce_bytes = SerializeUint32(0);
    payload.insert(payload.end(), nonce_bytes.begin(), nonce_bytes.end());

    CMutableTransaction tx;
    tx.version = 2;
    tx.vin.resize(1);
    tx.vout.resize(1);
    tx.vin[0].scriptSig = CScript() << payload;
    auxpow->tx = MakeTransactionRef(tx);
    auxpow->parentBlockHeader.hashMerkleRoot = auxpow->tx->GetHash().ToUint256();

    while (!CheckProofOfWork(auxpow->parentBlockHeader.GetPoWHash(), auxpow->parentBlockHeader.nBits, consensus)) {
        ++auxpow->parentBlockHeader.nNonce;
        BOOST_REQUIRE(auxpow->parentBlockHeader.nNonce != 0);
    }

    header.SetAuxPow(new CAuxPow(*auxpow));
    return header;
}

BOOST_AUTO_TEST_CASE(HasValidProofOfWork_uses_powhash_not_header_hash)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::REGTEST);
    const auto& consensus = chainParams->GetConsensus();
    const CBlockHeader header = MinePowOnlyHeader(*chainParams);

    BOOST_REQUIRE(CheckProofOfWork(header.GetPoWHash(), header.nBits, consensus));
    BOOST_REQUIRE(!CheckProofOfWork(header.GetHash(), header.nBits, consensus));
    BOOST_REQUIRE(HasValidProofOfWork({header}, consensus));
}

BOOST_AUTO_TEST_CASE(LoadBlockIndexGuts_uses_powhash_not_header_hash)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::REGTEST);
    const auto& consensus = chainParams->GetConsensus();
    const CBlockHeader header = MinePowOnlyHeader(*chainParams);

    kernel::BlockTreeDB block_index_db(DBParams{
        .path = "",
        .cache_bytes = 1 << 20,
        .memory_only = true,
    });

    CBlockIndex prev_index{chainParams->GenesisBlock()};
    const uint256 genesis_hash = chainParams->GenesisBlock().GetHash();
    prev_index.phashBlock = &genesis_hash;

    CBlockIndex source_index;
    source_index.pprev = &prev_index;
    source_index.nHeight = 1;
    source_index.nVersion = header.nVersion;
    source_index.hashMerkleRoot = header.hashMerkleRoot;
    source_index.nTime = header.nTime;
    source_index.nBits = header.nBits;
    source_index.nNonce = header.nNonce;
    WITH_LOCK(::cs_main, source_index.nStatus = BLOCK_VALID_TREE);
    source_index.nTx = 1;

    const CDiskBlockIndex disk_index{&source_index};
    BOOST_REQUIRE(CheckProofOfWork(header.GetPoWHash(), header.nBits, consensus));
    BOOST_REQUIRE(!CheckProofOfWork(disk_index.ConstructBlockHash(), header.nBits, consensus));
    BOOST_REQUIRE(block_index_db.Write(std::make_pair(uint8_t{'b'}, disk_index.ConstructBlockHash()), disk_index));

    node::BlockMap loaded_indexes;
    const auto inserter = [&](const uint256& hash) {
        const auto [it, inserted] = loaded_indexes.try_emplace(hash);
        CBlockIndex* index = &it->second;
        if (inserted) {
            index->phashBlock = &it->first;
        }
        return index;
    };

    WITH_LOCK(::cs_main, BOOST_CHECK(block_index_db.LoadBlockIndexGuts(consensus, inserter, m_interrupt)));
    const auto loaded_it = loaded_indexes.find(disk_index.ConstructBlockHash());
    BOOST_REQUIRE(loaded_it != loaded_indexes.end());
    BOOST_CHECK_EQUAL(loaded_it->second.nBits, header.nBits);
    BOOST_CHECK_EQUAL(loaded_it->second.nNonce, header.nNonce);
}

BOOST_AUTO_TEST_CASE(CheckBlockProofOfWork_accepts_valid_auxpow_parent_pow)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::REGTEST);
    const auto& consensus = chainParams->GetConsensus();
    const CBlockHeader header = MakeValidAuxpowHeader(*chainParams);

    BOOST_REQUIRE(header.IsAuxPow());
    BOOST_REQUIRE(header.auxpow);
    BOOST_REQUIRE(CheckProofOfWork(header.auxpow->GetParentBlockHash(), header.nBits, consensus));
    BOOST_REQUIRE(CheckAuxpow(header.auxpow, header.GetHash(), header.GetChainID(), consensus));
    BOOST_CHECK(CheckBlockProofOfWork(header, consensus));
}

BOOST_AUTO_TEST_CASE(CheckBlockProofOfWork_rejects_invalid_auxpow_payload_even_with_valid_parent_pow)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::REGTEST);
    const auto& consensus = chainParams->GetConsensus();
    CBlockHeader header = MakeValidAuxpowHeader(*chainParams);

    BOOST_REQUIRE(header.auxpow);
    BOOST_REQUIRE(CheckProofOfWork(header.auxpow->GetParentBlockHash(), header.nBits, consensus));
    CAuxPow invalid_auxpow{*header.auxpow};
    invalid_auxpow.nIndex = 1;
    header.SetAuxPow(new CAuxPow(invalid_auxpow));

    BOOST_CHECK(!CheckBlockProofOfWork(header, consensus));
}

BOOST_AUTO_TEST_CASE(GetBlockProofEquivalentTime_test)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    std::vector<CBlockIndex> blocks(10000);
    for (int i = 0; i < 10000; i++) {
        blocks[i].pprev = i ? &blocks[i - 1] : nullptr;
        blocks[i].nHeight = i;
        blocks[i].nTime = 1269211443 + i * chainParams->GetConsensus().nPowTargetSpacing;
        blocks[i].nBits = 0x207fffff; /* target 0x7fffff000... */
        blocks[i].nChainWork = i ? blocks[i - 1].nChainWork + GetBlockProof(blocks[i - 1]) : arith_uint256(0);
    }

    for (int j = 0; j < 1000; j++) {
        CBlockIndex *p1 = &blocks[m_rng.randrange(10000)];
        CBlockIndex *p2 = &blocks[m_rng.randrange(10000)];
        CBlockIndex *p3 = &blocks[m_rng.randrange(10000)];

        int64_t tdiff = GetBlockProofEquivalentTime(*p1, *p2, *p3, chainParams->GetConsensus());
        BOOST_CHECK_EQUAL(tdiff, p1->GetBlockTime() - p2->GetBlockTime());
    }
}

BOOST_AUTO_TEST_SUITE_END()
