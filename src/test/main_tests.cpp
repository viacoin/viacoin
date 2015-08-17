// Copyright (c) 2014 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/transaction.h"
#include "main.h"

#include <boost/test/unit_test.hpp>
BOOST_AUTO_TEST_SUITE(main_tests)

BOOST_AUTO_TEST_CASE(subsidy_limit_test)
{
    CAmount nSum = 0;
    CAmount nSubsidy;
    for (int nHeight = 0; nHeight < 1971000; nHeight += 1000) {
        nSubsidy = GetBlockValue(nHeight, 0);
        BOOST_CHECK(nSubsidy <= 50 * COIN);
        nSum += nSubsidy * 1000;
        BOOST_CHECK(MoneyRange(nSum));
    }
    BOOST_CHECK(nSum == 988300000000000ULL);

    nSubsidy = GetBlockValue(1971000, 0);
    BOOST_CHECK(nSubsidy == 5 * COIN);

    nSubsidy = GetBlockValue(1971001, 0);
    BOOST_CHECK(nSubsidy == 2.5 * COIN);

    nSubsidy = GetBlockValue(2628000, 0);
    BOOST_CHECK(nSubsidy == 1.25 * COIN);
}

BOOST_AUTO_TEST_SUITE_END()
