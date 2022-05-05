
// TIMER
// stores the ones and tens
typedef struct digits
{
    int ones;
    int tens;
} digits;

// stores the counter data
struct count
{
    int fps;
    digits secs;
    digits mins;
    int hours;
};
struct count timer = {};

// functions

// TIMER

bool CountUp(int fpsMax)
{
    // initial copy
    curMap = demo;
    mapCopy();

    // increments timer
    timer.fps++;

    // fps -> secs
    if (timer.fps == fpsMax)
    {
        timer.secs.ones++;
        timer.fps = 0;
    }

    // secs ^
    if (timer.secs.ones == 10) // 9 seconds for every 10
    {
        timer.secs.tens++;
        timer.secs.ones = 0;
    }

    // secs -> mins
    if (timer.secs.tens == 6) // 60 seconds for every min
    {
        timer.mins.ones++;
        timer.secs.tens = 0;
    }
    // mins ^
    if (timer.mins.ones == 10) // 9 mins for every 10
    {
        timer.mins.tens++;
        timer.mins.ones = 0;
    }
    // mins -> hours

    if (timer.mins.tens == 6) // 60 mins every hour
    {
        timer.hours++;
    }

    return false;
}

// stopwatch
bool StopWatch(int fps, int duration, bool (*command)(void))
{
    static int fpsCount;
    static int secs;

    fpsCount++;

    if (fpsCount == fps)
    {
        secs++;
        fpsCount = 0;
    }

    if (secs == duration)
    {
        command();
        secs = 0;
    }

    return false;
}

bool resetTime(void)
{
    timer.fps = 0;
    timer.secs.ones = 0;
    timer.secs.tens = 0;
    timer.mins.ones = 0;
    timer.mins.tens = 0;
    timer.hours = 0;

    return false;
}
