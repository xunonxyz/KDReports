/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include <QTextDocument>

#ifdef HAVE_KDCHART
#include <KDChartChart>

#include "KDReportsChartTextObject_p.h"
#include "KDReportsLayoutHelper_p.h" // mmToPixels
#include "KDReportsUnit.h"

namespace KDReports {

Q_GLOBAL_STATIC(ChartTextObject, globalChartInterface)

void ChartTextObject::registerChartTextObjectHandler(QTextDocument *doc)
{
    ChartTextObject *chartInterface = globalChartInterface();

    // This assert is here because a bad build environment can cause this to fail. There is a note
    // in the Qt source that indicates an error should be output, but there is no such output.
    QTextObjectInterface *iface = qobject_cast<QTextObjectInterface *>(chartInterface);
    if (!iface) {
        Q_ASSERT(iface);
    }
    doc->documentLayout()->registerHandler(ChartTextObject::ChartObjectTextFormat, chartInterface);
}

QSizeF ChartTextObject::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(posInDocument);

    QSizeF size(format.property(Size).toSizeF());

    switch (format.property(Unit).toInt()) {
    case Percent: {
        const qreal pageWidth = doc->pageSize().width();
        const qreal percent = size.width();
        const qreal ratio = size.height() / size.width();

        qreal newWidth = pageWidth * percent / 100.0;

        // XXX For some reason 100% width charts will get cropped
        // need to make a little room
        if (percent == 100.0)
            newWidth -= 9;

        return QSizeF(newWidth, newWidth * ratio);
    }
    case Millimeters:
        return QSizeF(mmToPixels(size.width()), mmToPixels(size.height()));
    }

    return QSizeF();
}

void ChartTextObject::drawObject(QPainter *painter, const QRectF &r, QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(doc);
    Q_UNUSED(posInDocument);
    Q_UNUSED(format);

    KDChart::Chart *chart = qvariant_cast<KDChart::Chart *>(format.property(ChartObject));

    chart->paint(painter, r.toRect());
}

} // namespace

#endif
