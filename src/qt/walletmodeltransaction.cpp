// Copyright (c) 2011-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/walletmodeltransaction.h>

#include <memory>

#include <interfaces/node.h>
#include "policy/policy.h"
//#include "wallet/wallet.h"

WalletModelTransaction::WalletModelTransaction(
    const QList<SendCoinsRecipient> &_recipients)
    : recipients(_recipients), fee() {}

QList<SendCoinsRecipient> WalletModelTransaction::getRecipients() const {
    return recipients;
}

std::unique_ptr<interfaces::PendingWalletTx> &WalletModelTransaction::getWtx() {
    return wtx;
}

unsigned int WalletModelTransaction::getTransactionSize() {
    return wtx ? wtx->get().GetTotalSize() : 0;
}

Amount WalletModelTransaction::getTransactionFee() const {
    return fee;
}

void WalletModelTransaction::setTransactionFee(const Amount newFee) {
    fee = newFee;
}

void WalletModelTransaction::reassignAmounts(int nChangePosRet) {
    const CTransaction *walletTransaction = &wtx->get();
    int i = 0;
    for (SendCoinsRecipient &rcp : recipients) {
        {
            // normal recipient (no payment request)
            if (i == nChangePosRet) {
                i++;
            }

            rcp.amount = walletTransaction->vout[i].nValue;
            i++;
        }
    }
}

Amount WalletModelTransaction::getTotalTransactionAmount() const {
    Amount totalTransactionAmount = Amount::zero();
    for (const SendCoinsRecipient &rcp : recipients) {
        totalTransactionAmount += rcp.amount;
    }
    return totalTransactionAmount;
}
