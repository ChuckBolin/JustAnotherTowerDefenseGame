/**************************************************************************************
Filename: CGameData.h Date: December, 2008
Purpose: Manages all game data.
**************************************************************************************/
#ifndef CGAMEDATA_H
#define CGAMEDATA_H

#include <string>
#include <vector>
#include <windows.h>
#include "CLog.h"
#include "CFileReader.h"
#include "CCSVReader.h"
#include "CGraphics.h"
#include "CGameClock.h"
//#include "CImpact.h"
#include "CWeapon.h"
#include "CObject.h"
#include "CMapMgr.h"
#include "CTrack.h"
#include "CWaveGen.h"
#include "CProjectile.h"

class CGraphics;
class CConfigData;
class CObject;
class CProjectile;

class CGameData{
public:
  CGameData();
  ~CGameData();
  bool LoadConfigFile(std::string filename);
  bool SaveLevelFile(int fileNum);
  void Update(double timeDifference);
  void IncrementCredits(float health);

  float m_FPS;
  HWND m_hWnd;
  
  //environmental boundaries
  int m_worldX;
  int m_worldY;  
  bool m_bLeftMouseDown;
  bool m_bRightMouseDown;

  bool LoadGameLevel(std::string filename);
  bool LoadObjectFile(std::string filename);
  void OpenLevel (int level, CGraphics &con);
  bool AddObject(CGraphics &con, std::string objectName, int x, int y);
  bool AddGraphicDataToLevelData(CGraphics &con);

  //variables loaded from level1.dat file
  int m_screenColorRed;  //background color
  int m_screenColorGreen;
  int m_screenColorBlue;
  int m_textureColorRed;  //sets texture/alpha color
  int m_textureColorGreen;
  int m_textureColorBlue;
  long m_worldLeft;
  long m_worldRight;
  long m_lastWorldLeft;
  long m_lastWorldTop;
  long m_worldTop;
  long m_worldBottom;
  float m_scale;
  bool m_playNewGame;
  int m_headquartersCount;

  // - gc14
  bool m_bGameOver;
  bool m_bGameWon;
  int m_gameSeconds;
  int m_gameTime;
  CTimer m_gameTimer;
  double m_deltaTime;
  bool m_bPaused;
  int m_mapSpriteNumber;
  bool m_bGridOn;
  bool m_bMapOn;
  bool m_bLegalOn;
  CMapMgr m_map;
  std::vector<CTrack> m_tracks;
  bool m_bTrackOn;
  bool m_bAdvanced;
  int m_mapLevelToPlay;
  float m_timeScale;
  long m_score;
  long m_credits;
  long m_weaponUniqueID;
  CWaveGen m_waveGen;  //manages waves and spawning

  //render damaged HQ sprite
  int m_hqX;
  int m_hqY;
  int m_hqSpriteDamaged;
  long m_projectileCount;  //<<<<<<<< testing only
  long m_cost;  //temp var
  bool m_bDisplayHelp;

  //UI and mouse tie in
  int m_mouseState;   //0 = nothing, 1 = defensive unit selected at panel, ready to be added to map
  int m_mouseValue;   //object number

  //game objects
  std::vector<CObject*> m_pObject;

  //catalog of objects
  CCSVReader m_catalog;  
  std::vector<CProjectile> m_projectile;
  int m_shadowX;
  int m_shadowY;

  //sound and music
  bool m_playSound;
  bool m_playMusic;

  //debug use only
  int m_debugx1;
  int m_debugx2;
  int m_debugy1;
  int m_debugy2;
  int m_debugvalue;
  int m_debugx;
  int m_debugy;
 
  bool m_bAttackerFiring;

  //used for displaying repair/upgrade cost of hovering object
  int m_costRepair;
  int m_costUpgrade;

  //balancing variables used for upgrades, damage calcs, etc.
  float m_balWavecountDivisor;//
  float m_balAdvFactor;//
  float m_balBegFactor;//
  float m_balCreditMult;//
  float m_balScoreMult;//
  float m_balUpgradeMult1;
  float m_balUpgradeMult2;
  float m_balUpgradeMult3;
  float m_balUpgradeCostMult1;
  float m_balUpgradeCostMult2;
  float m_balUpgradeCostMult3;
  float m_balRepairCostMult1;
  float m_balRepairCostMult2;
  float m_balRepairCostMult3;
  float m_balDamageUpper;
  float m_balDamageLower;
  float m_balanceFactor; //off/def
  float m_balanceAdjustedFactor;//adjusted
  float m_totalOffDamage;//off
  float m_totalDefDamage;//def

private:

};

#endif