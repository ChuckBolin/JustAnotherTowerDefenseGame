#include "..\include\CTrack.h"

CTrack::CTrack(){
  m_currentIndex = 0;
  m_bLastPoint = false;
}

CTrack::~CTrack(){

}

void CTrack::AddWaypoint(int x, int y, int radius){
  if(radius < 1) 
    return;

  TRACK_WAYPOINT twpt;
  twpt.x = x;
  twpt.y = y;
  twpt.radius = radius;
  m_waypoint.push_back(twpt);

}

bool CTrack::GetWayPointIndex(TRACK_WAYPOINT &trackWaypoint, int index){
  trackWaypoint.x = 0;
  trackWaypoint.y = 0;
  trackWaypoint.radius = 0;

  if(index < 0 || index > m_waypoint.size() - 1)
    return false;
  if(m_waypoint.empty() == false){
    trackWaypoint = m_waypoint[index];
  }
  return true;
}

bool CTrack::GetCurrentWayPoint(TRACK_WAYPOINT &trackWaypoint){
  trackWaypoint.x = 0;
  trackWaypoint.y = 0;
  trackWaypoint.radius = 0;
  
  if(m_waypoint.empty() == true)
    return false;

  if(m_currentIndex > -1 && m_currentIndex < m_waypoint.size())
    trackWaypoint = m_waypoint[m_currentIndex];
  return true;  
}

bool CTrack::GetNextWayPoint(TRACK_WAYPOINT &trackWaypoint){
  trackWaypoint.x = 0;
  trackWaypoint.y = 0;
  trackWaypoint.radius = 0;

  if(m_waypoint.empty() == true)
  //if(m_waypoint.size() == 0)
    return false;

  int nextWaypoint = m_currentIndex + 1;
  if(nextWaypoint > m_waypoint.size() - 1)
    return false;

  if(nextWaypoint > -1 && nextWaypoint < m_waypoint.size())
    trackWaypoint = m_waypoint[nextWaypoint];
  return true;  
}

unsigned int CTrack::GetNumWaypoints(){
  return m_waypoint.size();
}

bool CTrack::IncrementWaypoint(){

  m_currentIndex++;
  if(m_currentIndex == m_waypoint.size() - 1)
    m_bLastPoint = true;

  if(m_currentIndex > m_waypoint.size() - 1){
    m_currentIndex--;
    return false;
  }

  return true;
}

bool CTrack::ResetWaypointIndex(){
  m_currentIndex = 0;
  if(m_waypoint.size() == 0)
    return false;

  return true;
}
