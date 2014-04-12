#ifndef CWAYPOINT_H
#define CWAYPOINT_H

#include <vector>
#include <cmath>

struct WAYPOINT{
  float x;
  float y;
  float velocity;
};

class CWaypoint{
public:
  CWaypoint();
  ~CWaypoint();
  std::vector<WAYPOINT> m_waypoints;
  float GetWaypointX();
  float GetWaypointY();
  float GetWaypointVelocity();
  float GetNextWaypointX();
  float GetNextWaypointY();

  void Add(float x, float y, float velocity);
  void Update(float x, float y);
private:
  int m_index;

};

#endif