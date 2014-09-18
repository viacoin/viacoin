SYSTEMD SUPPORT IN VIACOIN
==========================

Packagers can find a .service file in this repo in order to integrate viacoin's 
daemon into systemd based distributions.

viacoind.service file is located in contrib/systemd/ folder.

1. Users
---------------------------------

This .service file assumes bitcoind user and group exist in the system, so packager
should make sure they are created on installation. 

2. Files
---------------------------------

The .service file assumes several paths that might need to be adjusted according
to packager's needs.

Daemon's config file is assumed to be located at /etc/viacoind.conf (you can
use contrib/debian/examples/viacoin.conf as an example). Once installed, users
must edit the file in order to update at least these two 
values: rpcuser and rpcpassword . Failing to do so will make the daemon fail 
to boot. However, the message written to /var/lib/viacoind/debug.log file is
very helpful and no default values should be set:

    YYYY-MM-DD HH:MM:DD Error: To use the "-server" option, you must set a rpcpassword in the configuration file:
    /etc/viacoind.conf
    It is recommended you use the following random password:
    rpcuser=viacoinrpc
    rpcpassword=HdYZ5HGtAF7mx8aTw6uCATtD2maMAK4E12Ysp4YNZQcX
    (you do not need to remember this password)
    The username and password MUST NOT be the same.
    If the file does not exist, create it with owner-readable-only file permissions.
    It is also recommended to set alertnotify so you are notified of problems;
    for example: alertnotify=echo %s | mail -s Viacoin Alert" admin@foo.com

Daemon's data and pid files will be stored in /var/lib/viacoind directory, so it
should be created on installation and make viacoind user/group it's owner.

3. Installing .service file
---------------------------------

Installing this .service file consists on just copying it to /usr/lib/systemd/system
directory, followed by the command "systemctl daemon-reload" in order to update
running systemd configuration.
