#include "../include/CGameClock.h"

CGameClock::CGameClock(){
  timer.Initialize();
  timeRemaining = 1800;
  startDay = 1;
  endDay = 11;
  startHour = 12;
  endHour = 24;
  seconds = 0;
  timeScale = 1;
  m_paused = false;
}

void CGameClock::Update(){
  static double secondTime;
  double timeDiff;
  
  if(m_paused == true){
    timeDiff = timer.getTimeDifference();//absorb the time
    return;
  }

  timeDiff = timer.getTimeDifference();
  secondTime += timeDiff;

  if(secondTime > 1.0){
    secondTime -= 1.0 ;
    timeRemaining -= (int)timeScale;
  }

  if(timeRemaining < 0)
    timeRemaining = 0;
}

void CGameClock::SetSeconds(int sec){
  timeRemaining = sec;
}
int CGameClock::GetDay(){
  return startDay + ((1890 - timeRemaining) / 180); 
}

int CGameClock::GetHour(){
  int hour = (int)((float)(1890 - timeRemaining) / 7.5f);
  
  return hour % 24;
}

int CGameClock::GetSeconds(){
  return timeRemaining;
}

int CGameClock::GetLightSeconds(){
  return (1890 - timeRemaining) % 180;
}

void CGameClock::SetTimeScale(double scale){
  timeScale = scale;
}

double CGameClock::GetTimeScale(){
  return timeScale;
}
