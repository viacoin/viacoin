# Viacoin assumeUTXO Fast-Sync

assumeUTXO allows a new Viacoin node to bootstrap from a UTXO snapshot at a
trusted block height instead of validating all 13.6M+ blocks from genesis.
The snapshot is verified against a hash hardcoded in `src/kernel/chainparams.cpp`,
so even snapshots obtained from untrusted sources are safe to use.

For the design rationale, see [design/assumeutxo.md](/doc/design/assumeutxo.md).

## Current snapshot

| Field | Value |
|-------|-------|
| Height | 13,644,731 |
| Block hash | `4fbc17c6edf747be9bb16ba3c99dfc3925ed1d891098398d082adcdadcb9afcd` |
| UTXO set hash | `92015bf35909d0c19665d94077f363fd506970c4ec35a53f80f9527603d1310c` |
| Chain TX count | 14,345,241 |
| UTXO coins | 3,757,889 |
| Snapshot file size | ~223 MB |

## Quick start

1. Download the snapshot file and place it in your Viacoin data directory
2. Load it before starting normal sync

```bash
viacoin-cli -rpcclienttimeout=0 loadtxoutset /path/to/utxo_snapshot.dat
```

After loading, the node will have a usable chainstate at the snapshot height
and sync the remaining blocks from peers. A background validation from genesis
runs concurrently and will eventually catch up to the snapshot block.

Monitor both chainstates with:

```bash
viacoin-cli getchainstates
```

## Platform-specific instructions

### Linux

```bash
# Download the snapshot (replace URL with actual hosting location)
wget -O ~/.viacoin/utxo_snapshot.dat <SNAPSHOT_URL>

# If viacoind is already running:
viacoin-cli -rpcclienttimeout=0 loadtxoutset ~/.viacoin/utxo_snapshot.dat

# If viacoind is not running yet, start it first:
viacoind -daemon
viacoin-cli -rpcclienttimeout=0 loadtxoutset ~/.viacoin/utxo_snapshot.dat
```

Alternatively, using the Qt GUI:

```bash
viacoin-qt &
# Then from a terminal:
viacoin-cli -rpcclienttimeout=0 loadtxoutset ~/.viacoin/utxo_snapshot.dat
```

After `loadtxoutset` completes, the snapshot file can be deleted to save disk
space:

```bash
rm ~/.viacoin/utxo_snapshot.dat
```

### macOS

```bash
# Download the snapshot
curl -L -o ~/Library/Application\ Support/Viacoin/utxo_snapshot.dat <SNAPSHOT_URL>

# Load it
viacoin-cli -rpcclienttimeout=0 loadtxoutset ~/Library/Application\ Support/Viacoin/utxo_snapshot.dat
```

The default Viacoin data directory on macOS is:
`~/Library/Application Support/Viacoin/`

### Windows

```powershell
# Download the snapshot using PowerShell
Invoke-WebRequest -Uri <SNAPSHOT_URL> -OutFile "$env:APPDATA\Viacoin\utxo_snapshot.dat"

# Load it
viacoin-cli -rpcclienttimeout=0 loadtxoutset "%APPDATA%\Viacoin\utxo_snapshot.dat"
```

The default Viacoin data directory on Windows is:
`%APPDATA%\Viacoin\`

## How it works

1. `loadtxoutset` reads the snapshot file and deserializes the UTXO set into a
   new `chainstate_snapshot/` directory inside the data directory.

2. The snapshot's `hash_serialized` is checked against the value hardcoded in
   `src/kernel/chainparams.cpp`. If it doesn't match, the load is rejected.
   This means even a tampered download cannot compromise your node.

3. The snapshot chainstate becomes the active chainstate. The node syncs
   forward from the snapshot height to the network tip using normal block
   download.

4. A background chainstate validates blocks from genesis to the snapshot
   height. This takes several hours but happens in the background -- your
   node is usable immediately.

5. Once background validation reaches the snapshot block and confirms the
   UTXO set matches, the two chainstates are merged and the background
   validation is cleaned up.

## Pruning

A pruned node can load a snapshot. The snapshot file can be deleted as soon as
`loadtxoutset` finishes.

During background validation there will be two chainstate directories on disk,
each potentially multiple gigabytes. The minimum `-prune` setting for a snapshot
node is 1100 MiB (overrides the normal 550 MiB minimum).

## Generating a new snapshot

If you have a fully synced Viacoin node, you can generate a fresh snapshot:

```bash
viacoin-cli -rpcclienttimeout=0 dumptxoutset utxo_snapshot.dat latest
```

This produces output like:

```json
{
  "coins_written": 3757889,
  "base_hash": "4fbc17c6edf747be9bb16ba3c99dfc3925ed1d891098398d082adcdadcb9afcd",
  "base_height": 13644731,
  "path": "/home/user/.viacoin/utxo_snapshot.dat",
  "txoutset_hash": "92015bf35909d0c19665d94077f363fd506970c4ec35a53f80f9527603d1310c",
  "nchaintx": 14345241
}
```

To use the new snapshot on other nodes, its data must be added to
`src/kernel/chainparams.cpp` in the `m_assumeutxo_data` array and the binary
must be recompiled. Without a matching entry in chainparams, `loadtxoutset`
will reject the snapshot.

To verify an existing hardcoded snapshot hash, regenerate it using the
`rollback` type:

```bash
viacoin-cli -rpcclienttimeout=0 dumptxoutset utxo_snapshot.dat rollback
```

## Troubleshooting

**"assumeutxo snapshot not found"** -- The node didn't find a snapshot
chainstate on disk. This is normal for a fresh node. Either use `loadtxoutset`
to load one, or do a full IBD from genesis.

**"Failed to read block header from disk"** -- This can happen if the node
was compiled before the Bug C fix (CBlockIndex auxpow removal). Ensure you
are running a build from commit `f4f3418bda` or later.

**Background validation is slow** -- Normal for Viacoin. The 13.6M blocks
require scrypt PoW verification for each header. Background validation may
take several hours. Your node remains fully usable during this time.

**Disk space concerns** -- During background validation, two chainstates
exist on disk. Ensure you have at least 30 GB free. The extra chainstate
is cleaned up automatically after validation completes.
