// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "uritests.h"

#include "guiutil.h"
#include "walletmodel.h"

#include <QUrl>

void URITests::uriTests()
{
    SendCoinsRecipient rv;
    QUrl uri;
    uri.setUrl(QString("viacoin:Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn?req-dontexist="));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("viacoin:Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn?dontexist="));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("viacoin:Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn?label=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn"));
    QVERIFY(rv.label == QString("Wikipedia Example Address"));
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("viacoin:Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn?amount=0.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100000);

    uri.setUrl(QString("viacoin:Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn?amount=1.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100100000);

    uri.setUrl(QString("viacoin:Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn?amount=100&label=Wikipedia Example"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn"));
    QVERIFY(rv.amount == 10000000000LL);
    QVERIFY(rv.label == QString("Wikipedia Example"));

    uri.setUrl(QString("viacoin:Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn?message=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn"));
    QVERIFY(rv.label == QString());

    QVERIFY(GUIUtil::parseBitcoinURI("viacoin://Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn?message=Wikipedia Example Address", &rv));
    QVERIFY(rv.address == QString("Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn"));
    QVERIFY(rv.label == QString());

    uri.setUrl(QString("viacoin:Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn?req-message=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("viacoin:Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn?amount=1,000&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("viacoin:Vg5iRXkaipLKSYBDJjkV5qa1CE9b1djeHn?amount=1,000.0&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));
}
