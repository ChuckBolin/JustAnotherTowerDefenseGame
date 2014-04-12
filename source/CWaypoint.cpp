#include "..\include\CWaypoint.h"

CWaypoint::CWaypoint(){
  m_index = -1;
}

CWaypoint::~CWaypoint(){

}

float CWaypoint::GetWaypointX(){
  if(m_index > -1)
    return m_waypoints[m_index].x;
  else
    return -1;
}

float CWaypoint::GetWaypointY(){
  if(m_index > -1)
    return m_waypoints[m_index].y;
  else
    return -1;
}


float CWaypoint::GetNextWaypointX(){
  int nextIndex = m_index + 1;
  
  if(nextIndex > m_waypoints.size())
    nextIndex = 0;

  return m_waypoints[nextIndex].x;
  
}

float CWaypoint::GetNextWaypointY(){
  int nextIndex = m_index + 1;
  
  if(nextIndex > m_waypoints.size())
    nextIndex = 0;

  return m_waypoints[nextIndex].y;
}

float CWaypoint::GetWaypointVelocity(){
  if(m_index > -1)
    return m_waypoints[m_index].velocity;
  else
    return -1;
}

void CWaypoint::Add(float x, float y, float velocity){
  WAYPOINT wp;
  wp.x = x;
  wp.y = y;
  wp.velocity = velocity;
  m_waypoints.push_back(wp);
  m_index = 0;
}

void CWaypoint::Update(float x, float y){
  float wx = m_waypoints[m_index].x;
  float wy = m_waypoints[m_index].y;

  float distance = sqrt( (wx - x) * (wx - x) + (wy - y) * (wy - y));
  float radius = 50;

  if(distance < radius){
    m_index++;
    if(m_index > m_waypoints.size())
      m_index = 0;
  }
}