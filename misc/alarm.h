#ifndef __ALARM_H_
#define __ALARM_H_

#include <time.h>

class Alarm
{
  time_t d_last_time, d_alarm_time;
  unsigned int d_wait_time;
  bool d_auto_set;

public:
 Alarm(unsigned int wait_time = 0, bool auto_set = true) 
   : 
  d_last_time(0), 
    d_alarm_time(0), 
    d_wait_time(wait_time),
    d_auto_set(auto_set)
  {
    set();
  }

  void set()
  {
    d_last_time = time(0);
    d_alarm_time = d_last_time + d_wait_time;
  }
  
  bool operator()()
  {
    if (!d_wait_time) //if no wait time, never go off
        return false;
    bool alarm = time(0) >= d_alarm_time;
    if (alarm && d_auto_set)
      set();
    return alarm;
  }

  void wait_time(unsigned int time)
  {
    d_wait_time = time;
    set();
  }

};

#endif
