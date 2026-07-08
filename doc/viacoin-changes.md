# Viacoin 30.x curated patch series (draft)

Draft for Claude review. This file summarizes the curated `via-patches-30.x` branch after the hard invariant was checked against `v30.2.0`.

The 12 commits are intended to be net-equivalent to the organic 30.x Viacoin history while keeping future Bitcoin Core rebases reviewable by logical subsystem.

## Acceptance invariant

Before this draft file was amended into the docs commit, the branch tree matched `v30.2.0` exactly:

```bash
git diff --exit-code via-patches-30.x v30.2.0
```

After this draft is present, the expected check excludes only this file:

```bash
git diff --exit-code via-patches-30.x v30.2.0 -- ':!doc/viacoin-changes.md'
```

## 1. `identity` — viacoin: network and app identity — chainparams base, seeds, genesis params

Purpose: Carries Viacoin network identity over the Bitcoin Core 0.30 base: chain parameter bases, DNS seed material, magic/port-facing identity, and validation assumptions that make the node identify as Viacoin rather than Bitcoin.

Upstream subsystems touched: core application code, seed generation, unit tests.

Key files by size of change:
- `src/chainparamsseeds.h` (+15/-1316)
- `contrib/seeds/nodes_main.txt` (+11/-849)
- `contrib/seeds/nodes_test.txt` (+0/-179)
- `contrib/seeds/nodes_testnet4.txt` (+0/-164)
- `src/test/validation_tests.cpp` (+125/-26)
- `contrib/seeds/nodes_signet.txt` (+0/-123)
- `contrib/seeds/README.md` (+49/-22)
- `contrib/seeds/makeseeds.py` (+5/-17)
- `src/chainparamsbase.cpp` (+3/-3)

## 2. `scrypt` — viacoin: scrypt proof-of-work — primitives, GetPoWHash callers, SSE2 acceleration

Purpose: Switches proof-of-work plumbing from Bitcoin double-SHA256 header hashing to Viacoin scrypt hashing. The commit wires the hash primitive through mining, validation tests, utility code, and optimized crypto implementations.

Upstream subsystems touched: RPC, block primitives, core application code, crypto primitives, fuzz tests, unit tests.

Key files by size of change:
- `src/crypto/scrypt.cpp` (+225/-0)
- `src/rpc/mining.cpp` (+126/-2)
- `src/crypto/scrypt-sse2.cpp` (+117/-0)
- `src/crypto/scrypt.h` (+69/-0)
- `src/test/scrypt_tests.cpp` (+57/-0)
- `src/crypto/CMakeLists.txt` (+14/-2)
- `src/primitives/block.cpp` (+13/-2)
- `src/test/util/setup_common.cpp` (+7/-7)
- `src/test/validation_block_tests.cpp` (+7/-5)
- `src/init.cpp` (+11/-1)

## 3. `agw` — viacoin: AGW difficulty retargeting

Purpose: Ports Viacoin's difficulty retargeting behavior through the proof-of-work module. The change isolates the retargeting rule where consensus code evaluates work requirements.

Upstream subsystems touched: consensus/validation.

Key files by size of change:
- `src/pow.cpp` (+160/-49)

## 4. `subsidy` — viacoin: subsidy schedule, coinbase maturity, and consensus constants

Purpose: Applies Viacoin monetary constants and subsidy/maturity behavior. The changes keep amount ranges, wallet coinbase handling, consensus checks, and related fuzz/tests aligned with Viacoin economics.

Upstream subsystems touched: Qt GUI, consensus/validation, fuzz tests, unit tests, wallet.

Key files by size of change:
- `src/test/pow_tests.cpp` (+697/-59)
- `src/test/amount_tests.cpp` (+9/-4)
- `src/consensus/consensus.h` (+5/-4)
- `src/test/fuzz/utxo_snapshot.cpp` (+4/-4)
- `src/test/fuzz/tx_pool.cpp` (+4/-3)
- `src/consensus/tx_verify.cpp` (+4/-3)
- `src/wallet/wallet.cpp` (+3/-2)
- `src/test/fuzz/package_eval.cpp` (+2/-2)
- `src/qt/test/rpcnestedtests.cpp` (+2/-1)
- `src/test/fuzz/process_messages.cpp` (+1/-1)

## 5. `versionbits` — viacoin: versionbits layout, activation heights, and deployment params

Purpose: Ports deployment and activation layout to Viacoin. This commit updates consensus deployment parameters, versionbits state handling, activation heights, and tests that encode those assumptions.

Upstream subsystems touched: RPC, consensus/validation, core application code, functional test framework, fuzz tests, kernel chain parameters, node/P2P/storage, unit tests.

Key files by size of change:
- `src/kernel/chainparams.cpp` (+205/-191)
- `src/validation.cpp` (+179/-48)
- `src/test/versionbits_tests.cpp` (+140/-0)
- `test/functional/feature_block.py` (+38/-14)
- `src/rpc/blockchain.cpp` (+19/-13)
- `src/validation.h` (+27/-1)
- `src/consensus/params.h` (+10/-1)
- `src/versionbits.h` (+4/-4)
- `src/test/fuzz/versionbits.cpp` (+5/-3)
- `src/deploymentinfo.cpp` (+5/-1)

## 6. `auxpow` — viacoin: auxiliary proof-of-work (merged mining) support

Purpose: Adds Viacoin auxiliary proof-of-work support for merged mining. The changes add AuxPoW data structures and validation hooks, then thread them through block storage, P2P/RPC, mining helpers, and functional-test block construction.

Upstream subsystems touched: AuxPoW consensus support, benchmarks, block primitives, consensus/validation, crypto primitives, functional test framework, fuzz tests, node/P2P/storage, unit tests.

Key files by size of change:
- `src/test/auxpow_tests.cpp` (+218/-0)
- `src/crypto/scrypt-avx2.cpp` (+147/-0)
- `src/primitives/blockheader.h` (+95/-0)
- `src/net_processing.cpp` (+59/-16)
- `src/auxpow/check.h` (+75/-0)
- `src/auxpow/auxpow.h` (+72/-0)
- `src/primitives/block.h` (+3/-57)
- `src/node/blockstorage.cpp` (+51/-5)
- `src/test/rpc_tests.cpp` (+22/-9)
- `src/auxpow/consensus.h` (+26/-0)

## 7. `fees` — viacoin: legacy port — 100x fee scale, core entry-point tuning

Purpose: Applies the legacy Viacoin fee scale and command entry-point tuning. It updates policy/RPC/wallet fee handling and the top-level binary entry points that still inherit Bitcoin naming or assumptions.

Upstream subsystems touched: RPC, core application code, macOS deployment, node/P2P/storage, policy/mempool, repository metadata, wallet.

Key files by size of change:
- `src/wallet/rpc/spend.cpp` (+25/-25)
- `src/bitcoin-cli.cpp` (+21/-21)
- `src/bitcoin.cpp` (+8/-8)
- `CMakeLists.txt` (+10/-5)
- `src/wallet/rpc/coins.cpp` (+7/-7)
- `src/wallet/rpc/addresses.cpp` (+7/-7)
- `src/bitcoin-tx.cpp` (+6/-6)
- `src/rpc/rawtransaction.cpp` (+5/-5)
- `src/bitcoin-wallet.cpp` (+5/-5)
- `src/wallet/wallet.h` (+4/-4)

## 8. `branding` — viacoin: branding — binary OUTPUT_NAMEs, ViacoinTranslator, icons, packaging

Purpose: Rebrands the application surface from Bitcoin to Viacoin. This includes executable/package names, Qt translation and UI strings, icons, plist/setup metadata, and GUI tests.

Upstream subsystems touched: CI/build tooling, Qt GUI, core application code, init wrappers, packaging resources.

Key files by size of change:
- `src/qt/bitcoin.cpp` (+85/-4)
- `src/qt/res/src/bitcoin.svg` (+26/-58)
- `src/qt/guiutil.cpp` (+24/-31)
- `src/qt/test/uritests.cpp` (+15/-15)
- `src/qt/res/icons/bitcoin.svg` (+26/-0)
- `share/examples/bitcoin.conf` (+9/-9)
- `src/qt/platformstyle.cpp` (+16/-1)
- `src/qt/paymentserver.cpp` (+8/-8)
- `src/clientversion.cpp` (+11/-4)
- `src/qt/guiconstants.h` (+7/-7)

## 9. `perf` — viacoin: IBD and runtime performance tuning

Purpose: Applies runtime and IBD performance tuning that is specific to the Viacoin port. The changes touch block-manager defaults and wallet/node interface paths that affect startup or sync behavior.

Upstream subsystems touched: Qt GUI, kernel chain parameters, node/P2P/storage, wallet.

Key files by size of change:
- `src/node/blockmanager_args.cpp` (+5/-0)
- `src/wallet/interfaces.cpp` (+0/-3)
- `src/kernel/blockmanager_opts.h` (+3/-0)
- `src/qt/walletmodel.cpp` (+1/-1)

## 10. `testharness` — viacoin: test suite and fuzz harness alignment

Purpose: Aligns tests, fuzz targets, and framework helpers with the Viacoin port without changing consensus intent. The commit updates fixtures, harness constants, and test utility behavior that would otherwise encode Bitcoin-specific assumptions.

Upstream subsystems touched: benchmarks, consensus/validation, core application code, functional test framework, fuzz tests, kernel chain parameters, policy/mempool, unit tests.

Key files by size of change:
- `src/test/data/key_io_valid.json` (+150/-150)
- `src/test/descriptor_tests.cpp` (+80/-80)
- `src/test/miner_tests.cpp` (+25/-85)
- `src/test/bip324_tests.cpp` (+41/-41)
- `src/test/txpackage_tests.cpp` (+35/-27)
- `src/test/transaction_tests.cpp` (+25/-24)
- `test/functional/feature_config_args.py` (+22/-22)
- `test/functional/test_framework/test_framework.py` (+20/-15)
- `src/test/util_tests.cpp` (+24/-11)
- `src/test/mempool_tests.cpp` (+19/-14)

## 11. `ci` — viacoin: CI and build tooling — self-hosted runners, cmake fixes

Purpose: Ports CI and build-tooling details for the Viacoin build environment. It updates GitHub workflow wiring, CMake maintenance helpers, ignored build artifacts, and component install logic.

Upstream subsystems touched: CI/build tooling, benchmarks, unit tests, wallet.

Key files by size of change:
- `.github/workflows/ci.yml` (+39/-23)
- `AGENTS.md` (+47/-0)
- `cmake/module/Maintenance.cmake` (+11/-11)
- `cmake/module/GenerateSetupNsi.cmake` (+8/-8)
- `cmake/introspection.cmake` (+15/-0)
- `cmake/module/InstallBinaryComponent.cmake` (+6/-1)
- `src/test/bloom_tests.cpp` (+4/-2)
- `ci/test/03_test_script.sh` (+3/-3)
- `src/wallet/CMakeLists.txt` (+1/-2)
- `src/bench/CMakeLists.txt` (+3/-0)

## 12. `docs` — viacoin: docs and release engineering — migration guide, notes, Guix naming

Purpose: Ports documentation and release-engineering names to Viacoin. This includes manpages, migration/build documents, assumeutxo notes, init examples, and Guix release artifact naming.

Upstream subsystems touched: Guix release tooling, core application code, documentation, service/init examples.

Key files by size of change:
- `doc/man/viacoin-qt.1` (+852/-0)
- `doc/man/viacoind.1` (+830/-0)
- `doc/viacoin-migration.md` (+292/-0)
- `doc/release-notes.md` (+151/-90)
- `doc/viacoin-build-unix.md` (+223/-0)
- `doc/man/viacoin-cli.1` (+201/-0)
- `doc/viacoin-assumeutxo.md` (+176/-0)
- `doc/man/viacoin-tx.1` (+159/-0)
- `doc/man/viacoin-wallet.1` (+113/-0)
- `doc/man/viacoin-util.1` (+78/-0)
