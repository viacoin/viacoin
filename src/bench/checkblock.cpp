// Copyright (c) 2016-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bench/bench.h>
#include <chainparams.h>
#include <common/args.h>
#include <consensus/validation.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <serialize.h>
#include <span.h>
#include <streams.h>
#include <test/util/setup_common.h>
#include <util/chaintype.h>
#include <util/strencodings.h>
#include <validation.h>

#include <cassert>
#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

// These are the two major time-sinks which happen after we have fully received
// a block off the wire, but before we can relay the block on to peers using
// compact block relay.

static std::vector<std::byte> CreateBenchBlockData()
{
    const auto test_setup{MakeNoLogFileContext<TestChain100Setup>()};
    const CScript script_pub_key = CScript() << ToByteVector(test_setup->coinbaseKey.GetPubKey()) << OP_CHECKSIG;
    const CBlock block = test_setup->CreateBlock({}, script_pub_key, test_setup->m_node.chainman->ActiveChainstate());
    DataStream stream{};
    stream << TX_WITH_WITNESS(block);
    return {stream.begin(), stream.end()};
}

static void DeserializeBlockTest(benchmark::Bench& bench)
{
    static const std::vector<std::byte> block_data = CreateBenchBlockData();
    DataStream stream{block_data};
    std::byte a{0};
    stream.write({&a, 1}); // Prevent compaction

    bench.unit("block").run([&] {
        CBlock block;
        stream >> TX_WITH_WITNESS(block);
        bool rewound = stream.Rewind(block_data.size());
        assert(rewound);
    });
}

static void DeserializeAndCheckBlockTest(benchmark::Bench& bench)
{
    static const std::vector<std::byte> block_data = CreateBenchBlockData();
    DataStream stream{block_data};
    std::byte a{0};
    stream.write({&a, 1}); // Prevent compaction

    ArgsManager bench_args;
    const auto chainParams = CreateChainParams(bench_args, ChainType::REGTEST);

    bench.unit("block").run([&] {
        CBlock block; // Note that CBlock caches its checked state, so we need to recreate it here
        stream >> TX_WITH_WITNESS(block);
        bool rewound = stream.Rewind(block_data.size());
        assert(rewound);

        BlockValidationState validationState;
        bool checked = CheckBlock(block, validationState, chainParams->GetConsensus());
        assert(checked);
    });
}

BENCHMARK(DeserializeBlockTest, benchmark::PriorityLevel::HIGH);
BENCHMARK(DeserializeAndCheckBlockTest, benchmark::PriorityLevel::HIGH);
