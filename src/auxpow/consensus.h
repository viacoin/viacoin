// Copyright (c) 2009-2010 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef VIACOIN_AUXPOW_CONSENSUS_H
#define VIACOIN_AUXPOW_CONSENSUS_H

namespace AuxPow {

inline constexpr int START_MAINNET = 498725;
inline constexpr int START_TESTNET = 452608;
inline constexpr int START_REGTEST = 452608;

inline constexpr int CHAIN_ID = 0x56;

enum
{
    BLOCK_VERSION_DEFAULT = (1 << 0),
    BLOCK_VERSION_AUXPOW = (1 << 8),
    BLOCK_VERSION_CHAIN_START = (1 << 16),
    BLOCK_VERSION_CHAIN_END = (1 << 30),
};

} // namespace AuxPow

#endif // VIACOIN_AUXPOW_CONSENSUS_H
