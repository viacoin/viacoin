Quickstart Build Instructions
=============================

Ubuntu/Debian require the following dependencies:

    sudo add-apt-repository ppa:bitcoin/bitcoin
    sudo apt-get update
    sudo apt-get install build-essential libtool autotools-dev autoconf pkg-config libssl-dev libboost-all-dev libdb4.8-dev libdb4.8++-dev libminiupnpc-dev 

If you want to build the GUI you also need: 

    sudo apt-get install libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev protobuf-compiler libqrencode-dev
    
viacoind
--------

To build just `viacoind` (which will include `viacoin-cli` and `viacoin-tx`) but without the GUI

    ./autogen.sh
    ./configure --with-gui=no --enable-tests=no
    make 
    sudo make install
    
Qt GUI
------

To build with the GUI

    ./autogen.sh
    ./configure --enable-tests=no
    make 
    sudo make install
