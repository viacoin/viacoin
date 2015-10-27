Viacoin Core version 0.10.8 is now available from:

  https://github.com/viacoin/viacoin/releases

This is a new minor version release, bringing security fixes and translation 
updates. It is recommended to upgrade to this version as soon as possible.

Please report bugs using the issue tracker at github:

  https://github.com/viacoin/viacoin/issues

Upgrading and downgrading
=========================

How to Upgrade
--------------

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then run the
installer (on Windows) or just copy over /Applications/Viacoin-Qt (on Mac) or
viacoind/viacoin-qt (on Linux).


Notable changes
===============

Hard fork introduces subsidy halving every 657000 blocks starting from block 1971001


0.10.8 Change log
=================

Detailed release notes follow. This overview includes changes that affect external
behavior, not code moves, refactors or string updates.

Wallet:
- `824c011` fix boost::get usage with boost 1.58

Miscellaneous:
- `da65606` Avoid crash on start in TestBlockValidity with gen=1.
- `424ae66` don't imbue boost::filesystem::path with locale "C" on windows (fixes #6078)
- `220ebb5` Hard fork introduces subsidy halving every 657000 blocks starting from block 1971001
- #6186 `e4a7d51` Fix two problems in CSubnet parsing
- #6153 `ebd7d8d` Parameter interaction: disable upnp if -proxy set
- #6203 `ecc96f5` Remove P2SH coinbase flag, no longer interesting
- #6226 `181771b` json: fail read_string if string contains trailing garbage
- #6244 `09334e0` configure: Detect (and reject) LibreSSL
- #6276 `0fd8464` Fix getbalance * 0
- #6274 `be64204` Add option `-alerts` to opt out of alert system
- #6319 `3f55638` doc: update mailing list address
- #6438 `7e66e9c` openssl: avoid config file load/race
- #6439 `255eced` Updated URL location of netinstall for Debian
- #6412 `0739e6e` Test whether created sockets are select()able
- #6694 `f696ea1` [QT] fix thin space word wrap line brake issue
- #6704 `743cc9e` Backport bugfixes to 0.10
- #6769 `1cea6b0` Test LowS in standardness, removes nuisance malleability vector.
- #6789 `093d7b5` Update miniupnpc to 1.9.20151008
- #6795 `f2778e0` net: Disable upnp by default
- #6797 `91ef4d9` Do not store more than 200 timedata samples
- #6793 `842c48d` Bump minrelaytxfee default

Credits
=======

Thanks to everyone who directly contributed to this release:

- Adam Weiss
- Alex Morcos
- Casey Rodarmor
- Cory Fields
- fanquake
- Gregory Maxwell
- Jonas Schnelli
- J Ross Nicoll
- Luke Dashjr
- Pavel Vasin
- Pieter Wuille
- randy-waterhouse
- ฿tcDrak
- Tom Harding
- Veres Lajos
- Wladimir J. van der Laan

And all those who contributed additional code review and/or security research:

- timothy on IRC for reporting the issue
- Vulnerability in miniupnp discovered by Aleksandar Nikolic of Cisco Talos
