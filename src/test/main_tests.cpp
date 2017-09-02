// Copyright (c) 2014-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "validation.h"
#include "net.h"

#include "test/test_bitcoin.h"

#include <boost/signals2/signal.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(main_tests, TestingSetup)

static void TestBlockSubsidyHalvings(const Consensus::Params& consensusParams)
{
    int maxHalvings = 64;
    CAmount nInitialSubsidy = 0 * COIN;

    CAmount nPreviousSubsidy = nInitialSubsidy * 2; // for height == 0
    BOOST_CHECK_EQUAL(nPreviousSubsidy, nInitialSubsidy * 2);
    for (int nHalvings = 0; nHalvings < maxHalvings; nHalvings++) {
        int nHeight = nHalvings * consensusParams.nSubsidyHalvingInterval;
        CAmount nSubsidy = GetBlockSubsidy(nHeight, consensusParams);
        BOOST_CHECK(nSubsidy <= nInitialSubsidy);
        BOOST_CHECK_EQUAL(nSubsidy, nPreviousSubsidy / 2);
        nPreviousSubsidy = nSubsidy;
    }
    BOOST_CHECK_EQUAL(GetBlockSubsidy(maxHalvings * consensusParams.nSubsidyHalvingInterval, consensusParams), 0);
}

static void TestBlockSubsidyHalvings(int nSubsidyHalvingInterval)
{
    Consensus::Params consensusParams;
    consensusParams.nSubsidyHalvingInterval = nSubsidyHalvingInterval;
    TestBlockSubsidyHalvings(consensusParams);
}

BOOST_AUTO_TEST_CASE(block_subsidy_test)
{
    // Viacoin: doesn't do generic halvings, so we test for the expected subsidy schedule
    const auto chainParams = CreateChainParams(CBaseChainParams::MAIN);
    //TestBlockSubsidyHalvings(chainParams->GetConsensus()); // As in main
    //TestBlockSubsidyHalvings(150); // As in regtest
    //TestBlockSubsidyHalvings(1000); // Just another interval

    //Consensus::Params consensusParams(Params(CBaseChainParams::MAIN).GetConsensus());

    struct {
        unsigned nHeight;
        double nSubsidy;
    } subsidyinfo[] = {
        {      0,      0 * COIN}, {      1, 10000000 * COIN}, {      2,       0 * COIN}, {  10001,         0 * COIN},
        {  10002,     10 * COIN}, {  20801,       10 * COIN}, {  20802,       7 * COIN}, {  31601,         7 * COIN},
        {  31602,      6 * COIN}, {  42401,        6 * COIN}, {  42402,       5 * COIN}, { 500000,         5 * COIN},
        {1971000,      5 * COIN}, {1971001,      2.5 * COIN}, {2627999,     2.5 * COIN}, {2628000,      1.25 * COIN},
        {3284999,   1.25 * COIN}, {3285000,    0.625 * COIN}, {3941999, 0.62500 * COIN}, {3942000,    0.3125 * COIN},
        {4598999, 0.3125 * COIN}, {4599000,  0.15625 * COIN}, {5255999, 0.15625 * COIN}, {5256000,  0.078125 * COIN}
    };

    for (auto&& s : subsidyinfo) {
        CAmount nSubsidy = GetBlockSubsidy(s.nHeight, chainParams->GetConsensus());
        BOOST_CHECK_EQUAL(nSubsidy, s.nSubsidy);
    }

}

BOOST_AUTO_TEST_CASE(subsidy_limit_test)
{
    const auto chainParams = CreateChainParams(CBaseChainParams::MAIN);
    CAmount nSum = 0;
    CAmount nSubsidy = 0;
    for (int nHeight = 0; nHeight < 1971000; nHeight += 1000) {
        CAmount nSubsidy = GetBlockSubsidy(nHeight, chainParams->GetConsensus());
        BOOST_CHECK(nSubsidy <= 10 * COIN);
        nSum += nSubsidy * 1000;
        BOOST_CHECK(MoneyRange(nSum));
    }
    BOOST_CHECK_EQUAL(nSum, 988300000000000ULL);
}

bool ReturnFalse() { return false; }
bool ReturnTrue() { return true; }

BOOST_AUTO_TEST_CASE(test_combiner_all)
{
    boost::signals2::signal<bool (), CombinerAll> Test;
    BOOST_CHECK(Test());
    Test.connect(&ReturnFalse);
    BOOST_CHECK(!Test());
    Test.connect(&ReturnTrue);
    BOOST_CHECK(!Test());
    Test.disconnect(&ReturnFalse);
    BOOST_CHECK(Test());
    Test.disconnect(&ReturnTrue);
    BOOST_CHECK(Test());
}
BOOST_AUTO_TEST_SUITE_END()
