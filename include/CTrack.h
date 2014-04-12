#ifndef CTRACK_H
#define CTRACK_H

#include <vector>

struct TRACK_WAYPOINT{
  int x;
  int y;
  int radius;
};

class CTrack{
public:
  CTrack();
  ~CTrack();
  void AddWaypoint(int x, int y, int radius);
  bool GetWayPointIndex(TRACK_WAYPOINT &trackWaypoint, int index);
  bool GetCurrentWayPoint(TRACK_WAYPOINT &trackWaypoint);
  bool GetNextWayPoint(TRACK_WAYPOINT &trackWaypoint);
  bool IncrementWaypoint();
  bool ResetWaypointIndex();  
  unsigned int GetNumWaypoints();
  bool IsLastPoint(){return m_bLastPoint;}

private:
  std::vector<TRACK_WAYPOINT> m_waypoint;
  int m_currentIndex;
  bool m_bLastPoint;
};

#endif