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
************************************************************************************/

#ifndef RISIPCALL_H
#define RISIPCALL_H

#include "risipsdkglobal.h"
#include "risipmedia.h"
#include "risipbuddy.h"
#include "risipaccount.h"

#include <QDateTime>
#include <QObject>

#include <pjsua2.hpp>
using namespace pj;

namespace risip {

class PjsipCall;

class RISIP_VOIPSDK_EXPORT RisipCall : public QObject
{
    Q_OBJECT

public:

    enum CallType {
        Pstn = 1,
        Sip,
        Undefined = -1
    };

    enum CallDirection {
        Incoming = 1,
        Outgoing,
        Unknown = -1
    };

    enum Status {
        OutgoingCallStarted = 1,
        IncomingCallStarted,
        ConnectingToCall,
        CallConfirmed,
        CallDisconnected,
        CallEarly,
        Null
    };

    Q_ENUM(CallType)
    Q_ENUM(CallDirection)
    Q_ENUM(Status)
    Q_PROPERTY(int callType READ callType NOTIFY callTypeChanged)
    Q_PROPERTY(RisipAccount * account READ account WRITE setAccount NOTIFY accountChanged)
    Q_PROPERTY(RisipBuddy * buddy READ buddy WRITE setBuddy NOTIFY buddyChanged)
    Q_PROPERTY(RisipMedia * media READ media NOTIFY mediaChanged)
    Q_PROPERTY(int callId READ callId NOTIFY callIdChanged)
    Q_PROPERTY(int status READ status NOTIFY statusChanged)
    Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged)
    Q_PROPERTY(int callDirection READ callDirection NOTIFY callDirectionChanged)
    Q_PROPERTY(long callDuration READ callDuration CONSTANT)
    Q_PROPERTY(int errorCode READ errorCode NOTIFY errorCodeChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(QString errorInfo READ errorInfo NOTIFY errorInfoChanged)
    Q_PROPERTY(int lastResponseCode READ lastResponseCode NOTIFY lastResponseCodeChanged)

    RisipCall(QObject *parent = 0);
    ~RisipCall();

    risip::RisipAccount *account() const;
    void setAccount(risip::RisipAccount *acc);

    risip::RisipBuddy *buddy() const;
    void setBuddy(risip::RisipBuddy *buddy);

    int callType() const;
    void setCallType(int type);

    RisipMedia *media() const;
    int callId() const;
    int status() const;
    QDateTime timestamp() const;
    int callDirection() const;
    long callDuration() const;
    int errorCode() const;
    QString errorMessage();
    QString errorInfo() const;
    int lastResponseCode() const;

    //[SG] [27-08-2020]
    int sendDTMF(QString dtmf);

public Q_SLOTS:
    void answer();
    void hangup();
    void call();
    void invite(const QString &uri);
    void reinvite();
    void transferDirect(const QString &destUri);
    void transferAttendedCall(const QString &destUri);
    void hold(bool hold);

Q_SIGNALS:
    void accountChanged(RisipAccount *account);
    void buddyChanged(RisipBuddy *buddy);
    void mediaChanged(RisipMedia *media);
    void callIdChanged(int callId);
    void callTypeChanged(int type);
    void statusChanged();
    void timestampChanged(QDateTime timestamp);
    void callDirectionChanged(int direction);
    void errorCodeChanged(int code);
    void errorMessageChanged(const QString &message);
    void errorInfoChanged(const QString &info);
    void lastResponseCodeChanged(int response);


private:
    PjsipCall *pjsipCall() const;
    void setPjsipCall(PjsipCall *call);
    void createTimestamp();
    void setCallDirection(int direction);
    void initiateIncomingCall();
    void initializeMediaHandler();
    void setMedia(RisipMedia *med);
    void setError(const Error &error);
    void setLastResponseCode(int response);

    friend class RisipCallManager;
    friend class RisipMedia;
    friend class PjsipCall;

    class Private;
    Private *m_data;
};

} //end of risip namespace

#endif // RISIPCALL_H
