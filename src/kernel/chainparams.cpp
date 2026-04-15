// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <hash.h>
#include <kernel/messagestartchars.h>
#include <logging.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/strencodings.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

using namespace util::hex_literals;

// Workaround MSVC bug triggering C7595 when calling consteval constructors in
// initializer lists.
// https://developercommunity.visualstudio.com/t/Bogus-C7595-error-on-valid-C20-code/10906093
#if defined(_MSC_VER)
auto consteval_ctor(auto&& input) { return input; }
#else
#define consteval_ctor(input) (input)
#endif

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.version = 1;
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
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    const CScript genesisOutputScript = CScript() << "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"_hex << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

static CBlock CreateViacoinGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Block #310379:0000000000000000241252a7bb27be97befe9218c1290dff36c1ff129ec7211a";
    const CScript genesisOutputScript = CScript() << "0459934a6a228ce9716fa0b13aa1cdc01593fca5f8599473c803a5109ff834dfdaf4c9ee35f2218c9ee3e7cf7db734e1179524b9d6ae8ebbeba883d4cb89b6c7bf"_hex << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network on which people trade goods and services.
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        m_chain_type = ChainType::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 657000;
        consensus.BIP16Height = 0;
        consensus.script_flag_exceptions.emplace( // BIP16 exception
            uint256{"00000000000002dc756eebf4f49723ed8d30cc28a5f108eb94b1ba88ac4f9c22"}, SCRIPT_VERIFY_NONE);
        consensus.script_flag_exceptions.emplace( // Taproot exception
            uint256{"0000000000000000000f14c35b2d841e986ab5441de8c585d5ffe55ea1e395ad"}, SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_WITNESS);
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256{"4e9b54001f9976049830128ec0331515eaabe35a70970d79971da1539a400ba1"};
        consensus.BIP65Height = 598725;
        consensus.BIP66Height = 1421641;
        consensus.BlockVer5Height = 1976295;
        consensus.nAuxPowStartHeight = AuxPow::START_MAINNET;
        consensus.nWitnessStartHeight = 4040000;
        consensus.CSVHeight = 419328; // script-flag tranche will align witness-era activation path separately
        consensus.SegwitHeight = 481824; // script-flag tranche will align witness-era activation path separately
        consensus.MinBIP9WarningHeight = 483840; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256{"000001ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60;
        consensus.nPowTargetSpacing = 24;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 6;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 8100;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].period = 10800;

        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].nStartTime = 1498694400;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].nTimeout = 1530230400;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].threshold = 8100;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].period = 10800;

        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].nStartTime = 1498694400;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].nTimeout = 1530230400;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].threshold = 8100;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].period = 10800;

        consensus.nMinimumChainWork = uint256{"0000000000000000000000000000000000000000000003967cac56f2a1b39348"};
        consensus.defaultAssumeValid = uint256{"63898baba0b3a6aefe911fcc4877422510651cdaee3f6da5aa8d693849650975"}; // 13537313

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
        m_assumed_blockchain_size = 15;
        m_assumed_chain_state_size = 2;

        genesis = CreateViacoinGenesisBlock(1405164774, 4016033, 0x1e01ffff, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"4e9b54001f9976049830128ec0331515eaabe35a70970d79971da1539a400ba1"});
        assert(genesis.hashMerkleRoot == uint256{"0317d32e01a2adf6f2ac6f58c7cdaab6c656edc6fdb45986c739290053275200"});

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as an addrfetch if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("seed.viacoin.net"); // hosted by @Sprux
        vSeeds.emplace_back("viaseeder.barbatos.fr"); // hosted by barbatos
        vSeeds.emplace_back("mainnet.viacoin.net"); // hosted by @Sprux

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,71);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,33);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,199);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "via";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_main), std::end(chainparams_seed_main));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        m_assumeutxo_data = {};

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 b057393d1af96736631886cbf3321d3889874b391addbcdfbfef0bbc948c6c0e
            .nTime    = 1776203386,
            .tx_count = 14337559,
            .dTxRate  = 0.03429767636592004,
        };
    }
};

/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        m_chain_type = ChainType::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP16Height = -1;
        consensus.script_flag_exceptions.emplace( // BIP16 exception
            uint256{"00000000dd30457c001f4095d208cc1296b0eed002427aa599874af7a432b105"}, SCRIPT_VERIFY_NONE);
        consensus.BIP34Height = -1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 800000;
        consensus.BIP66Height = 502664;
        consensus.BlockVer5Height = 1800000;
        consensus.nAuxPowStartHeight = AuxPow::START_TESTNET;
        consensus.nWitnessStartHeight = 4040000;
        consensus.CSVHeight = 770112; // script-flag tranche will align witness-era activation path separately
        consensus.SegwitHeight = 834624; // script-flag tranche will align witness-era activation path separately
        consensus.MinBIP9WarningHeight = 836640; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256{"00001fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60;
        consensus.nPowTargetSpacing = 24;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 6;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1232032894;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 2700;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].period = 3600;

        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].nStartTime = 1494547200;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].nTimeout = 1526083200;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].threshold = 2700;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].period = 3600;

        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].nStartTime = 1494547200;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].nTimeout = 1526083200;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].threshold = 2700;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].period = 3600;

        consensus.nMinimumChainWork = uint256{"0000000000000000000000000000000000000000000016dd270dd94fac1d7632"};
        consensus.defaultAssumeValid = uint256{"0000000000000065c6c38258e201971a3fdfcc2ceee0dd6e85a6c022d45dee34"}; // 4550000

        pchMessageStart[0] = 0xa9;
        pchMessageStart[1] = 0xc5;
        pchMessageStart[2] = 0xef;
        pchMessageStart[3] = 0x92;
        nDefaultPort = 25223;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 5;
        m_assumed_chain_state_size = 1;

        genesis = CreateViacoinGenesisBlock(1405168772, 262929, 0x1e1fffff, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"770aa712aa08fdcbdecc1c8df1b3e2d4e17a7cf6e63a28b785b32e74c96cb27d"});
        assert(genesis.hashMerkleRoot == uint256{"0317d32e01a2adf6f2ac6f58c7cdaab6c656edc6fdb45986c739290053275200"});

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet.viacoin.net");
        vSeeds.emplace_back("seed-testnet.viacoin.net");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,127);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,255);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tvia";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_test), std::end(chainparams_seed_test));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        m_assumeutxo_data = {};

        chainTxData = ChainTxData{
            // Data from rpc: getchaintxstats 4096 00000000000001c200b9790dc637d3bb141fe77d155b966ed775b17e109f7c6c
            .nTime    = 1405625749,
            .tx_count = 4440,
            .dTxRate  = 5000,
        };
    }
};

/**
 * Testnet (v4): public test network which is reset from time to time.
 */
class CTestNet4Params : public CChainParams {
public:
    CTestNet4Params() {
        m_chain_type = ChainType::TESTNET4;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = true;
        consensus.fPowNoRetargeting = false;

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 6;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 1512;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].period = 2016;

        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].threshold = 1512;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].period = 2016;

        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].threshold = 1512;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].period = 2016;

        consensus.nMinimumChainWork = uint256{"00000000000000000000000000000000000000000000034a4690fe592dc49c7c"};
        consensus.defaultAssumeValid = uint256{"000000000000000180a58e7fa3b0db84b5ea76377524894f53660d93ac839d9b"}; // 91000

        pchMessageStart[0] = 0x1c;
        pchMessageStart[1] = 0x16;
        pchMessageStart[2] = 0x3f;
        pchMessageStart[3] = 0x28;
        nDefaultPort = 48333;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 2;
        m_assumed_chain_state_size = 1;

        const char* testnet4_genesis_msg = "03/May/2024 000000000000000000001ebd58c244970b3aa9d783bb001011fbe8ea8e98e00e";
        const CScript testnet4_genesis_script = CScript() << "000000000000000000000000000000000000000000000000000000000000000000"_hex << OP_CHECKSIG;
        genesis = CreateGenesisBlock(testnet4_genesis_msg,
                testnet4_genesis_script,
                1714777860,
                393743547,
                0x1d00ffff,
                1,
                50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"00000000da84f2bafbbc53dee25a72ae507ff4914b867c565be350b0da8bf043"});
        assert(genesis.hashMerkleRoot == uint256{"7aa0a7ae1e223414cb807e40cd57e667b718e42aaf9306db9102fe28912b7b4e"});

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("seed.testnet4.bitcoin.sprovoost.nl."); // Sjors Provoost
        vSeeds.emplace_back("seed.testnet4.wiz.biz."); // Jason Maurice

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tb";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_testnet4), std::end(chainparams_seed_testnet4));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        m_assumeutxo_data = {};

        chainTxData = ChainTxData{
            // Viacoin testnet4 has minimal activity
            .nTime    = 1752470331,
            .tx_count = 0,
            .dTxRate  = 0,
        };
    }
};

/**
 * Signet: test network with an additional consensus parameter (see BIP325).
 */
class SigNetParams : public CChainParams {
public:
    explicit SigNetParams(const SigNetOptions& options)
    {
        std::vector<uint8_t> bin;
        vFixedSeeds.clear();
        vSeeds.clear();

        if (!options.challenge) {
            bin = "512103ad5e0edad18cb1f0fc0d28a3d4f1f3e445640337489abb10404f2d1e086be430210359ef5021964fe22d6f8e05b2463c9540ce96883fe3b278760f048f5189f2e6c452ae"_hex_v_u8;
            vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_signet), std::end(chainparams_seed_signet));
            vSeeds.emplace_back("seed.signet.bitcoin.sprovoost.nl.");
            vSeeds.emplace_back("seed.signet.achownodes.xyz."); // Ava Chow, only supports x1, x5, x9, x49, x809, x849, xd, x400, x404, x408, x448, xc08, xc48, x40c

            consensus.nMinimumChainWork = uint256{"0000000000000000000000000000000000000000000000000000067d328e681a"};
            consensus.defaultAssumeValid = uint256{"000000128586e26813922680309f04e1de713c7542fee86ed908f56368aefe2e"}; // 267665
            m_assumed_blockchain_size = 1;
            m_assumed_chain_state_size = 1;
            chainTxData = ChainTxData{
                // Data from RPC: getchaintxstats 4096 000000128586e26813922680309f04e1de713c7542fee86ed908f56368aefe2e
                .nTime    = 1756723017,
                .tx_count = 26185472,
                .dTxRate  = 0.7452721495389969,
            };
        } else {
            bin = *options.challenge;
            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                0,
                0,
                0,
            };
            LogInfo("Signet with challenge %s", HexStr(bin));
        }

        if (options.seeds) {
            vSeeds = *options.seeds;
        }

        m_chain_type = ChainType::SIGNET;
        consensus.signet_blocks = true;
        consensus.signet_challenge.assign(bin.begin(), bin.end());
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"00000377ae000000000000000000000000000000000000000000000000000000"};
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 6;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 1815;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].period = 2016;

        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].threshold = 1815;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].period = 2016;

        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].threshold = 1815;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].period = 2016;

        // message start is defined as the first 4 bytes of the sha256d of the block script
        HashWriter h{};
        h << consensus.signet_challenge;
        uint256 hash = h.GetHash();
        std::copy_n(hash.begin(), 4, pchMessageStart.begin());

        nDefaultPort = 38333;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1598918400, 52613770, 0x1e0377ae, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"00000008819873e925422c1ff0f99f7cc9bbb232af63a077a480a3633bee1ef6"});
        assert(genesis.hashMerkleRoot == uint256{"4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"});

        m_assumeutxo_data = {
            {
                .height = 160'000,
                .hash_serialized = AssumeutxoHash{uint256{"fe0a44309b74d6b5883d246cb419c6221bcccf0b308c9b59b7d70783dbdf928a"}},
                .m_chain_tx_count = 2289496,
                .blockhash = consteval_ctor(uint256{"0000003ca3c99aff040f2563c2ad8f8ec88bd0fd6b8f0895cfaf1ef90353a62c"}),
            }
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tb";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
    }
};

/**
 * Regression test: intended for private networks only. Has minimal difficulty to ensure that
 * blocks can be found instantly.
 */
class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        m_chain_type = ChainType::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP16Height = 0;
        consensus.BIP34Height = 100000000;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1251;
        consensus.BIP66Height = 1351;
        consensus.BlockVer5Height = 1697078;
        consensus.nAuxPowStartHeight = AuxPow::START_REGTEST;
        consensus.nWitnessStartHeight = 20000;
        consensus.CSVHeight = 1;    // script-flag tranche will align witness-era activation path separately
        consensus.SegwitHeight = 0; // script-flag tranche will align witness-era activation path separately
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"efffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60;
        consensus.nPowTargetSpacing = 24;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = opts.enforce_bip94;
        consensus.fPowNoRetargeting = true;

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 6;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 108;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].period = 144;

        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].threshold = 108;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_CSV].period = 144;

        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].min_activation_height = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].threshold = 108;
        consensus.vDeployments[Consensus::DEPLOYMENT_VIACOIN_SEGWIT].period = 144;

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart[0] = 0x2d;
        pchMessageStart[1] = 0x97;
        pchMessageStart[2] = 0x7b;
        pchMessageStart[3] = 0x37;
        nDefaultPort = 15224;
        nPruneAfterHeight = opts.fastprune ? 100 : 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        for (const auto& [dep, height] : opts.activation_heights) {
            switch (dep) {
            case Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT:
                consensus.SegwitHeight = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB:
                consensus.BIP34Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_DERSIG:
                consensus.BIP66Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CLTV:
                consensus.BIP65Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CSV:
                consensus.CSVHeight = int{height};
                break;
            }
        }

        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height = version_bits_params.min_activation_height;
        }

        genesis = CreateViacoinGenesisBlock(1405166035, 0, 0x207fffff, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"f0dae070f24fbc35311533a22aa85c0a616c84a1f22881612304d802acda286f"});
        assert(genesis.hashMerkleRoot == uint256{"0317d32e01a2adf6f2ac6f58c7cdaab6c656edc6fdb45986c739290053275200"});

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();
        vSeeds.emplace_back("dummySeed.invalid.");

        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;

        m_assumeutxo_data = {
            {   // For use by unit tests
                .height = 110,
                .hash_serialized = AssumeutxoHash{uint256{"b952555c8ab81fec46f3d4253b7af256d766ceb39fb7752b9d18cdf4a0141327"}},
                .m_chain_tx_count = 111,
                .blockhash = consteval_ctor(uint256{"d0ef984810fbff08c6e80c79cc60dfaf8900b457a68a1748714d39e09d74d187"}),
            },
            {
                // For use by fuzz target src/test/fuzz/utxo_snapshot.cpp
                .height = 200,
                .hash_serialized = AssumeutxoHash{uint256{"17dcc016d188d16068907cdeb38b75691a118d43053b8cd6a25969419381d13a"}},
                .m_chain_tx_count = 201,
                .blockhash = consteval_ctor(uint256{"385901ccbd69dff6bbd00065d01fb8a9e464dede7cfe0372443884f9b1dcf6b9"}),
            },
            {
                // For use by test/functional/feature_assumeutxo.py
                .height = 299,
                .hash_serialized = AssumeutxoHash{uint256{"d2b051ff5e8eef46520350776f4100dd710a63447a8e01d917e92e79751a63e2"}},
                .m_chain_tx_count = 334,
                .blockhash = consteval_ctor(uint256{"7cc695046fec709f8c9394b6f928f81e81fd3ac20977bb68760fa1faa7916ea2"}),
            },
        };

        chainTxData = ChainTxData{
            .nTime = 0,
            .tx_count = 0,
            .dTxRate = 0.001, // Set a non-zero rate to make it testable
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "rvia";
    }
};

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions& options)
{
    return std::make_unique<const SigNetParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet4()
{
    return std::make_unique<const CTestNet4Params>();
}

std::vector<int> CChainParams::GetAvailableSnapshotHeights() const
{
    std::vector<int> heights;
    heights.reserve(m_assumeutxo_data.size());

    for (const auto& data : m_assumeutxo_data) {
        heights.emplace_back(data.height);
    }
    return heights;
}

std::optional<ChainType> GetNetworkForMagic(const MessageStartChars& message)
{
    const auto mainnet_msg = CChainParams::Main()->MessageStart();
    const auto testnet_msg = CChainParams::TestNet()->MessageStart();
    const auto testnet4_msg = CChainParams::TestNet4()->MessageStart();
    const auto regtest_msg = CChainParams::RegTest({})->MessageStart();
    const auto signet_msg = CChainParams::SigNet({})->MessageStart();

    if (std::ranges::equal(message, mainnet_msg)) {
        return ChainType::MAIN;
    } else if (std::ranges::equal(message, testnet_msg)) {
        return ChainType::TESTNET;
    } else if (std::ranges::equal(message, testnet4_msg)) {
        return ChainType::TESTNET4;
    } else if (std::ranges::equal(message, regtest_msg)) {
        return ChainType::REGTEST;
    } else if (std::ranges::equal(message, signet_msg)) {
        return ChainType::SIGNET;
    }
    return std::nullopt;
}
