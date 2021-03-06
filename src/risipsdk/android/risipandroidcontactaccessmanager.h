/***********************************************************************************
**    Copyright (C) 2016  Petref Saraci
**    http://risip.io
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You have received a copy of the GNU General Public License
**    along with this program. See LICENSE.GPLv3
**    A copy of the license can be found also here <http://www.gnu.org/licenses/>.
**
************************************************************************************/
#ifndef RISIPANDROIDCONTACTACCESSMANAGER_H
#define RISIPANDROIDCONTACTACCESSMANAGER_H

#include <QObject>

namespace risip {

class RisipPhoneContact;

class RisipAndroidContactAccessManager : public QObject
{
    Q_OBJECT
public:
    explicit RisipAndroidContactAccessManager(QObject *parent = 0);
    void fetchContactsFromDevice();

Q_SIGNALS:
    void phoneContactDiscovered(RisipPhoneContact *phoneContact);

private:
    void parseXML(const QString &xml);
};

} //end of risip namespace

#endif // RISIPANDROIDCONTACTACCESSMANAGER_H
