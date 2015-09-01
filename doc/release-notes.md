Viacoin Core version 0.10.8 is now available from:

  https://github.com/viacoin/viacoin/releases

This is a new minor version release, bringing minor bug fixes and translation 
updates. It is recommended to upgrade to this version.

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