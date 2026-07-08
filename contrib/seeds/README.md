# Viacoin Seed Nodes

Utility to generate the seeds list compiled into the Viacoin client
(see [src/chainparamsseeds.h](/src/chainparamsseeds.h)).

Unlike Bitcoin Core which uses DNS crawlers to produce `seeds_main.txt`,
Viacoin's network is small enough that seed nodes are maintained manually in
`nodes_main.txt`. These are known stable Viacoin nodes with good uptime that
serve the Viacoin P2P network on port 5223.

## Adding or Removing Seed Nodes

Edit `nodes_main.txt` directly. Each line should be in the format:

```
<ip>:<port>
[<ipv6>]:<port>
<onion>.onion:<port>
```

Then regenerate the compiled seeds header:

```bash
cd contrib/seeds
python3 generate-seeds.py . > ../../src/chainparamsseeds.h
```

## Guidelines for Seed Nodes

- Must have service bit 1 (NODE_NETWORK) -- serves full blocks
- Must be on port 5223 (Viacoin mainnet default)
- Should have reliable uptime (>50% over 30 days)
- Should run a recent Viacoin Core version
- Do not include personal/home IP addresses without the operator's consent
- Do not include IP addresses that are known to be unstable or abusive

## DNS Seeders

Viacoin also has DNS seeders that provide dynamic peer discovery at runtime.
These are configured in `src/kernel/chainparams.cpp`:

- `seed.viacoin.net`
- `mainnet.viacoin.net`

The fixed seeds in `nodes_main.txt` serve as a fallback when DNS seeders
are unreachable.

## Regenerating the Seeds Header

From the repository root:

```bash
python3 contrib/seeds/generate-seeds.py contrib/seeds > src/chainparamsseeds.h
```

Then rebuild the binary. Verify the new seeds are loaded by checking the
debug log at startup for "adding fixed seed" messages.
