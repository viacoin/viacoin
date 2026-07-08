Viacoin Core Build Notes (Unix)
===============================

These are Viacoin-specific build instructions for Unix-like systems.
For the full upstream build reference, see [build-unix.md](/doc/build-unix.md).

Quick Start
-----------

```bash
git clone https://github.com/romanornr/viacoin.git
cd viacoin
cmake -B build -DBUILD_GUI=ON
cmake --build build -j$(nproc)
```

This produces the following binaries in `build/bin/`:

| Binary | Description |
|--------|-------------|
| `viacoind` | Node daemon |
| `viacoin-qt` | GUI wallet (requires `-DBUILD_GUI=ON`) |
| `viacoin-cli` | RPC client |
| `viacoin-tx` | Transaction utility |
| `viacoin-wallet` | Wallet tool |
| `viacoin-util` | General utility |

Viacoin-Specific Build Options
-------------------------------

### GUI (off by default)

The GUI is not built by default. You must explicitly enable it:

```bash
cmake -B build -DBUILD_GUI=ON
```

This requires Qt 6 development packages (see [GUI dependencies](#gui-dependencies)).

### Scrypt Hardware Acceleration

Viacoin uses scrypt for proof-of-work (unlike Bitcoin's SHA-256d). Optimized
implementations are auto-detected at configure time:

| Implementation | Auto-detected | Notes |
|----------------|---------------|-------|
| SSE2 | Yes (x86-64 always) | Always enabled on 64-bit x86 |
| AVX2 | Yes (CPUID check) | ~2x faster scrypt; auto-detected at build time |
| ARM SHA | Yes | For ARM-based systems |

No special CMake flags are needed -- just build on a machine that supports
these instruction sets and they will be enabled automatically.

You can verify which implementation is active in the debug log at startup:

```
scrypt: using scrypt-sse2 as built-in
```

### PoW Verification at Load

Viacoin adds a `-skipcheckpowatload` option (default: **true**) that skips
scrypt PoW verification when loading the block index from disk. This
dramatically speeds up startup for nodes with trusted local data.

For a fresh IBD or untrusted data, you can re-enable it:

```bash
viacoind -skipcheckpowatload=0
```

This option does not exist in Bitcoin Core.

Dependencies
------------

### Build requirements

```bash
# Ubuntu / Debian
sudo apt-get install build-essential cmake pkgconf python3

# Fedora
sudo dnf install gcc-c++ cmake make python3

# Arch Linux
sudo pacman -S cmake gcc make python
```

### Core dependencies

```bash
# Ubuntu / Debian
sudo apt-get install libevent-dev libboost-dev libsqlite3-dev

# Fedora
sudo dnf install libevent-devel boost-devel sqlite-devel

# Arch Linux
sudo pacman -S libevent boost sqlite
```

SQLite is required for the wallet. To build without wallet support:

```bash
cmake -B build -DENABLE_WALLET=OFF
```

### GUI dependencies

Required for `viacoin-qt` (only when `-DBUILD_GUI=ON`):

```bash
# Ubuntu / Debian
sudo apt-get install qt6-base-dev qt6-tools-dev qt6-l9n-tools qt6-tools-dev-tools libgl-dev libqrencode-dev

# Fedora
sudo dnf install qt6-qtbase-devel qt6-qttools-devel qrencode-devel

# Arch Linux
sudo pacman -S qt6-base qt6-tools qt6-translations qrencode
```

For Wayland support on modern desktops:

```bash
# Ubuntu / Debian
sudo apt install qt6-wayland

# Fedora
sudo dnf install qt6-qtwayland

# Arch Linux
sudo pacman -S qt6-wayland
```

### Optional dependencies

| Feature | Flag | Ubuntu/Debian | Fedora | Arch |
|---------|------|---------------|--------|------|
| ZMQ notifications | `-DWITH_ZMQ=ON` | `libzmq3-dev` | `zeromq-devel` | `zeromq` |
| IPC (multiprocess) | `-DENABLE_IPC=OFF` to disable | `libcapnp-dev capnproto` | `capnproto capnproto-devel` | `capnproto` |
| USDT tracing | `-DWITH_USDT=ON` | `systemtap-sdt-dev` | `systemtap-sdt-devel` | `systemtap` |

Common Build Commands
---------------------

### Headless node (no GUI)

```bash
cmake -B build
cmake --build build -j$(nproc)
./build/bin/viacoind
```

### Full build with GUI

```bash
cmake -B build -DBUILD_GUI=ON
cmake --build build -j$(nproc)
./build/bin/viacoin-qt
```

### Run the test suite

```bash
cmake --build build -j$(nproc)
ctest --test-dir build
```

### Low-memory systems

If the compiler runs out of memory, reduce parallelism or use clang:

```bash
# Fewer parallel jobs
cmake --build build -j2

# Or use clang (less memory hungry)
cmake -B build -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
cmake --build build -j$(nproc)

# Or skip debug info
cmake -B build -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O2 -g0"
cmake --build build -j$(nproc)
```

### See all CMake options

```bash
cmake -B build -LH
```

Complete Example: Arch Linux
-----------------------------

```bash
sudo pacman -S cmake boost gcc git libevent make python sqlite qt6-base qt6-tools qrencode
git clone https://github.com/romanornr/viacoin.git
cd viacoin
cmake -B build -DBUILD_GUI=ON
cmake --build build -j$(nproc)
./build/bin/viacoin-qt
```

Complete Example: Ubuntu 24.04
-------------------------------

```bash
sudo apt-get install build-essential cmake pkgconf python3 \
  libevent-dev libboost-dev libsqlite3-dev \
  qt6-base-dev qt6-tools-dev qt6-l9n-tools qt6-tools-dev-tools \
  libgl-dev libqrencode-dev qt6-wayland
git clone https://github.com/romanornr/viacoin.git
cd viacoin
cmake -B build -DBUILD_GUI=ON
cmake --build build -j$(nproc)
./build/bin/viacoin-qt
```

For the full list of dependencies and advanced build options, see
[dependencies.md](dependencies.md) and [build-unix.md](build-unix.md).
