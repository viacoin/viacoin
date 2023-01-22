Viacoin Core integration/staging tree [![Build Status](https://travis-ci.org/viacoin/viacoin.svg?branch=master)](https://travis-ci.org/viacoin/viacoin)
=====================================

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-FFD43B?style=for-the-badge&logo=python&logoColor=blue)



What is Viacoin? 
----------------

Viacoin is an experimental digital currency that enables instant payments to
anyone, anywhere in the world. Viacoin uses peer-to-peer technology to operate
with no central authority: managing transactions and issuing money are carried
out collectively by the network. Viacoin Core is the name of open source
software which enables the use of this currency.

Viacoin is an open-source experimental cryptocurrency that enables instant payments to
anyone, anywhere in the world. Viacoin uses peer-to-peer technology to operate
with no central authority: managing transactions and issuing money are carried
out collectively by the network

Viacoin supports embedded consensus with an extended OP_RETURN of 120 bytes and provides 25x faster transactions than Bitcoin. 

Furthermore, the inflation rate of Viacoin is low, given its low block reward, which is offset by miners’ incentives to mine Viacoin through Merged mining (AuxPoW). Notably, F2Pool, one of the largest mining pools, is currently mining Viacoin at a high hash rate and the mining reward halving occurs every 6 months with a total supply of 23,176,392.41459 coins.

Additionally, its mining difficulty is adjusted with a Kimotos Gravity Well algorithm, and its mining reward halving occurs every 6 months. There is a total supply of 23,176,392.41459 coins, and F2Pool, one of the largest mining pools, is currently mining Viacoin at a high hash rate. The Lightning Network will also be supported. Versionbits to allow up to 29 simultaneous Soft Fork changes to be implemented.

https://viacoin.org

https://coinmarketcap.com/currencies/viacoin/

***

### Technical documentation

The wiki is created due to the widespread lack of technical knowledge among investors and programmers prompted the building of this wiki. This wiki's purpose is to demonstrate its genuine craftsmanship and serve as our own documentation.

https://github.com/viacoin/viacoin/wiki

***

For more information, as well as an immediately useable, binary version of
the Viacoin Core software, see https://viacoin.org

![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![MacOS](https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=apple&logoColor=white)




***

License
-------

Viacoin Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/viacoin/viacoin/tags) are created
regularly to indicate new official, stable release versions of Viacoin Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).

The developer [mailing list](https://lists.linuxfoundation.org/mailman/listinfo/viacoin-dev)
should be used to discuss complicated or controversial changes before working
on a patch set.

Developer IRC can be found on Freenode at #viacoin-core-dev.

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](src/test/README.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`. Further details on running
and extending unit tests can be found in [/src/test/README.md](/src/test/README.md).

There are also [regression and integration tests](/test), written
in Python, that are run automatically on the build server.
These tests can be run (if the [test dependencies](/test) are installed) with: `test/functional/test_runner.py`

The Travis CI system makes sure that every pull request is built for Windows, Linux, and OS X, and that unit/sanity tests are run automatically.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.

Translations
------------

Changes to translations as well as new translations can be submitted to
[Viacoin Core's Transifex page](https://www.transifex.com/projects/p/viacoin/).

Translations are periodically pulled from Transifex and merged into the git repository. See the
[translation process](doc/translation_process.md) for details on how this works.

**Important**: We do not accept translation changes as GitHub pull requests because the next
pull from Transifex would automatically overwrite them again.

Translators should also subscribe to the [mailing list](https://groups.google.com/forum/#!forum/viacoin-translators).
