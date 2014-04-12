#ifndef CWAVEGEN_H
#define CWAVEGEN_H

#include <vector>
#include "CLog.h"
//#include <iostream>
//using namespace std;

//**************************************************************************
//file format and order
//wave, timeToStart(seconds), trackIndex, objectID, quantity, spawnInterval
//**************************************************************************

//used for tracking wave
struct  WAVE_DATA{
  //loaded from file
  
  long timeToStart;       //time to start in seconds after start of map level
  int trackIndex;         //index for the track to follow...see CTrack()
  int objectID;           //id of the object to add to m_Object list
  int quantity;           //number of objects to spawn in this wave
  double spawnInterval;   //time interval between spawns
  
  //this data used internally
  double timeWave;        //used for each wave
  int spawnCount;         //tracks the number of objects that have been spawned
  bool bSpawning;         //true if this is an active wave (can be many active waves)
  bool bDone;             //true if this wave has been completely spawned
};

//this data used outside class...outside must read x,y,radius from track 0 index
struct WAVE_OBJECT_DATA{
  int trackIndex;
  int objectID;
};

class CWaveGen{
public:
  CWaveGen();
  ~CWaveGen();
  bool AddWave(long time, int track, int object, int qty, double interval);  
  bool Update(double timeDifference, WAVE_OBJECT_DATA & wod); //returns true if something available
  long GetSpawnSecondCount();
  int GetWaveCount(){return m_waveCount;}
  void Clear();
  bool IsFinished(){return m_bFinished;}  //returns true if waves are finished

private:
  std::vector<WAVE_DATA> m_waveData;
  std::vector<WAVE_OBJECT_DATA> m_waveObjectData; //ready to spawn

  int m_waveCount;   //counts number of waves
  long m_secondCount; //counts number of seconds
  bool m_bWavesComplete; //true if there are no more units to spawn

  long m_lastTime;
  int m_lastQty;
  double m_lastInterval;
  int m_veryLastSecond;
  bool m_bFinished;
};

#endif