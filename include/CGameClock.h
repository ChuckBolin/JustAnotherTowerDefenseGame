/*******************************************************
  CGameClock - Written by Chuck Bolin
  Purpose: Game requires values for day of the month,
  hour of



*******************************************************/

#ifndef CGAMECLOCK_H
#define CGAMECLOCK_H

#include "CTimer.h"

class CGameClock{
public:
  CGameClock();
  void Update();
  int GetDay();
  int GetHour();
  int GetSeconds();
  int GetLightSeconds();
  void SetTimeScale(double scale);
  double GetTimeScale();
  void Pause(bool pause){m_paused = pause;}
  void SetSeconds(int sec);

private:
  int timeRemaining;
  int startDay;
  int endDay;
  int startHour; 
  int endHour;
  int seconds;
  CTimer timer;
  double timeScale;
  bool m_paused;

};

#endif