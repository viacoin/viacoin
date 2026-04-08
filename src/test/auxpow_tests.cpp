// Copyright (c) 2026 The Viacoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include <auxpow/auxpow.h>
#include <auxpow/check.h>
#include <auxpow/consensus.h>
#include <chain.h>
#include <chainparams.h>
#include <crypto/common.h>
#include <hash.h>
#include <primitives/block.h>
#include <script/script.h>
#include <streams.h>
#include <test/util/setup_common.h>

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(auxpow_tests, BasicTestingSetup)

namespace {
CBlockHeader MakeBaseAuxpowHeader()
{
    CBlockHeader header;
    header.nVersion = AuxPow::BLOCK_VERSION_DEFAULT |
        AuxPow::BLOCK_VERSION_AUXPOW |
        (AuxPow::CHAIN_ID * AuxPow::BLOCK_VERSION_CHAIN_START);
    header.hashPrevBlock = uint256{1};
    header.hashMerkleRoot = uint256{2};
    header.nTime = 1234567890;
    header.nBits = 0x1f00ffff;
    header.nNonce = 42;
    return header;
}

std::shared_ptr<CAuxPow> MakeAuxpowPayload(uint32_t parent_nonce)
{
    auto auxpow = std::make_shared<CAuxPow>();
    CMutableTransaction tx;
    tx.version = 2;
    tx.vin.resize(1);
    tx.vout.resize(1);
    auxpow->tx = MakeTransactionRef(tx);
    auxpow->nIndex = 0;
    auxpow->vMerkleBranch = {uint256{3}};
    auxpow->vChainMerkleBranch = {uint256{4}};
    auxpow->nChainIndex = 0;
    auxpow->parentBlockHeader.nVersion = 7;
    auxpow->parentBlockHeader.hashPrevBlock = uint256{5};
    auxpow->parentBlockHeader.hashMerkleRoot = uint256{6};
    auxpow->parentBlockHeader.nTime = 1234567999;
    auxpow->parentBlockHeader.nBits = 0x1f00aaaa;
    auxpow->parentBlockHeader.nNonce = parent_nonce;
    return auxpow;
}

std::vector<unsigned char> SerializeUint32(uint32_t value)
{
    std::array<unsigned char, 4> bytes;
    WriteLE32(bytes.data(), value);
    return {bytes.begin(), bytes.end()};
}

std::shared_ptr<CAuxPow> MakeValidAuxpow(const uint256& aux_block_hash, unsigned int chain_id, uint32_t nonce = 7)
{
    auto auxpow = std::make_shared<CAuxPow>();
    auxpow->nIndex = 0;
    auxpow->vMerkleBranch.clear();
    auxpow->vChainMerkleBranch.clear();

    uint32_t slot = nonce;
    slot = slot * 1103515245 + 12345;
    slot += chain_id;
    slot = slot * 1103515245 + 12345;
    auxpow->nChainIndex = slot % 1;
    auxpow->parentBlockHeader.nVersion = 1;
    auxpow->parentBlockHeader.hashPrevBlock = uint256{21};
    auxpow->parentBlockHeader.nTime = 1234568000;
    auxpow->parentBlockHeader.nBits = 0x1f00ffff;
    auxpow->parentBlockHeader.nNonce = nonce;

    CMutableTransaction tx;
    tx.version = 2;
    tx.vin.resize(1);
    tx.vout.resize(1);

    std::vector<unsigned char> root_bytes(aux_block_hash.begin(), aux_block_hash.end());
    std::reverse(root_bytes.begin(), root_bytes.end());

    std::vector<unsigned char> payload{0xfa, 0xbe, 'm', 'm'};
    payload.insert(payload.end(), root_bytes.begin(), root_bytes.end());
    const auto tree_size = SerializeUint32(1);
    payload.insert(payload.end(), tree_size.begin(), tree_size.end());
    const auto nonce_bytes = SerializeUint32(nonce);
    payload.insert(payload.end(), nonce_bytes.begin(), nonce_bytes.end());

    tx.vin[0].scriptSig = CScript() << payload;

    auxpow->tx = MakeTransactionRef(tx);
    auxpow->parentBlockHeader.hashMerkleRoot = auxpow->tx->GetHash().ToUint256();
    return auxpow;
}
} // namespace

BOOST_AUTO_TEST_CASE(block_header_reports_auxpow_bit_and_chain_id)
{
    CBlockHeader header = MakeBaseAuxpowHeader();

    BOOST_CHECK(header.IsAuxPow());
    BOOST_CHECK_EQUAL(header.GetChainID(), AuxPow::CHAIN_ID);
}

BOOST_AUTO_TEST_CASE(block_header_hash_excludes_auxpow_payload_but_serialization_keeps_it)
{
    CBlockHeader header_one = MakeBaseAuxpowHeader();
    header_one.SetAuxPow(new CAuxPow(*MakeAuxpowPayload(1)));

    CBlockHeader header_two = MakeBaseAuxpowHeader();
    header_two.SetAuxPow(new CAuxPow(*MakeAuxpowPayload(2)));

    BOOST_CHECK(header_one.IsAuxPow());
    BOOST_CHECK(header_two.IsAuxPow());
    BOOST_CHECK_EQUAL(header_one.GetHash(), header_two.GetHash());

    DataStream stream_one{};
    DataStream stream_two{};
    stream_one << header_one;
    stream_two << header_two;

    BOOST_CHECK(stream_one.size() > 80);
    BOOST_CHECK_EQUAL(stream_one.size(), stream_two.size());
    BOOST_CHECK(std::vector<std::byte>(stream_one.begin(), stream_one.end()) != std::vector<std::byte>(stream_two.begin(), stream_two.end()));
}

BOOST_AUTO_TEST_CASE(auxpow_parent_block_hash_uses_parent_pow_hash)
{
    auto auxpow = MakeAuxpowPayload(999);

    BOOST_CHECK_EQUAL(auxpow->GetParentBlockHash(), auxpow->parentBlockHeader.GetPoWHash());
    BOOST_CHECK(auxpow->GetParentBlockHash() != auxpow->parentBlockHeader.GetHash());
}

BOOST_AUTO_TEST_CASE(auxpow_check_merkle_branch_matches_legacy_hash_order)
{
    auto auxpow = MakeAuxpowPayload(123);
    const uint256 leaf{11};
    const std::vector<uint256> branch{uint256{12}, uint256{13}};

    const uint256 left_then_right = Hash(Hash(leaf, branch[0]), branch[1]);
    const uint256 right_then_left = Hash(Hash(branch[0], leaf), branch[1]);
    const uint256 mixed_order = Hash(Hash(leaf, branch[0]), Hash(branch[1]));

    BOOST_CHECK_EQUAL(auxpow->CheckMerkleBranch(leaf, branch, 0), left_then_right);
    BOOST_CHECK_EQUAL(auxpow->CheckMerkleBranch(leaf, branch, 1), right_then_left);
    BOOST_CHECK_EQUAL(auxpow->CheckMerkleBranch(leaf, branch, -1), uint256());
    BOOST_CHECK(auxpow->CheckMerkleBranch(leaf, branch, 0) != mixed_order);
}

BOOST_AUTO_TEST_CASE(check_auxpow_accepts_minimal_valid_auxpow)
{
    const uint256 aux_block_hash{31};
    auto auxpow = MakeValidAuxpow(aux_block_hash, AuxPow::CHAIN_ID);
    const auto params = CreateChainParams(*m_node.args, ChainType::MAIN);

    BOOST_CHECK(CheckAuxpow(auxpow, aux_block_hash, AuxPow::CHAIN_ID, params->GetConsensus()));
}

BOOST_AUTO_TEST_CASE(check_auxpow_rejects_non_generate_coinbase)
{
    const uint256 aux_block_hash{32};
    auto auxpow = MakeValidAuxpow(aux_block_hash, AuxPow::CHAIN_ID);
    const auto params = CreateChainParams(*m_node.args, ChainType::MAIN);
    auxpow->nIndex = 1;

    BOOST_CHECK(!CheckAuxpow(auxpow, aux_block_hash, AuxPow::CHAIN_ID, params->GetConsensus()));
}

BOOST_AUTO_TEST_CASE(block_index_getblockheader_preserves_auxpow_payload)
{
    CBlockHeader header = MakeBaseAuxpowHeader();
    header.SetAuxPow(new CAuxPow(*MakeAuxpowPayload(77)));

    CBlockIndex index{header};
    CBlockHeader restored = index.GetBlockHeader();

    BOOST_REQUIRE(restored.IsAuxPow());
    BOOST_REQUIRE(restored.auxpow);
    BOOST_CHECK_EQUAL(restored.nVersion, header.nVersion);
    BOOST_CHECK_EQUAL(restored.hashMerkleRoot, header.hashMerkleRoot);
    BOOST_CHECK_EQUAL(restored.auxpow->GetParentBlockHash(), header.auxpow->GetParentBlockHash());
}

BOOST_AUTO_TEST_CASE(disk_block_index_roundtrip_keeps_auxpow_payload)
{
    CBlockHeader header = MakeBaseAuxpowHeader();
    header.SetAuxPow(new CAuxPow(*MakeAuxpowPayload(91)));

    CBlockIndex index{header};
    CDiskBlockIndex disk_index{&index};

    DataStream stream{};
    stream << disk_index;

    CDiskBlockIndex restored;
    stream >> restored;

    BOOST_REQUIRE(restored.auxpow);
    BOOST_CHECK_EQUAL(restored.nVersion, header.nVersion);
    BOOST_CHECK_EQUAL(restored.auxpow->GetParentBlockHash(), header.auxpow->GetParentBlockHash());
}

BOOST_AUTO_TEST_SUITE_END()
