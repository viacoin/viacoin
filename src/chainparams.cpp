// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "assert.h"
#include "random.h"
#include "util.h"

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

//
// Main network
//

unsigned int pnSeed[] =
{
    0x0,
};

class CMainParams : public CChainParams {
public:
    CMainParams() {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0x0f;
        pchMessageStart[1] = 0x68;
        pchMessageStart[2] = 0xc6;
        pchMessageStart[3] = 0xcb;
        vAlertPubKey = ParseHex("047885d9f6c0cf9e918d04634d4dd696cf172763f1975aad099daddca3f3c712c98754eae293b36484055e0d414800e519f5a342e56e09217faf07abff5bd96507");
        nDefaultPort = 5223;
        bnProofOfWorkLimit = ~uint256(0) >> 23;
        nSubsidyHalvingInterval = 2102400;
        nEnforceBlockUpgradeMajority = 15000;
        nRejectBlockOutdatedMajority = 19000;
        nToCheckBlockUpgradeMajority = 20000;
        nMinerThreads = 0;
        nTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        nTargetSpacing = 1 * 24;

        // Build the genesis block. Note that the output of the genesis coinbase cannot
        // be spent as it did not originally exist in the database.
        //
        // CBlock(hash=4e9b54001f9976049830128ec0331515eaabe35a70970d79971da1539a400ba1, PoW=000001a16729477595c7247e1b49b4ec93acca8345037177cabbe898ce8a5783, ver=1, hashPrevBlock=0000000000000000000000000000000000000000000000000000000000000000, 
        //     hashMerkleRoot=0317d32e01a2adf6f2ac6f58c7cdaab6c656edc6fdb45986c739290053275200,
        //     nTime=1405164774, nBits=1e01ffff, nNonce=4016033, vtx=1)
        //   CTransaction(hash=0317d32e01, ver=1, vin.size=1, vout.size=1, nLockTime=0)
        //   CTxIn(COutPoint(0000000000, 4294967295), coinbase 04ffff001d01044c4e426c6f636b20233331303337393a30303030303030303030303030303030323431323532613762623237626539376265666539323138633132393064666633366331666631323965633732313161)
        //   CTxOut(nValue=0.00000000, scriptPubKey=0459934a6a228ce9716fa0b13aa1cd)
        // vMerkleTree: 0317d32e01a2adf6f2ac6f58c7cdaab6c656edc6fdb45986c739290053275200
        const char* pszTimestamp = "Block #310379:0000000000000000241252a7bb27be97befe9218c1290dff36c1ff129ec7211a";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 0 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("0459934a6a228ce9716fa0b13aa1cdc01593fca5f8599473c803a5109ff834dfdaf4c9ee35f2218c9ee3e7cf7db734e1179524b9d6ae8ebbeba883d4cb89b6c7bf") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1405164774;
        genesis.nBits    = 0x1e01ffff;
        genesis.nNonce   = 4016033;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x4e9b54001f9976049830128ec0331515eaabe35a70970d79971da1539a400ba1"));
        assert(genesis.hashMerkleRoot == uint256("0x0317d32e01a2adf6f2ac6f58c7cdaab6c656edc6fdb45986c739290053275200"));

        vSeeds.push_back(CDNSSeedData("viacoin.net", "seed.viacoin.net"));
        vSeeds.push_back(CDNSSeedData("bootstap.viacoin.net", "mainnet.viacoin.net"));

        base58Prefixes[PUBKEY_ADDRESS] = list_of(71);
        base58Prefixes[SCRIPT_ADDRESS] = list_of(33);
        base58Prefixes[SECRET_KEY] =     list_of(199);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x88)(0xB2)(0x1E);
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x88)(0xAD)(0xE4);

        // Convert the pnSeeds array into usable address objects.
        for (unsigned int i = 0; i < ARRAYLEN(pnSeed); i++)
        {
            // It'll only connect to one or two seed nodes because once it connects,
            // it'll get a pile of addresses with newer timestamps.
            // Seed nodes are given a random 'last seen time' of between one and two
            // weeks ago.
            const int64_t nOneWeek = 7*24*60*60;
            struct in_addr ip;
            memcpy(&ip, &pnSeed[i], sizeof(ip));
            CAddress addr(CService(ip, GetDefaultPort()));
            addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
            vFixedSeeds.push_back(addr);
        }

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fDefaultCheckMemPool = false;
        fAllowMinDifficultyBlocks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
    }
};
static CMainParams mainParams;

//
// Testnet (v3)
//
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xa9;
        pchMessageStart[1] = 0xc5;
        pchMessageStart[2] = 0xef;
        pchMessageStart[3] = 0x92;
        vAlertPubKey = ParseHex("045c480dac3b2c8ef95a8577faa2420eabfe376fbfa31b2bb1896b3e5a30675403f4b3d084724d65afcbbb61473a302a6ed3286e39041176d9af6ff601543bd113");
        nDefaultPort = 25223;
        nEnforceBlockUpgradeMajority = 510;
        nRejectBlockOutdatedMajority = 750;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        nTargetSpacing = 1 * 24;
        bnProofOfWorkLimit = ~uint256(0) >> 19;
        genesis.nBits = 0x1e1fffff;

        // Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1405168772;
        genesis.nNonce = 262929;
        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x770aa712aa08fdcbdecc1c8df1b3e2d4e17a7cf6e63a28b785b32e74c96cb27d"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("bootstrap-testnet.viacoin.net", "testnet.viacoin.net"));
        vSeeds.push_back(CDNSSeedData("viacoin.net", "seed-testnet.viacoin.net"));

        base58Prefixes[PUBKEY_ADDRESS] = list_of(127);
        base58Prefixes[SCRIPT_ADDRESS] = list_of(196);
        base58Prefixes[SECRET_KEY]     = list_of(255);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x35)(0x87)(0xCF);
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x35)(0x83)(0x94);

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fDefaultCheckMemPool = false;
        fAllowMinDifficultyBlocks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
    }
};
static CTestNetParams testNetParams;

//
// Regression test
//
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        pchMessageStart[0] = 0x2d;
        pchMessageStart[1] = 0x97;
        pchMessageStart[2] = 0x7b;
        pchMessageStart[3] = 0x37;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 7500;
        nRejectBlockOutdatedMajority = 9500;
        nToCheckBlockUpgradeMajority = 10000;
        nMinerThreads = 1;
        nTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        nTargetSpacing = 1 * 24;
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        genesis.nTime = 1405166035;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 0;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 15224;
        assert(hashGenesisBlock == uint256("0xf0dae070f24fbc35311533a22aa85c0a616c84a1f22881612304d802acda286f"));

        vSeeds.clear();  // Regtest mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultCheckMemPool = true;
        fAllowMinDifficultyBlocks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

void SelectParams(CBaseChainParams::Network network) {
    SelectBaseParams(network);
    switch (network) {
        case CBaseChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CBaseChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        case CBaseChainParams::REGTEST:
            pCurrentParams = &regTestParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    }
}

bool SelectParamsFromCommandLine() {
    if (!SelectBaseParamsFromCommandLine())
        return false;

    SelectParams(BaseParams().NetworkID());
    return true;
}
