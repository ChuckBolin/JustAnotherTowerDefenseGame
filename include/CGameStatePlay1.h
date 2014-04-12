#ifndef GAME_STATE_PLAY1_H
#define GAME_STATE_PLAY1_H

#include <iomanip>
#include <sstream>
#include <string>
#include "CGameStateObject.h"
#include "CGraphics.h"
#include "CTimer.h"
#include "keystatus.h"
#include "CGameData.h"
#include "CLog.h"
#include "CEffect.h"
#include "CObject.h"
//#include "CImpact.h"
#include "CWeapon.h"
#include "CFizzix.h"
#include "CAudioManager.h"
#include "CGameClock.h"
#include "CUserInterface.h"

//class CUserInterface;

class CGameStatePlay1 :  public CGameStateObject{
public:
  CGameStatePlay1(void);
  ~CGameStatePlay1(void);
  void Initialize();
  void Activate(CGameData &data, CConfigData &cfg, CGraphics &con);
  CGameStateObject* Update(double timeDifference,  CGameData &data, CConfigData &cfg, CGraphics &con);
  void Render(CGraphics &con,  CGameData &data, CConfigData &cfg);
  void ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
  void Deactivate(CGameData &data, CConfigData &cfg, CGraphics &con);
  void Resume();
  void Pause();
  void Save();
  void AddTransitionEvent(int event, CGameStateObject* p_Next);

private:

  std::vector<TRANSITION_EVENT> m_TE; //stores all transition events
  CTimer m_timer;
  int m_event;
  CMouse m_mouse;
  CUserInterface m_UI;
  int m_UIstate;
  //members for this game
  //bool m_bMapOn;
  //bool m_bGridOn;
  
  //************** old methods and members

  //added for BFK
  //float m_zoom;  //0.125, 0.25, 0.5, 1.0
  float m_zoomValue[4];
  int m_zoomIndex;
  //MANAGES pyro effects
  CEffect m_effect;
  DWORD m_startTime;
  int m_effectNumber;
  int m_effectMax;

  //time experiment
  bool m_bReadyToFire;

  bool m_bReadyToRender;
  bool m_bPlayerHasFired;
  GRAPHIC_IMAGE giMarker;
  bool m_bPaused;
  int m_visibleObjectCount;
  int m_visibleTerrainCount;
  bool m_bMouseTarget; //true if mouse cursor is target
};

#endif
