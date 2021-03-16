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
#ifndef PJSIPBUDDY_H
#define PJSIPBUDDY_H

#include <pjsua2.hpp>
using namespace pj;

namespace risip {

class RisipBuddy;

class PjsipBuddy: public Buddy
{
public:
    PjsipBuddy();
    ~PjsipBuddy();

    void onBuddyState();
    void setRisipInterface(RisipBuddy *risipBuddy);
    RisipBuddy *risipInterface();

private:
    RisipBuddy *m_risipBuddyInterface;
};

} //end of risip namespace

#endif // PJSIPBUDDY_H
