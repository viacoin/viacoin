Viacoin Core v30.2.0 Release Notes
=================================

Viacoin Core version v30.2.0 is now available from:

  <https://github.com/viacoin/viacoin/releases>

This is the first release of Viacoin Core based on Bitcoin Core 30.x,
bringing modern node features, performance improvements, and security fixes
to the Viacoin network.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/viacoin/viacoin/issues>

How to Upgrade
==============

If you are running an older Viacoin Core version, shut it down. Wait until it
has completely shut down (which might take a few minutes), then run the
installer (on Windows) or just copy over `/Applications/Viacoin-Qt` (on macOS)
or `viacoind`/`viacoin-qt` (on Linux).

**Important:** This is a major version upgrade from 0.16. Back up your
wallet and datadir first, do not load the old `wallet.dat` directly in 30.x,
and prepare the existing datadir before the first 30.x start. See the
[migration guide](/doc/viacoin-migration.md).

Compatibility
==============

Viacoin Core is supported and tested on operating systems using the
Linux Kernel 3.17+, macOS 13+, and Windows 10+. Viacoin
Core should also work on most other Unix-like systems but is not as
frequently tested on them. It is not recommended to use Viacoin Core on
unsupported systems.

Notable Changes
===============

assumeUTXO Fast-Sync
--------------------

Viacoin Core 30.x supports assumeUTXO, allowing new nodes to bootstrap from
a UTXO snapshot at a trusted block height instead of validating all 13.6M+
blocks from genesis. A snapshot at height 13,644,731 is hardcoded in the
source code. See [viacoin-assumeutxo.md](/doc/viacoin-assumeutxo.md) for
platform-specific instructions.

AuxPow Memory Optimization (Bug C)
-----------------------------------

The `auxpow` shared_ptr has been removed from `CBlockIndex`, saving
approximately 3.5 GB of memory for Viacoin's 13.6M blocks. AuxPow data is
now read from disk on demand via `ReadBlockHeaderFromDisk()` when needed for
P2P, RPC, and REST serialization. This is a transparent change -- no user
action required.

P2P Headers Presync Skip (Bug B)
---------------------------------

Viacoin's auxpow chains now skip the low-work headers presync phase, which
was causing IBD stalls. The presync optimization is designed for SHA-256d
chains and is not applicable to auxpow/merged-mining chains.

Scrypt SSE2/AVX2 Acceleration
------------------------------

Viacoin Core 30.x includes SSE2- and AVX2-optimized scrypt implementations
for faster proof-of-work verification. SSE2 is always enabled on x86-64.
AVX2 is auto-detected at build time. No configuration needed.

-skipcheckpowatload
--------------------

New option `-skipcheckpowatload` (default: true) skips scrypt PoW
verification when loading the block index from disk. This dramatically
speeds up node startup on trusted local data. Re-enable with
`-skipcheckpowatload=0` if verifying data from untrusted sources.

32x IBD Download Parameters
----------------------------

Block download concurrency has been increased to 32x (from Bitcoin's
default) to match Viacoin's 24-second block time. Parallel header sync is
also enabled, significantly reducing initial block download time.

Wallet Migration from 0.16
---------------------------

The BDB wallet format from Viacoin Core 0.16 is not compatible with 30.x.
Use `dumpwallet` on the old node and `importwallet` on the new node. See
the [migration guide](/doc/viacoin-migration.md) for details.

ViacoinTranslator
-----------------

A `QTranslator` subclass performs runtime string replacement of
"Bitcoin" → "Viacoin" in all Qt translatable strings. This keeps the `.ui`
files and locale sources identical to upstream Bitcoin Core, reducing merge
conflict surface.

Binary Names
------------

All binaries use the `viacoin-` prefix:

| Binary | Description |
|--------|-------------|
| `viacoind` | Node daemon |
| `viacoin-qt` | GUI wallet |
| `viacoin-cli` | RPC client |
| `viacoin-tx` | Transaction utility |
| `viacoin-wallet` | Wallet tool |
| `viacoin-util` | General utility |

Fee Defaults
------------

Viacoin 30.x uses Viacoin-specific fee defaults, including higher relay and
wallet fallback thresholds due to Viacoin's smaller block size:

| Setting | Viacoin | Bitcoin |
|---------|---------|---------|
| Incremental relay fee | 0.00001 VIA/kvB | 0.00001 BTC/kvB |
| Minimum relay fee | 0.001 VIA/kvB | 0.000001 BTC/kvB |
| Fallback fee | 0.002 VIA/kvB | 0 BTC/kvB |
| Discard fee | 0.001 VIA/kvB | 0.0001 BTC/kvB |
| Dust relay fee | 0.003 VIA/kvB | 0.00003 BTC/kvB |

Consensus Parameters
--------------------

Viacoin-specific consensus parameters carried forward from legacy:

- Block interval: 24 seconds
- Block weight limit: 240,000 (vs Bitcoin's 4,000,000)
- Max money: 23,000,000 VIA
- Mainnet subsidy halving interval: 657,000 blocks
- Chain ID: 0x56 (nVersion auxpow chain ID bits)
- COINBASE_MATURITY: 3,600 blocks
- PoW algorithm: scrypt (with SSE2/AVX2 acceleration)

Build System
------------

- GUI is OFF by default -- use `-DBUILD_GUI=ON` to build `viacoin-qt`
- Scrypt SSE2/AVX2 is auto-detected at configure time
- Self-hosted CI runner support (GTX1080 label)

Low-Level Changes
-----------------

### P2P

- Skip presync for auxpow chains (avoids IBD stalls)
- AuxPow blocks: `ReadBlockHeaderFromDisk()` used instead of
  `GetBlockHeader()` for P2P header serialization
- BIP34 activation height set to Viacoin's historical value
- Protocol version set to Viacoin's network version

### RPC

- `getblockheader`: returns full header with auxpow via disk read
- `dumptxoutset`: now available for assumeUTXO snapshot generation
- `loadtxoutset`: now available for assumeUTXO snapshot loading

### REST

- `/rest/headers`: returns full headers with auxpow via disk read

### Block Storage

- New `BlockManager::ReadBlockHeaderFromDisk()` for reading just the
  block header (with auxpow) from blk*.dat, much cheaper than reading
  the full block
- PoW check skipped for auxpow blocks in `LoadBlockIndexGuts` (auxpow
  data not available in CBlockIndex)
- `CDiskBlockIndex` no longer serializes auxpow to the block index
  database (backward-compatible: old entries with trailing auxpow bytes
  are ignored by the deserializer)

Credits
=======

Thanks to everyone who directly contributed to this Viacoin Core release:

- romanornr

As well as the Bitcoin Core developers whose work this release is built upon.
See the Bitcoin Core 30.x release notes for the full list of upstream
contributors.
