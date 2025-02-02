// Copyright (c) 2011-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "bitcoinunits.h"

#include "primitives/transaction.h"

#include <QStringList>

BitcoinUnits::BitcoinUnits(QObject *parent)
    : QAbstractListModel(parent), unitlist(availableUnits()) {}

QList<BitcoinUnits::Unit> BitcoinUnits::availableUnits() {
    QList<BitcoinUnits::Unit> unitlist;
    unitlist.append(DVT);
    unitlist.append(mDVT);
    return unitlist;
}

bool BitcoinUnits::valid(int unit) {
    switch (unit) {
        case DVT:
        case mDVT:
            return true;
        default:
            return false;
    }
}

QString BitcoinUnits::name(int unit) {
    switch (unit) {
        case DVT:
            return QString("DVT");
        case mDVT:
            return QString("mDVT");
        default:
            return QString("???");
    }
}

QString BitcoinUnits::description(int unit) {
    switch (unit) {
        case DVT:
            return QString("DVTs");
        case mDVT:
            return QString("Milli-DVTs (1 / 1" THIN_SP_UTF8 "000)");
        default:
            return QString("???");
    }
}

qint64 BitcoinUnits::factor(int unit) {
    switch (unit) {
        case DVT:
            return 100000000;
        case mDVT:
            return 100000;
        default:
            return 100000000;
    }
}

int BitcoinUnits::decimals(int unit) {
    switch (unit) {
        case DVT:
            return 8;
        case mDVT:
            return 5;
        default:
            return 0;
    }
}

int BitcoinUnits::display_decimals(int unit) {
  switch (unit) {
    case DVT:
      return 3;
    default:
      return 0;
  }
}
int BitcoinUnits::scale_decimals(int unit) {
  switch (unit) {
    case DVT:
      return Amount::min_amount().toInt();
    default:
      return COIN.toInt()/(1000*Amount::min_amount().toInt()); // 1000 since mDVT
  }
}


QString BitcoinUnits::format(int unit, const Amount nIn, bool fPlus,
                             SeparatorStyle separators) {
    // Note: not using straight sprintf here because we do NOT want
    // localized number formatting.
    if (!valid(unit)) {
        // Refuse to format invalid unit
        return QString();
    }
    qint64 n = qint64(nIn.toInt());
    qint64 coin = factor(unit);
    qint64 n_abs = (n > 0 ? n : -n);
    qint64 quotient = n_abs / coin;
    qint64 remainder = n_abs % coin;
    // Now since we are using less decimal places
    // divide the remainder to appropriate precision
    remainder /= scale_decimals(unit);
    // and use number of decimals specified here
    int num_decimals = display_decimals(unit);
    //
    QString quotient_str = QString::number(quotient);
    QString remainder_str =
        QString::number(remainder).rightJustified(num_decimals, '0', true);

    // Use SI-style thin space separators as these are locale independent and
    // can't be confused with the decimal marker.
    QChar thin_sp(THIN_SP_CP);
    int q_size = quotient_str.size();
    if (separators == separatorAlways ||
        (separators == separatorStandard && q_size > 4)) {
        for (int i = 3; i < q_size; i += 3) {
            quotient_str.insert(q_size - i, thin_sp);
        }
    }

    if (n < 0) {
        quotient_str.insert(0, '-');
    } else if (fPlus && n > 0) {
        quotient_str.insert(0, '+');
    }

    return quotient_str + QString(".") + remainder_str;
}

// NOTE: Using formatWithUnit in an HTML context risks wrapping
// quantities at the thousands separator. More subtly, it also results
// in a standard space rather than a thin space, due to a bug in Qt's
// XML whitespace canonicalisation
//
// Please take care to use formatHtmlWithUnit instead, when
// appropriate.

QString BitcoinUnits::formatWithUnit(int unit, const Amount amount,
                                     bool plussign, SeparatorStyle separators) {
    return format(unit, amount, plussign, separators) + QString(" ") +
           name(unit);
}

QString BitcoinUnits::formatHtmlWithUnit(int unit, const Amount amount,
                                         bool plussign,
                                         SeparatorStyle separators) {
    QString str(formatWithUnit(unit, amount, plussign, separators));
    str.replace(QChar(THIN_SP_CP), QString(THIN_SP_HTML));
    return QString("<span style='white-space: nowrap;'>%1</span>").arg(str);
}

bool BitcoinUnits::parse(int unit, const QString &value, Amount *val_out) {
    if (!valid(unit) || value.isEmpty()) {
        // Refuse to parse invalid unit or empty string
        return false;
    }
    int num_decimals = decimals(unit);
    int num_display_decimals = display_decimals(unit);

    // Ignore spaces and thin spaces when parsing
    QStringList parts = removeSpaces(value).split(".");

    if (parts.size() > 2) {
        // More than one dot
        return false;
    }
    QString whole = parts[0];
    QString decimals;

    if (parts.size() > 1) {
        decimals = parts[1];
    }
    if (decimals.size() > num_display_decimals) {
        // Exceeds max precision
        return false;
    }
    bool ok = false;
    QString str = whole + decimals.leftJustified(num_decimals, '0');

    if (str.size() > 18) {
        // Longer numbers will exceed 63 bits
        return false;
    }
    Amount retvalue(int64_t(str.toLongLong(&ok)));
    if (val_out) {
        *val_out = retvalue;
    }
    return ok;
}

QString BitcoinUnits::getAmountColumnTitle(int unit) {
    QString amountTitle = QObject::tr("Amount");
    if (BitcoinUnits::valid(unit)) {
        amountTitle += " (" + BitcoinUnits::name(unit) + ")";
    }
    return amountTitle;
}

int BitcoinUnits::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return unitlist.size();
}

QVariant BitcoinUnits::data(const QModelIndex &index, int role) const {
    int row = index.row();
    if (row >= 0 && row < unitlist.size()) {
        Unit unit = unitlist.at(row);
        switch (role) {
            case Qt::EditRole:
            case Qt::DisplayRole:
                return QVariant(name(unit));
            case Qt::ToolTipRole:
                return QVariant(description(unit));
            case UnitRole:
                return QVariant(static_cast<int>(unit));
        }
    }
    return QVariant();
}

Amount BitcoinUnits::maxMoney() {
    return MAX_MONEY;
}
