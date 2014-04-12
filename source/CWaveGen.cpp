#include "..\include\CWaveGen.h"

CWaveGen::CWaveGen(){
  m_waveCount = 0;
  m_secondCount = 0;

  m_bWavesComplete = false;
  m_lastTime = 60;
  m_lastQty = 1;
  m_lastInterval = 1;
  m_veryLastSecond = 60;
}

CWaveGen::~CWaveGen(){

}

bool CWaveGen::AddWave(long time, int track, int object, int qty, double interval){
  WAVE_DATA wd;
  wd.timeToStart = time;
  wd.trackIndex = track;
  wd.objectID = object;
  wd.quantity = qty;
  wd.spawnInterval = interval;
  wd.bDone = false;
  wd.bSpawning = false;
  wd.spawnCount = 0;
  wd.timeWave = 0;
  m_waveData.push_back(wd);

  m_lastTime = time;
  m_lastQty = qty;
  m_lastInterval = interval;
  m_veryLastSecond = m_lastTime + (long)((double)(m_lastQty + 1) * interval);
  m_bFinished = false;

  return true;  
}

//update MUST
//set bSpawning = true for each wave
//add objects to m_waveObjectData
bool CWaveGen::Update(double timeDifference, WAVE_OBJECT_DATA & wod){
//  CLog *pLog = CLog::Instance();

  static double totalTime = 0;
  totalTime += timeDifference;

  //updates second counter
  if(totalTime > 1.0){
    totalTime = 0;
    m_secondCount++;

    //waves are all over
    if(m_secondCount > m_veryLastSecond)
      m_bFinished = true;

    //set bSpawning true if it's now time
    for(unsigned int i = 0; i < m_waveData.size(); ++i){
      if(m_secondCount == m_waveData[i].timeToStart &&
        m_waveData[i].bDone == false && m_waveData[i].bSpawning == false){
         m_waveData[i].bSpawning = true;
         m_waveData[i].timeWave = m_waveData[i].spawnInterval + 0.001;
         m_waveCount++;
      }
    }
  }

  //determine if a new object is being spawned
  WAVE_OBJECT_DATA tempWod;
  for(unsigned int i = 0; i < m_waveData.size(); ++i){
    if(m_waveData[i].bSpawning == true){     
      m_waveData[i].timeWave += timeDifference;
      if(m_waveData[i].timeWave > m_waveData[i].spawnInterval){
        m_waveData[i].timeWave = 0;
        
        //add object info to vector
        tempWod.objectID = m_waveData[i].objectID;
        tempWod.trackIndex = m_waveData[i].trackIndex;
        m_waveObjectData.push_back(tempWod);        
        m_waveData[i].spawnCount++;
        
        //spawn no more
        if(m_waveData[i].spawnCount >= m_waveData[i].quantity){
          m_waveData[i].bDone = true;
          m_waveData[i].bSpawning = false;
        }
      }
    }
  }

  //return true if something to return and pops wod
  if(m_waveObjectData.size() > 0){
    wod = m_waveObjectData[m_waveObjectData.size() - 1];
    m_waveObjectData.pop_back();
    return true;
  }
  else
    return false;
}

long CWaveGen::GetSpawnSecondCount(){
  return m_secondCount;
}

void CWaveGen::Clear(){
  m_waveData.clear();
  m_waveObjectData.clear();
  m_waveCount = 0;
  m_secondCount = 0;
}


