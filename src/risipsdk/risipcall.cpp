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

#include "risipcall.h"
#include "risipendpoint.h"
#include "risipaccountconfiguration.h"
#include "risipmodels.h"
#include "risipcallmanager.h"
#include "risipglobals.h"
#include "risip.h"

#include "pjsipwrapper/pjsipaccount.h"
#include "pjsipwrapper/pjsipcall.h"

#include <QDebug>

namespace risip {

class RisipCall::Private
{
public:
    int callType;
    int callDirection;
    RisipAccount *account;
    RisipBuddy *buddy;
    QDateTime timestamp;
    RisipMedia *risipMedia;
    PjsipCall *pjsipCall;
    Error error;
    int lastSipResponseCode;
};

RisipCall::RisipCall(QObject *parent)
    :QObject(parent)
    ,m_data(new Private)
{
    m_data->account = NULL;
    m_data->buddy = NULL;
    m_data->risipMedia = NULL;
    m_data->pjsipCall = NULL;
    m_data->callType = RisipCall::Sip;
    m_data->callDirection = Unknown;
    m_data->lastSipResponseCode = Risip::PJSIP_SC_OK;
}

RisipCall::~RisipCall()
{
    disconnect(this);

    delete m_data;
    m_data = NULL;
}

RisipAccount *RisipCall::account() const
{
    return m_data->account;
}

void RisipCall::setAccount(RisipAccount *acc)
{
    if(m_data->account != acc ) {
        m_data->account = acc;
        emit accountChanged(m_data->account);
    }
}

RisipBuddy *RisipCall::buddy() const
{
    return m_data->buddy;
}

void RisipCall::setBuddy(RisipBuddy *buddy)
{
    if(m_data->buddy != buddy) {
        m_data->buddy = buddy;
        emit buddyChanged(m_data->buddy);
    }

    setCallType(Sip);
}

RisipMedia *RisipCall::media() const
{
    return m_data->risipMedia;
}

void RisipCall::setMedia(RisipMedia *med)
{
    if(m_data->risipMedia != med) {
        if(m_data->risipMedia) {
            delete m_data->risipMedia;
            m_data->risipMedia = NULL;
        }

        m_data->risipMedia = med;
        if(m_data->risipMedia) {
            m_data->risipMedia->setActiveCall(this);
        }

        emit mediaChanged(m_data->risipMedia);
    }
}

int RisipCall::callId() const
{
   //[SG] [27-08-2020]  Bugfixed to get call ID

    if(m_data->pjsipCall)
        return m_data->pjsipCall->getId();

    return -1;
}

/**
 * @brief RisipCall::setPjsipCall
 * @param call
 *
 * Internal API. Initiates the call
 */
void RisipCall::setPjsipCall(PjsipCall *call)
{
    if(m_data->pjsipCall != NULL) {
        delete m_data->pjsipCall;
        m_data->pjsipCall = NULL;
    }

    m_data->pjsipCall = call;
    setMedia(NULL);

    if(m_data->pjsipCall != NULL) {
        m_data->pjsipCall->setRisipCall(this);
    }

    emit statusChanged();
}

/**
 * @brief RisipCall::pjsipCall
 * @return pjsip object
 *
 * Internal API.
 */
PjsipCall *RisipCall::pjsipCall() const
{
    return m_data->pjsipCall;
}

int RisipCall::callType() const
{
    return m_data->callType;
}

void RisipCall::setCallType(int type)
{
    if(m_data->callType != type) {
        m_data->callType = type;
        emit callTypeChanged(m_data->callType);
    }
}

int RisipCall::status() const
{
    if(m_data->pjsipCall == NULL)
        return Null;

    if(m_data->pjsipCall->isActive()) {
        CallInfo callInfo = m_data->pjsipCall->getInfo();
        switch (callInfo.state) {
        case PJSIP_INV_STATE_CALLING:
            return RisipCall::OutgoingCallStarted;
        case PJSIP_INV_STATE_CONNECTING:
            return RisipCall::ConnectingToCall;
        case PJSIP_INV_STATE_CONFIRMED:
            return RisipCall::CallConfirmed;
        case PJSIP_INV_STATE_DISCONNECTED:
            return RisipCall::CallDisconnected;
        case PJSIP_INV_STATE_EARLY:
            return RisipCall::CallEarly;
        case PJSIP_INV_STATE_INCOMING:
            return RisipCall::IncomingCallStarted;
        case PJSIP_INV_STATE_NULL:
            return RisipCall::Null;
        default:
            return RisipCall::Null;
        }
    } else {
        return RisipCall::Null;
    }
}

QDateTime RisipCall::timestamp() const
{
    return m_data->timestamp;
}

/**
 * @brief RisipCall::createTimestamp
 *
 * Internal API.
 */
void RisipCall::createTimestamp()
{
    m_data->timestamp = QDateTime::currentDateTime();
    emit timestampChanged(m_data->timestamp);
}

int RisipCall::callDirection() const
{
    return m_data->callDirection;
}

/**
 * @brief RisipCall::setCallDirection
 * @param direction
 *
 * Internal API.
 */
void RisipCall::setCallDirection(int direction)
{
    if(m_data->callDirection != direction) {
        m_data->callDirection = direction;
        emit callDirectionChanged(m_data->callDirection);
    }
}

/**
 * @brief RisipCall::callDuration
 * @return call duration in msec
 *
 * Retuns the duration of the call in milliseconds.
 */
long RisipCall::callDuration() const
{
    if(!m_data->pjsipCall)
        return 0.0;

    return m_data->pjsipCall->getInfo().connectDuration.msec;
}

int RisipCall::errorCode() const
{
    return m_data->error.status;
}

QString RisipCall::errorMessage()
{
    return QString::fromStdString(m_data->error.reason);
}

QString RisipCall::errorInfo() const
{
    return QString::fromStdString(m_data->error.info(true));
}

int RisipCall::lastResponseCode() const
{
    return m_data->lastSipResponseCode;
}

void RisipCall::setLastResponseCode(int response)
{
    if(m_data->lastSipResponseCode != response) {
        m_data->lastSipResponseCode = response;
        emit lastResponseCodeChanged(response);
    }
}

/**
 * @brief RisipCall::initializeMediaHandler
 *
 * Internal API.
 *
 * Initializes media objects for an active call that has been established and answered.
 */
void RisipCall::initializeMediaHandler()
{
    if(!m_data->risipMedia)
        setMedia(new RisipMedia);

    m_data->risipMedia->startCallMedia();
}

/**
 * @brief RisipCall::answer
 *
 * Answers an incoming call.
 *
 * @see RisipCallManager how incoming are handled.
 */
void RisipCall::answer()
{
    if(!m_data->account)
        return;

    if(m_data->pjsipCall != NULL) { //check if call object is set
        RisipCallManager::instance()->setActiveCall(this);
        CallOpParam prm;
        prm.statusCode = PJSIP_SC_OK;
        try {
            m_data->pjsipCall->answer(prm);
        } catch (Error &err) {
            setError(err);
        }
    } else {
        qDebug()<<"no account set or call id!";
    }
}

void RisipCall::hangup()
{
    if(m_data->pjsipCall == NULL
            || !m_data->pjsipCall->isActive()) {
        qDebug()<<"no call exists/active";

        emit statusChanged();
        return;
    }

    CallOpParam prm;
    try {
        m_data->pjsipCall->hangup(prm);
    } catch (Error &err) {
        setError(err);
    }

    emit statusChanged();
    RisipCallManager::instance()->setActiveCall(NULL);
}

/**
 * @brief RisipCall::call
 *
 * Internal API.
 * Use may use it with caution. @see RisipCallManager
 */
void RisipCall::call()
{
    if(m_data->callType == Undefined
            || !m_data->account
            || !m_data->buddy)
        return;

    if(m_data->account->status() != RisipAccount::SignedIn)
        return;

    setCallDirection(RisipCall::Outgoing);
    createTimestamp();
    setPjsipCall(new PjsipCall(*m_data->account->pjsipAccount()));
    CallOpParam prm(true);

    try {
        m_data->pjsipCall->makeCall(m_data->buddy->uri().toStdString(), prm);
    } catch (Error err) {
        setError(err);
    }
}

void RisipCall::invite(const QString &uri)
{
    setCallType(RisipCall::Sip);
    if(!m_data->account && uri.isEmpty())
        return;

    if(m_data->account->status() != RisipAccount::SignedIn)
        return;

    setCallDirection(RisipCall::Outgoing);
    createTimestamp();
    setPjsipCall(new PjsipCall(*m_data->account->pjsipAccount()));
    CallOpParam prm(true);

    try {
        m_data->pjsipCall->makeCall(uri.toStdString(), prm);
    } catch (Error err) {
        setError(err);
    }
}

void RisipCall::reinvite()
{
    int callid = this->callId();
    qDebug() <<"Send reinvite Callid:"<<callid;
    if ( callid == -1)
    {
        return;
    }


    pj_status_t x = pjsua_call_reinvite(callid, PJSUA_CALL_UNHOLD, NULL);

    qDebug() << "Reinvite: "<< x;
}

void RisipCall::transferDirect(const QString &destUri)
{

}

void RisipCall::transferAttendedCall(const QString &destUri)
{

}

/**
 * @brief RisipCall::hold
 * @param hold
 *
 * Use this function to hold/unhold an active call.
 */
void RisipCall::hold(bool hold)
{
    if(m_data->pjsipCall == NULL
            || !m_data->pjsipCall->isActive()) {
        qDebug()<<"no call exists nor is active";
        return;
    }

    if(hold) {
        CallOpParam prm;
        prm.options = PJSUA_CALL_UPDATE_CONTACT;
        try {
            m_data->pjsipCall->setHold(prm);
        } catch (Error &err) {
            setError(err);
        }
    } else {
        CallOpParam prm;
        prm.opt.flag = PJSUA_CALL_UNHOLD;
        try {
            m_data->pjsipCall->reinvite(prm);
        } catch (Error &err) {
            setError(err);
        }
    }
}

/**
 * @brief RisipCall::initiateIncomingCall
 *
 * Internal API.
 *
 * Used for initiating/handling an incoming call from the account.
 *
 * @see RisipCallManager how it is used.
 */
void RisipCall::initiateIncomingCall()
{
    if(!m_data->account)
        return;

    if(m_data->account->status() == RisipAccount::SignedIn) {
        setPjsipCall(m_data->account->incomingPjsipCall());
        createTimestamp();
        setCallDirection(RisipCall::Incoming);
    }
    //[SG]
    qDebug() << "initiateIncomingCall";
}

void RisipCall::setError(const Error &error)
{
    if(m_data->error.status != error.status) {
        qWarning()<<" ERROR: " <<"code: "<<error.status <<" info: " << QString::fromStdString(error.info(true));

        m_data->error.status = error.status;
        m_data->error.reason = error.reason;
        m_data->error.srcFile = error.srcFile;
        m_data->error.srcLine = error.srcLine;
        m_data->error.title = error.title;

        emit errorCodeChanged(m_data->error.status);
        emit errorMessageChanged(QString::fromStdString(m_data->error.reason));
        emit errorInfoChanged(QString::fromStdString(m_data->error.info(true)));
    }
}



//[27-08-2020][SG]
int RisipCall::sendDTMF(QString dtmf)
{
   // char x[32];
    QByteArray ba = dtmf.toLocal8Bit();

 //   sprintf(x,"%s",dtmf.toStdString());
    //x = dtmf.toStdString();
    int callid = this->callId();
    qDebug() <<"Send DTMF, Callid:"<<callid;
    if ( callid == -1)
    {
        return -1;
    }

//    const pjsua_call_send_dtmf_param *param
//            pjsua_call_send_dtmf (pjsua_call_id call_id, const pjsua_call_send_dtmf_param *param)
    pjsua_call_send_dtmf_param param;
    pjsua_call_send_dtmf_param_default(&param);
    param.digits = pj_str(ba.data()); //.ptr = QString::toStdString(dtmf);

    return pjsua_call_send_dtmf (callid, &param);

}


} //end of risip namespace
