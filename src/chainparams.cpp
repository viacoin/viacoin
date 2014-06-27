// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "assert.h"
#include "util.h"

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

//
// Main network
//

unsigned int pnSeed[] =
{
    0xc5f8c780, 0x51f9e2bc, 0x938caa6b,
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
        bnProofOfWorkLimit = ~uint256(0) >> 17;
        nSubsidyHalvingInterval = 2102400;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        nTargetSpacing = 1 * 60;

        // Build the genesis block. Note that the output of the genesis coinbase cannot
        // be spent as it did not originally exist in the database.
        //
        // CBlock(hash=e8b0c8fd9dac6d684a011ebccb8b7709dcc3314e15ac963e789975ece30b2371, PoW=0000676da9967023f40d73b6aa0744135e9992ad3bf965ff1ebb6b22f36526c0, ver=1, hashPrevBlock=0000000000000000000000000000000000000000000000000000000000000000, hashMerkleRoot=d9a52ba8673c87e4d8a73b7922dd58c05a3d14376378ff476946e0f3ce4091f3, nTime=1401031990, nBits=1e7fffff, nNonce=370206, vtx=1)
        //   CTransaction(hash=d9a52ba867, ver=1, vin.size=1, vout.size=1, nLockTime=0)
        //   CTxIn(COutPoint(0000000000, 4294967295), coinbase 04ffff001d01044c4e426c6f636b20233330323537333a30303030303030303030303030303030303766626137623430383231356633366265386235653262653638386563633265633135323165646531373931616465)
        //   CTxOut(nValue=0.00000000, scriptPubKey=0459934a6a228ce9716fa0b13aa1cd)
        // vMerkleTree: d9a52b
        const char* pszTimestamp = "Block #302573:000000000000000007fba7b408215f36be8b5e2be688ecc2ec1521ede1791ade";
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
        genesis.nTime    = 1401031990;
        genesis.nBits    = 0x1e7fffff;
        genesis.nNonce   = 370206;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0xe8b0c8fd9dac6d684a011ebccb8b7709dcc3314e15ac963e789975ece30b2371"));
        assert(genesis.hashMerkleRoot == uint256("0xd9a52ba8673c87e4d8a73b7922dd58c05a3d14376378ff476946e0f3ce4091f3"));

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
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        nTargetSpacing = 1 * 60;

        // Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1401043267;
        genesis.nNonce = 66080;
        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x5a2eb69d287d9e480d92bb54bcb2fdb35e64ebdf8c86aa12275be4a13c0ee105"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("bootstrap.viacoin.net", "testnet.viacoin.net"));
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
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        nTargetSpacing = 1 * 60;
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        genesis.nTime = 1401037650;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 1;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 15224;
        strDataDir = "regtest";
        assert(hashGenesisBlock == uint256("0x2382c32b31fd0fe74bab499e75bbabe2cc81d99fccddcabd2f2bd67228dd3f5a"));

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
