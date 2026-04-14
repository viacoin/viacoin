# Upgrading from Viacoin Core 0.16 to Viacoin Core 30.x

This guide covers upgrading from Viacoin Core 0.16 (or earlier) to Viacoin Core 30.x.
This is a major version jump — read carefully before proceeding.

## Before You Start

- **Back up your wallet** — Run `viacoin-cli dumpwallet backup.txt` on your old node
  before doing anything else. Store this file somewhere safe.
- **Back up your `wallet.dat`** — Copy it to a safe location. Do NOT just move it;
  keep the original intact on the old node until migration is confirmed.
- **Keep your old 0.16 node available** — Do not uninstall it until you have
  verified your wallet on 30.x.

## Wallet Migration

**You CANNOT copy `wallet.dat` from 0.16 into 30.x.** The BDB wallet format
changed across these major versions. Attempting to load an old `wallet.dat` will
fail and may corrupt the file.

### Method 1: Dump and Import (Recommended)

On the **old 0.16 node**:

```bash
viacoin-cli dumpwallet /path/to/safe/location/viacoin-dump.txt
```

This creates a text file containing all private keys, watch-only addresses,
and transaction metadata.

On the **new 30.x node**:

```bash
# Create a new wallet first
viacoin-cli createwallet "viacoin-wallet"

# Import the dump
viacoin-cli importwallet /path/to/safe/location/viacoin-dump.txt
```

After import, let the node rescan. This may take a while depending on how
many transactions are associated with your keys.

### Method 2: Restore from Seed Words

If you have your HD seed (12 or 24 words written down when you first created
the wallet):

```bash
viacoin-cli createwallet "viacoin-wallet"
# Then use the GUI: File -> Restore wallet from seed
# Or use the RPC:
viacoin-cli restorewallet "viacoin-wallet" "word1 word2 word3 ... word24"
```

This is the cleanest approach and captures all derived addresses.

### Method 3: Import Individual Private Keys

If you only have a few keys:

```bash
viacoin-cli createwallet "viacoin-wallet"
viacoin-cli importprivkey "VPRIVATEKEY..." "label" false
```

Set the last argument to `true` to trigger a rescan. This method is tedious
for many keys and does not capture labels or metadata from the old wallet.

## Blockchain Data Migration

You do not need to sync from scratch. You can reuse block data from your
0.16 node.

### Using Donor Block Data

1. Install and set up the 30.x binary.
2. Create a new data directory (do NOT reuse the 0.16 datadir directly):

   ```bash
   mkdir -p ~/.viacoin-30x
   ```

3. Copy only the block files from your 0.16 datadir:

   ```bash
   cp ~/.viacoin/blocks/blk*.dat ~/.viacoin-30x/blocks/
   cp ~/.viacoin/blocks/rev*.dat ~/.viacoin-30x/blocks/
   ```

4. Start 30.x with `-reindex` to rebuild the chainstate and block index
   from the block files:

   ```bash
   viacoind -datadir=~/.viacoin-30x -reindex
   ```

   The reindex reads the block files and rebuilds all indexes from scratch.
   With Viacoin's PoW skip at load (default enabled), this should complete
   in reasonable time.

### What NOT to Copy

| File/Directory | Reason |
|----------------|--------|
| `wallet.dat` | Format incompatible — use dumpwallet/importwallet |
| `chainstate/` | Format changed — 30.x will rebuild via -reindex |
| `blocks/index/` | Format changed — 30.x will rebuild via -reindex |
| `peers.dat` | Format may differ — 30.x will discover peers |
| `database/` | BDB environment from old wallet — not needed |
| `fee_estimates.dat` | Format changed — 30.x will re-estimate |

### Full Network Sync (Alternative)

If you prefer a clean start or don't have access to old block data:

```bash
viacoind -datadir=~/.viacoin-30x
```

The 30.x binary has optimized IBD parameters (32x block download window,
parallel header sync) which should speed up the initial sync compared to
older versions.

## Configuration Changes

Your old `viacoin.conf` will mostly work, but note these differences:

- The 30.x binary reads `viacoin.conf` (not `bitcoin.conf`)
- Some RPC commands have changed — check `viacoin-cli help` for the current list
- Fee defaults are different (see below)
- The `-skipcheckpowatload` option (default: true) skips PoW verification at
  startup for faster load times on trusted local data

## Fee Defaults

Viacoin 30.x uses higher fee defaults than Bitcoin Core due to Viacoin's
smaller block size:

| Setting | Viacoin | Bitcoin |
|---------|---------|---------|
| Incremental relay fee | 0.001 VIAC/kB | 0.00001 BTC/kB |
| Minimum relay fee | 0.001 VIAC/kB | 0.000001 BTC/kB |
| Fallback fee | 0.002 VIAC/kB | 0 BTC/kB |
| Discard fee | 0.001 VIAC/kB | 0.0001 BTC/kB |
| Dust relay fee | 0.003 VIAC/kB | 0.00003 BTC/kB |

If your old `viacoin.conf` sets explicit fee values, review them — some
may now be below the new minimums.

## SSE2 Scrypt Acceleration

Viacoin 30.x includes SSE2-optimized scrypt for faster PoW verification.
On x86-64 systems (virtually all modern PCs), SSE2 is always enabled.
On 32-bit x86, it is detected at runtime via CPUID.

You can verify SSE2 is active by checking the debug log at startup:

```
scrypt: using scrypt-sse2 as built-in
```

## Verifying Your Migration

After starting 30.x, verify:

1. **Block height matches** — Compare `viacoin-cli getblockcount` with your
   old node or a block explorer.
2. **Wallet balance matches** — Check `viacoin-cli getbalance` against your
   old wallet.
3. **Transaction history** — Review `viacoin-cli listtransactions` for
   completeness.
4. **Addresses** — Verify that receiving addresses you've shared are still
   accessible via `viacoin-cli listaddressgroupings` or the GUI.

## Troubleshooting

### "Wallet wallet.dat is not compatible with this version"

You tried to load a 0.16 wallet.dat directly. Use the dumpwallet/importwallet
method described above.

### "Error loading block index"

You may have copied chainstate/ or blocks/index/ from the old node. Remove
those directories and use `-reindex` instead.

### Missing transactions after importwallet

The rescan may still be in progress. Check `viacoin-cli getblockchaininfo`
and wait for `initialblockdownload` to be `false`. You can force a rescan
with `viacoin-cli rescanblockchain`.

### Slow startup / reindex

The `-reindex` process recomputes scrypt hashes for every block. With the
default `-skipcheckpowatload=true`, PoW verification is skipped at startup,
but the full reindex still takes time. On a modern machine, expect
30-90 minutes for Viacoin's ~5.6M blocks.
