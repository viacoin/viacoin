# Upgrading from Viacoin Core 0.16 to Viacoin Core 30.x

This guide covers upgrading from Viacoin Core 0.16 (or earlier) to Viacoin Core 30.x.
This is a major version jump — read carefully before proceeding.

---

## STOP — Back Up Your Wallet First

**Before doing anything else, back up your wallet.dat. Do not continue until
you have a confirmed backup in a safe location.**

If anything goes wrong during migration, your coins are only as safe as your
backup. There is no way to recover a lost or corrupted wallet without a backup.

1. **Copy `wallet.dat` to at least two safe locations** (USB drive, different
   machine, cloud storage — somewhere other than the machine you're migrating).

2. **Also dump your wallet** as a secondary backup:

   ```
   viacoin-cli dumpwallet /path/to/safe/location/viacoin-dump.txt
   ```

3. **Verify your backups** — Confirm the file sizes are reasonable and the
   dump file contains keys (open it in a text editor and look for private keys).

4. **Keep your old 0.16 node running** — Do not uninstall it until you have
   confirmed your wallet balance on 30.x.

---

## Data Directory Locations

Your Viacoin data directory depends on your operating system:

| OS | Path |
|----|------|
| Linux | `~/.viacoin/` |
| macOS | `~/Library/Application Support/Viacoin/` |
| Windows | `%APPDATA%\Viacoin\` |

For most users, this existing default data directory is where Viacoin 0.16
already stores wallets and blocks, and it is also where Viacoin Core 30.x will
look by default. Before the first 30.x start, make a full backup and prepare the
directory as described below. Release/testing operators should use an isolated
`-datadir`; normal users usually do not need a second data directory.

---

## Wallet Migration

**You CANNOT copy `wallet.dat` from 0.16 into 30.x.** The BDB wallet format
changed across these major versions. Attempting to load an old `wallet.dat`
will fail and may corrupt the file.

### Method 1: Dump and Import (Recommended)

On the **old 0.16 node**:

```
viacoin-cli dumpwallet /path/to/safe/location/viacoin-dump.txt
```

This creates a text file containing all private keys, watch-only addresses,
and transaction metadata.

On the **new 30.x node**, after preparing the data directory:

```
# Create a new wallet first
viacoin-cli createwallet "viacoin-wallet"

# Import the dump
viacoin-cli importwallet /path/to/safe/location/viacoin-dump.txt
```

After import, let the node rescan. This may take a while depending on how
many transactions are associated with your keys.

### Method 2: Import Individual Private Keys

If you only have a few keys:

```
viacoin-cli createwallet "viacoin-wallet"
viacoin-cli importprivkey "VPRIVATEKEY..." "label" false
```

Set the last argument to `true` to trigger a rescan. This method is tedious
for many keys and does not capture labels or metadata from the old wallet.
Prefer `dumpwallet`/`importwallet` for a full 0.16 wallet migration.


---

## Blockchain Data Migration

Most users can reuse their existing default datadir, but they must not reuse
the old wallet file, chainstate, or block index directly. The safest public
migration is:

1. Stop Viacoin Core 0.16 and wait until it exits fully.
2. Back up the whole data directory from the table above.
3. Keep `blocks/blk*.dat` and `blocks/rev*.dat`.
4. Move the incompatible old wallet and indexes out of the active datadir:

   | File/Directory | Action |
   |----------------|--------|
   | `wallet.dat` | Move aside after `dumpwallet`; do not load directly in 30.x |
   | `chainstate/` | Move aside or delete after backup; 30.x rebuilds it |
   | `blocks/index/` | Move aside or delete after backup; 30.x rebuilds it |
   | `database/` | Move aside or delete after backup; old BDB environment |
   | `fee_estimates.dat` | Move aside or delete after backup; 30.x recreates it |

5. Start 30.x with `-reindex` once to rebuild the chainstate and block index
   from the existing block files:

   **Linux/macOS:**
   ```bash
   viacoind -reindex
   ```

   **Windows:**
   ```powershell
   viacoind -reindex
   ```

   The reindex reads the existing block files and rebuilds all indexes from
   scratch. With Viacoin's PoW skip at load (default enabled), this should
   complete in reasonable time.

### Using Donor Block Data

If you prefer to create a fresh 30.x datadir instead of preparing the existing
default datadir, you can copy only the old block files from a backed-up 0.16
datadir.

In the examples below, replace `<OLD_DATADIR>` with the 0.16 datadir and
`<NEW_DATADIR>` with the fresh 30.x datadir. Do not use the same path for both.

1. Install the 30.x binary for your platform.
2. Create a new data directory.
3. Copy only the block files:

   **Linux/macOS:**
   ```bash
   mkdir -p "<NEW_DATADIR>/blocks"
   cp "<OLD_DATADIR>/blocks"/blk*.dat "<NEW_DATADIR>/blocks/"
   cp "<OLD_DATADIR>/blocks"/rev*.dat "<NEW_DATADIR>/blocks/"
   ```

   **Windows (PowerShell):**
   ```powershell
   New-Item -ItemType Directory -Force -Path "<NEW_DATADIR>\blocks"
   Copy-Item "<OLD_DATADIR>\blocks\blk*.dat" "<NEW_DATADIR>\blocks\"
   Copy-Item "<OLD_DATADIR>\blocks\rev*.dat" "<NEW_DATADIR>\blocks\"
   ```

4. Start 30.x with `-datadir=<NEW_DATADIR> -reindex` to rebuild the chainstate
   and block index from the copied block files:

   **Linux/macOS:**
   ```bash
   viacoind -datadir=<NEW_DATADIR> -reindex
   ```

   **Windows:**
   ```powershell
   viacoind -datadir=<NEW_DATADIR> -reindex
   ```

### Fast Sync with assumeUTXO (Alternative)

If you don't have access to old block data, you can use an assumeUTXO snapshot
to bootstrap quickly. See [viacoin-assumeutxo.md](/doc/viacoin-assumeutxo.md)
for platform-specific instructions.

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

If you prefer a clean start, back up the old datadir, move it aside, and let
30.x create a fresh default datadir:

```
viacoind
```

The 30.x binary has optimized IBD parameters (32x block download window,
parallel header sync) which should speed up the initial sync compared to
older versions.

---

## Configuration Changes

Review old `viacoin.conf` settings before reusing them with 30.x. Notable
differences:

- The 30.x binary reads `viacoin.conf` (not `bitcoin.conf`)
- Some RPC commands have changed — check `viacoin-cli help` for the current list
- Fee defaults are different (see below)
- The `-skipcheckpowatload` option (default: true) skips PoW verification at
  startup for faster load times on trusted local data

---

## Fee Defaults

Viacoin 30.x uses Viacoin-specific fee defaults, including higher relay and
wallet fallback thresholds due to Viacoin's smaller block size:

| Setting | Viacoin | Bitcoin |
|---------|---------|---------|
| Incremental relay fee | 0.00001 VIA/kvB | 0.00001 BTC/kvB |
| Minimum relay fee | 0.001 VIA/kvB | 0.000001 BTC/kvB |
| Fallback fee | 0.002 VIA/kvB | 0 BTC/kvB |
| Discard fee | 0.001 VIA/kvB | 0.0001 BTC/kvB |
| Dust relay fee | 0.003 VIA/kvB | 0.00003 BTC/kvB |

If your old `viacoin.conf` sets explicit fee values, review them — some
may now be below the new minimums.

---

## SSE2/AVX2 Scrypt Acceleration

Viacoin 30.x includes SSE2- and AVX2-optimized scrypt for faster PoW
verification. On x86-64 systems (virtually all modern PCs), SSE2 is always
enabled. AVX2 is detected at runtime.

You can verify which implementation is active by checking the debug log
at startup:

```
scrypt: using scrypt-sse2 as built-in
```

---

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

---

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
several hours for Viacoin's 13.6M+ blocks.

Consider using assumeUTXO for a faster bootstrap — see
[viacoin-assumeutxo.md](/doc/viacoin-assumeutxo.md).
