// Copyright (c) 2023 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bench/bench.h>
#include <flatfile.h>
#include <node/blockstorage.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <serialize.h>
#include <span.h>
#include <streams.h>
#include <test/util/setup_common.h>
#include <validation.h>

#include <cassert>
#include <cstdint>
#include <memory>
#include <vector>

static CBlock CreateTestBlock(TestChain100Setup& test_setup)
{
    const CScript script_pub_key = CScript() << ToByteVector(test_setup.coinbaseKey.GetPubKey()) << OP_CHECKSIG;
    return test_setup.CreateBlock({}, script_pub_key, test_setup.m_node.chainman->ActiveChainstate());
}

static void WriteBlockBench(benchmark::Bench& bench)
{
    const auto testing_setup{MakeNoLogFileContext<TestChain100Setup>()};
    auto& blockman{testing_setup->m_node.chainman->m_blockman};
    const CBlock block{CreateTestBlock(*testing_setup)};
    bench.run([&] {
        const auto pos{blockman.WriteBlock(block, 413'567)};
        assert(!pos.IsNull());
    });
}

static void ReadBlockBench(benchmark::Bench& bench)
{
    const auto testing_setup{MakeNoLogFileContext<TestChain100Setup>()};
    auto& blockman{testing_setup->m_node.chainman->m_blockman};
    const auto& test_block{CreateTestBlock(*testing_setup)};
    const auto& expected_hash{test_block.GetHash()};
    const auto& pos{blockman.WriteBlock(test_block, 413'567)};
    bench.run([&] {
        CBlock block;
        const auto success{blockman.ReadBlock(block, pos, expected_hash)};
        assert(success);
    });
}

static void ReadRawBlockBench(benchmark::Bench& bench)
{
    const auto testing_setup{MakeNoLogFileContext<TestChain100Setup>()};
    auto& blockman{testing_setup->m_node.chainman->m_blockman};
    const auto pos{blockman.WriteBlock(CreateTestBlock(*testing_setup), 413'567)};
    std::vector<std::byte> block_data;
    blockman.ReadRawBlock(block_data, pos); // warmup
    bench.run([&] {
        const auto success{blockman.ReadRawBlock(block_data, pos)};
        assert(success);
    });
}

BENCHMARK(WriteBlockBench, benchmark::PriorityLevel::HIGH);
BENCHMARK(ReadBlockBench, benchmark::PriorityLevel::HIGH);
BENCHMARK(ReadRawBlockBench, benchmark::PriorityLevel::HIGH);
