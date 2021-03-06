/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-9 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_MultiTimer.h"
#include "juce_Timer.h"
#include "../threads/juce_ScopedLock.h"


//==============================================================================
class MultiTimer::MultiTimerCallback    : public Timer
{
public:
    MultiTimerCallback (const int timerId_, MultiTimer& owner_)
        : timerId (timerId_),
          owner (owner_)
    {
    }

    ~MultiTimerCallback()
    {
    }

    void timerCallback()
    {
        owner.timerCallback (timerId);
    }

    const int timerId;

private:
    MultiTimer& owner;
};

//==============================================================================
MultiTimer::MultiTimer() throw()
{
}

MultiTimer::MultiTimer (const MultiTimer&) throw()
{
}

MultiTimer::~MultiTimer()
{
    const ScopedLock sl (timerListLock);
    timers.clear();
}

//==============================================================================
void MultiTimer::startTimer (const int timerId, const int intervalInMilliseconds) throw()
{
    const ScopedLock sl (timerListLock);

    for (int i = timers.size(); --i >= 0;)
    {
        MultiTimerCallback* const t = timers.getUnchecked(i);

        if (t->timerId == timerId)
        {
            t->startTimer (intervalInMilliseconds);
            return;
        }
    }

    MultiTimerCallback* const newTimer = new MultiTimerCallback (timerId, *this);
    timers.add (newTimer);
    newTimer->startTimer (intervalInMilliseconds);
}

void MultiTimer::stopTimer (const int timerId) throw()
{
    const ScopedLock sl (timerListLock);

    for (int i = timers.size(); --i >= 0;)
    {
        MultiTimerCallback* const t = timers.getUnchecked(i);

        if (t->timerId == timerId)
            t->stopTimer();
    }
}

bool MultiTimer::isTimerRunning (const int timerId) const throw()
{
    const ScopedLock sl (timerListLock);

    for (int i = timers.size(); --i >= 0;)
    {
        const MultiTimerCallback* const t = timers.getUnchecked(i);
        if (t->timerId == timerId)
            return t->isTimerRunning();
    }

    return false;
}

int MultiTimer::getTimerInterval (const int timerId) const throw()
{
    const ScopedLock sl (timerListLock);

    for (int i = timers.size(); --i >= 0;)
    {
        const MultiTimerCallback* const t = timers.getUnchecked(i);
        if (t->timerId == timerId)
            return t->getTimerInterval();
    }

    return 0;
}


END_JUCE_NAMESPACE
