
// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=4e9b54001f9976049830128ec0331515eaabe35a70970d79971da1539a400ba1, PoW=000001a16729477595c7247e1b49b4ec93acca8345037177cabbe898ce8a5783, ver=1, hashPrevBlock=0000000000000000000000000000000000000000000000000000000000000000,
 *     hashMerkleRoot=0317d32e01a2adf6f2ac6f58c7cdaab6c656edc6fdb45986c739290053275200,
 *     nTime=1405164774, nBits=1e01ffff, nNonce=4016033, vtx=1)
 *   CTransaction(hash=0317d32e01, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *   CTxIn(COutPoint(0000000000, 4294967295), coinbase 04ffff001d01044c4e426c6f636b20233331303337393a30303030303030303030303030303030323431323532613762623237626539376265666539323138633132393064666633366331666631323965633732313161)
 *   CTxOut(nValue=0.00000000, scriptPubKey=0459934a6a228ce9716fa0b13aa1cd)
 * vMerkleTree: 0317d32e01a2adf6f2ac6f58c7cdaab6c656edc6fdb45986c739290053275200
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Block #310379:0000000000000000241252a7bb27be97befe9218c1290dff36c1ff129ec7211a";
    const CScript genesisOutputScript = CScript() << ParseHex("0459934a6a228ce9716fa0b13aa1cdc01593fca5f8599473c803a5109ff834dfdaf4c9ee35f2218c9ee3e7cf7db734e1179524b9d6ae8ebbeba883d4cb89b6c7bf") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 657000;
        consensus.nMajorityEnforceBlockUpgrade = 15000;
        consensus.nMajorityRejectBlockOutdated = 19000;
        consensus.nMajorityWindow = 20000;
        consensus.BIP34Height = -1;
        consensus.BIP34Hash = uint256S("0x0");
        consensus.powLimit = uint256S("000001ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint25(0) >> 23
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 1 * 24;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 47880; // 95% of nMinerConfirmationWindow
        consensus.nMinerConfirmationWindow = 50400; // nPowTargetTimespan / nPowTargetSpacing
        consensus.nCLTVStartBlock = 598725;
        consensus.nBIP66MinStartBlock = 1200000;
        consensus.nAuxPowStartHeight = AuxPow::START_MAINNET;

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1489968000; // March 21, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1522454400;   // March 31, 2018

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1489968000; // March 21, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1522454400;   // March 31, 2018

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000024678b06568466a22");

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x0f;
        pchMessageStart[1] = 0x68;
        pchMessageStart[2] = 0xc6;
        pchMessageStart[3] = 0xcb;
        nDefaultPort = 5223;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1405164774, 4016033, 0x1e01ffff, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x4e9b54001f9976049830128ec0331515eaabe35a70970d79971da1539a400ba1"));
        assert(genesis.hashMerkleRoot == uint256S("0x0317d32e01a2adf6f2ac6f58c7cdaab6c656edc6fdb45986c739290053275200"));

        // Note that of those with the service bits flag, most only support a subset of possible options
        // TODO - LED - Check which viacoin nodes support service bits and add the 'true' flag
        vSeeds.push_back(CDNSSeedData("viacoin.net", "seed.viacoin.net"));
        vSeeds.push_back(CDNSSeedData("zzy.su", "seed.zzy.su"));
        vSeeds.push_back(CDNSSeedData("bootstap.viacoin.net", "mainnet.viacoin.net"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,71);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,33);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,199);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;

	checkpointData = (CCheckpointData) {
		boost::assign::map_list_of
            ( 3901, uint256S("0x39c94020b653871bbcb29c9489bb12f84c5e89adef75f2e5c5f59e88869129d9"))
            ( 40821, uint256S("0xe0471675f9c98aa5ed321ed951d140d4603d96254a4ca9fbca07b8da5ee11954"))
            ( 41433, uint256S("0x627e18cc08a276282781705bac09508992dc8b665391edd7bde8a601f011954c"))
            ( 44606, uint256S("0x5ceeec38564a36ee3e1e5404970f5715efe0420e92c8e92bedfdfef782c49320")),
            1406662650, // * UNIX timestamp of last checkpoint block
            54299,   // * total number of transactions between genesis and last checkpoint
			//   (the tx=... number in the SetBestChain debug.log lines)
			10000.0     // * estimated number of transactions per day after checkpoint
	};

    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 657000;
        consensus.nMajorityEnforceBlockUpgrade = 510;
        consensus.nMajorityRejectBlockOutdated = 750;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = -1;
        consensus.BIP34Hash = uint256S("0x0");
        consensus.powLimit = uint256S("00001fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint25(0) >> 19
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // 3.5 days
        consensus.nPowTargetSpacing = 1 * 24;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 9450; // 75% of nMinerConfirmationWindow
        consensus.nMinerConfirmationWindow = 12600; // nPowTargetTimespan / nPowTargetSpacing
        consensus.nCLTVStartBlock = 502664;
        consensus.nBIP66MinStartBlock = 800000;
        consensus.nAuxPowStartHeight = AuxPow::START_TESTNET;

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1483228800; // January 1, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1517356801; // January 31st, 2018

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1483228800; // January 1, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1517356801; // January 31st, 2018

        // The best chain should have at least this much work.
        // TODO LED no idea for test yet
        // consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000000000006fce5d67766e");
        consensus.nMinimumChainWork = uint256S("0x0");

        pchMessageStart[0] = 0xa9;
        pchMessageStart[1] = 0xc5;
        pchMessageStart[2] = 0xef;
        pchMessageStart[3] = 0x92;
        nDefaultPort = 25223;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1405168772, 262929, 0x1e1fffff, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x770aa712aa08fdcbdecc1c8df1b3e2d4e17a7cf6e63a28b785b32e74c96cb27d"));
        // assert(genesis.hashMerkleRoot == uint256S("0x97ddfbbae6be97fd6cdf3e7ca13232a3afff2353e29badfab7f73011edd4ced9"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.push_back(CDNSSeedData("bootstrap-testnet.viacoin.net", "testnet.viacoin.net"));
        vSeeds.push_back(CDNSSeedData("viacoin.net", "seed-testnet.viacoin.net"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,127);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,255);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = true;


        checkpointData = (CCheckpointData) {
                boost::assign::map_list_of
                ( 4230, uint256S("0x15a29dde01cbad777180c089bc8fcf0d7b4bd18993b47d8c301c41fc90ce8c8f")),
                1405625749,
                4440,
                5000
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 5400;
        consensus.nMajorityEnforceBlockUpgrade = 7500;
        consensus.nMajorityRejectBlockOutdated = 9500;
        consensus.nMajorityWindow = 10000;
        consensus.BIP34Height = -1;
        consensus.BIP34Hash = uint256();
        consensus.powLimit = uint256S("efffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint25(0) >> 1
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // !  two weeks
        consensus.nPowTargetSpacing = 1 * 24;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.nCLTVStartBlock = 1;
        consensus.nBIP66MinStartBlock = 1;
        consensus.nAuxPowStartHeight = AuxPow::START_REGTEST;

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        pchMessageStart[0] = 0x2d;
        pchMessageStart[1] = 0x97;
        pchMessageStart[2] = 0x7b;
        pchMessageStart[3] = 0x37;
        nDefaultPort = 15224;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1405166035, 0, 0x207fffff, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0xf0dae070f24fbc35311533a22aa85c0a616c84a1f22881612304d802acda286f"));
        // assert(genesis.hashMerkleRoot == uint256S("0x97ddfbbae6be97fd6cdf3e7ca13232a3afff2353e29badfab7f73011edd4ced9"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;

	checkpointData = (CCheckpointData){
         boost::assign::map_list_of
            ( 0, uint256S("0xf0dae070f24fbc35311533a22aa85c0a616c84a1f22881612304d802acda286f")),
            1405166035,
			0,
			0
	};

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
    }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}
