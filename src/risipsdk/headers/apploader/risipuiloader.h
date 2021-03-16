/***********************************************************************************
**    Copyright (C) 2016  Petref Saraci
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

#ifndef UILOADER_H
#define UILOADER_H

#include "risipsdkglobal.h"
#include "risipapplicationsettings.h"

namespace risip {

class RISIP_VOIPSDK_EXPORT RisipUiLoader : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(RisipApplicationSettings * applicationSettings READ applicationSettings CONSTANT)
    Q_PROPERTY(QString qmlFile READ qmlFile WRITE setQmlFile NOTIFY qmlFileChanged)

    RisipUiLoader(QObject *parent = 0);
    ~RisipUiLoader();

    void start();

    RisipApplicationSettings *applicationSettings();

    QString qmlFile() const;
    void setQmlFile(const QString &qmlFile);

Q_SIGNALS:
    void qmlFileChanged(const QString &qmlfile);

private:
    class Private;
    Private *m_data;
};

} //end of risip namespace

#endif // UILOADER_H
