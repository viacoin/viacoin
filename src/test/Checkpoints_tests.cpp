// Copyright (c) 2011-2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

//
// Unit tests for block-chain checkpoints
//

#include "checkpoints.h"

#include "uint256.h"

#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_SUITE(Checkpoints_tests)

BOOST_AUTO_TEST_CASE(sanity)
{
    uint256 p40821 = uint256("0xe0471675f9c98aa5ed321ed951d140d4603d96254a4ca9fbca07b8da5ee11954");
    uint256 p44606 = uint256("0x5ceeec38564a36ee3e1e5404970f5715efe0420e92c8e92bedfdfef782c49320");
    BOOST_CHECK(Checkpoints::CheckBlock(40821, p40821));
    BOOST_CHECK(Checkpoints::CheckBlock(44606, p44606));

    
    // Wrong hashes at checkpoints should fail:
    BOOST_CHECK(!Checkpoints::CheckBlock(40821, p44606));
    BOOST_CHECK(!Checkpoints::CheckBlock(44606, p40821));

    // ... but any hash not at a checkpoint should succeed:
    BOOST_CHECK(Checkpoints::CheckBlock(40821+1, p44606));
    BOOST_CHECK(Checkpoints::CheckBlock(44606+1, p40821));

    BOOST_CHECK(Checkpoints::GetTotalBlocksEstimate() >= 44606);
}    

BOOST_AUTO_TEST_SUITE_END()
