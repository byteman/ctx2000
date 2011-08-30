#ifndef BEEPER_H
#define BEEPER_H

class CBeeper
{
public:
    CBeeper();
    static CBeeper& get();
/*
    last_ms:     持续时间  以毫秒为单位
    interval_ms :间隔时间  以毫秒为单位
*/
    bool BeepMs(int last_ms=1000, int interval_ms=10);
};

#endif // BEEPER_H
