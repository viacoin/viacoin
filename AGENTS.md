# Viacoin 30.x port harness map

This repo is the active Viacoin forward-port target from Bitcoin Core 0.30.

Use this file as a short routing layer only.

Read first
1. `/home/romano/github/viacoin-repos/_agent/viacoin/START_HERE.md`
2. `/home/romano/github/viacoin-repos/_agent/viacoin/SOURCE_OF_TRUTH.md`
3. `/home/romano/github/viacoin-repos/_agent/viacoin/FIXTURE_POLICY.md`
4. `/home/romano/github/viacoin-repos/_agent/viacoin/WORKFLOWS.md`

Authority hierarchy
- Historical Viacoin canon:
  - `/home/romano/github/viacoin-repos/viacoin-master-legacy`
  - for intent/provenance, inspect historical commits there especially by `romanornr` and `reorder`
- Pure Bitcoin Core 0.30 reference:
  - `/home/romano/github/viacoin-repos/bitcoin-core-30.x`
- Secondary comparison sources only:
  - `origin/24-dev` with extra attention to commits by `vswarte` and `romanornr`
  - `/home/romano/github/groestlcoin` with extra attention to `30.2.0` work by `gruve-p`
- Live runtime behavior for address/WIF/message/RPC questions:
  - local Viacoin 0.16 RPC on `127.0.0.1:5222`
- `_agent/` docs are derived harness notes only, not protocol authority.

Rules for changes in this repo
- Do not overfit tests by replacing fixed deterministic fixtures with self-generated roundtrip checks unless there is no canonical Viacoin-native fixture source available.
- If a test fails because it expects Bitcoin-native strings or prefixes while runtime behavior is already Viacoin-correct, rewrite the fixture, not production code.
- If runtime behavior differs from legacy Viacoin or live-node-confirmed behavior, treat it as a production bug first.
- Keep facts clearly separate from hypotheses during investigations.
- Prefer exact file paths, commit hashes, test names, and RPC outputs over long prose summaries.

Common routing
- Fixture / WIF / address / message-signing issue:
  - check live RPC first
  - then `viacoin-master-legacy`
  - then `_agent/viacoin/FIXTURE_REGISTRY.md`
  - then `_agent/viacoin/references/qdrant-collections.md` for local semantic discovery
- Consensus / protocol / chain identity issue:
  - check `viacoin-master-legacy`
  - compare against `bitcoin-core-30.x`
  - use `origin/24-dev` and `groestlcoin` only as secondary triangulation
- CI failure triage:
  - use `_agent/viacoin/WORKFLOWS.md`
  - expect GitHub CI runs to take hours; prefer fresh completed runs over stale partial symptom maps

Do not turn this file into a giant manual. Keep it short and routing-heavy.
