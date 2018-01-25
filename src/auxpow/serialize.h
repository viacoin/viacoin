// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_AUXPOW_SERIALIZE_H
#define BITCOIN_AUXPOW_SERIALIZE_H

class CAuxPow;

template<typename S>
void SerReadWrite(S& s, std::shared_ptr<CAuxPow>& pobj, CSerActionSerialize ser_action);
template<typename S>
void SerReadWrite(S& s, std::shared_ptr<CAuxPow>& pobj, CSerActionUnserialize ser_action);

/** Global dirty block merged mining entries. */
extern std::map<uint256, std::shared_ptr<CAuxPow> > mapDirtyAuxPow;

#endif // BITCOIN_AUXPOW_SERIALIZE_H
