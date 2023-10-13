/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2011 Emanuele Colombo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "theme.h"

#include <QColor>

const QString Theme::DEFAULT_THEME_COLOR = "#248b00";

Theme::Theme(QObject *parent) :
    QObject(parent),
    mMainColor(DEFAULT_THEME_COLOR), mLighterColor("#4cb328")
{
}

void Theme::setThemeColor(const QString &color)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    QColor c = QColor::fromString(color);
#else
    QColor c;
    c.setNamedColor(color);
#endif
    c.setRed(qMin(c.red() + 40, 255));
    c.setGreen(qMin(c.green() + 40, 255));
    c.setBlue(qMin(c.blue() + 40, 255));

    mMainColor = color;
    mLighterColor = c.name();
    emit mainColorChanged();
    emit lighterColorChanged();
}

float Theme::getHue(const QString &color) {

#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    QColor c = QColor::fromString(color);
#else
    QColor c;
    c.setNamedColor(color);
#endif
    QColor converted = c.toHsv();
    return converted.hsvHueF();
}

float Theme::getSaturation(const QString &color) {

#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    QColor c = QColor::fromString(color);
#else
    QColor c;
    c.setNamedColor(color);
#endif
    QColor converted = c.toHsv();
    return converted.hsvSaturationF();
}

float Theme::getLightness(const QString &color) {

#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    QColor c = QColor::fromString(color);
#else
    QColor c;
    c.setNamedColor(color);
#endif
    QColor converted = c.toHsv();
    return converted.valueF();
}
