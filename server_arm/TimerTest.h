#pragma once
#include "TemplateTimer.h"

class CTimerTest
{
public:
    void RunTest();

private:
    void OnTimedEvent1();

    TTimer<CTimerTest> timer1 ;
};
