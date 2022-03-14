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

#ifndef THEME_H
#define THEME_H

#include <QObject>

class Theme : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString mainColor READ mainColor NOTIFY mainColorChanged)
    Q_PROPERTY(QString lighterColor READ lighterColor NOTIFY lighterColorChanged)

public:
    explicit Theme(QObject *parent = nullptr);
    inline QString mainColor() { return mMainColor; }
    inline QString lighterColor() { return mLighterColor; }
    void setThemeColor(const QString &color);

    static const QString DEFAULT_THEME_COLOR;

signals:
    void mainColorChanged();
    void lighterColorChanged();

public slots:
    float getHue(const QString &color);
    float getSaturation(const QString &color);
    float getLightness(const QString &color);

private:
    QString mMainColor;
    QString mLighterColor;

};

#endif // THEME_H
