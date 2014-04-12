#include "..\include\CGameStateIntro.h"

CGameStateIntro::CGameStateIntro(void){
}

CGameStateIntro::~CGameStateIntro(void)
{
	m_TE.clear();
}

void CGameStateIntro::Initialize(){
  m_timer.Initialize();
  m_secondCounter = 0;
}

void CGameStateIntro::Activate(CGameData &data, CConfigData &cfg, CGraphics &con){
  CLog *pLog = CLog::Instance();

  //pyro setup
  //**********************
  m_effect.Init(con.m_d3ddev);
  m_effect.LoadFile(cfg.EffectsFileI);//"assets\\data\\effects_i.dat");
  m_effectNumber = 0;
  m_effectMax = m_effect.GetNumberEffects();
  pLog->Log("******* Number of Intro Screen effects loaded", m_effectMax);

  m_startTime = ::timeGetTime();
}

CGameStateObject* CGameStateIntro::Update(double timeDifference,  CGameData &data, CConfigData &cfg, CGraphics &con){
  m_event = EVENT_GO_NO_WHERE;
  m_mouse.SetHandle(data.m_hWnd);//needed for mouse

  //initialize data first time this state is called
  static bool bFirstTime = true;
  if(bFirstTime == true){
    bFirstTime = false;
  }
  static double counterTime = 0;
  counterTime += timeDifference;

  if(counterTime < 3.0){// || counterTime > 2.3){
    con.m_textureRed = 155;//data.m_textureColorRed;
    con.m_textureGreen = 155;//data.m_textureColorGreen;
    con.m_textureBlue = 155;//data.m_textureColorBlue;
  }
  else if(counterTime < 3.3){
    con.m_textureRed = 55;//data.m_textureColorRed;
    con.m_textureGreen = 55;//data.m_textureColorGreen;
    con.m_textureBlue = 55;//data.m_textureColorBlue;
  }
  else if(counterTime >= 4.2){// || counterTime > 2.3){
    con.m_textureRed = 155;//data.m_textureColorRed;
    con.m_textureGreen = 155;//data.m_textureColorGreen;
    con.m_textureBlue = 155;//data.m_textureColorBlue;
  }

  static bool bEffects = false;
  if(counterTime > 3 && counterTime < 3.1 && bEffects == false){
    m_effectNumber = 0;
    m_effect.Add(m_effectNumber, 266, 365, 1.2);
    m_effect.Add(m_effectNumber, 411, 327, 0.5);
    m_effect.Add(m_effectNumber, 363, 390, 0.6);
    m_effect.Add(m_effectNumber, 498, 359, 0.5);
    
    m_effect.Add(m_effectNumber, 496, 434, 0.5);
    m_effect.Add(m_effectNumber, 207, 484, 0.7);
    m_effect.Add(m_effectNumber, 301, 492, 0.5);
    m_effect.Add(m_effectNumber, 416, 467, 0.6);
    m_effect.Add(m_effectNumber, 614, 427, 1.6);

    m_effect.Add(m_effectNumber, 265, 565, 0.5);
    m_effect.Add(m_effectNumber, 374, 584, 0.6);
    m_effect.Add(m_effectNumber, 515, 560, 0.5);
    m_effect.Add(m_effectNumber, 629, 597, 0.6);
    bEffects = true;
  }

  if(counterTime > 3.2 && bEffects == true){
    m_effectNumber = 1;
    m_effect.Add(m_effectNumber, 266, 365, 1.3);
    m_effect.Add(m_effectNumber, 411, 327, 1);
    m_effect.Add(m_effectNumber, 363, 390, 1);
    m_effect.Add(m_effectNumber, 498, 359, 1);
    
    m_effect.Add(m_effectNumber, 496, 434, 1);
    m_effect.Add(m_effectNumber, 207, 484, 1);
    m_effect.Add(m_effectNumber, 301, 492, 1);
    m_effect.Add(m_effectNumber, 416, 467, 1.3);
    m_effect.Add(m_effectNumber, 614, 427, 1.6);

    m_effect.Add(m_effectNumber, 265, 565, 1);
    m_effect.Add(m_effectNumber, 374, 584, 1);
    m_effect.Add(m_effectNumber, 515, 560, 1);
    m_effect.Add(m_effectNumber, 629, 597, 1);
    bEffects = false;
  }

  if(counterTime > 3.3 && counterTime < 4.3)
    m_secondCounter = 3;
  if(counterTime > 4.3)
    m_secondCounter = 4;
  if(counterTime > 6.5)
    m_secondCounter = 7;

  //JUMPER to advanced quickly to state main
  //**************************************************************************
  //m_secondCounter = 5;

  /*
  CAudioManager *pAudio = CAudioManager::Instance();
  if(cfg.PlayMusic == true && pAudio->IsPlaying(0) == false)
    pAudio->PlayAudioClip(0, 1);
  */

  //left click to continue
  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< bypassed
  if(::GetActiveWindow() == data.m_hWnd && (data.m_bLeftMouseDown == true || m_secondCounter == 7))
    //m_event = EVENT_GO_PLAY1;
    m_event = EVENT_GO_MAIN;

  m_effect.Update(m_startTime);

  for(unsigned int i=0; i< m_TE.size(); i++){
    if (m_TE[i].event == m_event){
      return m_TE[i].p_gso;
    }
  }
  return 0;
}

void CGameStateIntro::Render(CGraphics &con, CGameData &data, CConfigData &cfg){
  ::ShowCursor(false);
  CLog *pLog = CLog::Instance();
  static bool bLostDevice = false;

  //test to see if graphic device is lost, if so, deal with it
  //**********************************************************
  HRESULT hr;
  if(FAILED(hr = con.m_d3ddev->TestCooperativeLevel())){
    
    //device is lost...cannot be reset...exit Render() function
    if(hr == D3DERR_DEVICELOST){
      if(bLostDevice == false){
        bLostDevice = true;
        con.DeleteLost();    //delete one time per failure
        m_effect.Delete();
        pLog->Log("Deleted");
      }
      return;
    }

    //device was lost...but now can be RESET
    else if( hr == D3DERR_DEVICENOTRESET ){
      hr = con.m_d3ddev->Reset(&con.GetDPP());      
      if(SUCCEEDED(hr)){
        m_effect.Init(con.m_d3ddev);
        m_effect.LoadFile(cfg.EffectsFileI);// "assets\\data\\effects_i.dat");
        m_effectNumber = 0;
        m_effectMax = m_effect.GetNumberEffects();
        pLog->Log("******* Number of effects reloaded", m_effectMax);
        con.LoadAssetFile(cfg.GamePlayAssetFile);
        con.LoadAssetFile(cfg.FrameworkAssetFile);     
        con.ReloadLost();
        bLostDevice = false;
      }
      return;
    }
  }

  // clear the window to a deep blue
  //if(m_graphic == 0)
  //  con.m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 111, 255), 1.0f, 0);
  //else
  con.m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(106, 126, 67), 1.0f, 0);
  con.m_d3ddev->BeginScene();    // begins the 3D scene

  GRAPHIC_IMAGE gi;  
  con.BeginRender();
  
  //render title
  if(m_secondCounter < 3){
    gi = con.GetSprite(1000);
    con.RenderGraphicModulate(gi, con.GetScreenCenterX() - gi.width/2, con.GetScreenCenterY() - gi.height/2, 255, 255, 255);
  }
  else{
    gi = con.GetSprite(1001);
    con.RenderGraphicModulate(gi, con.GetScreenCenterX() - gi.width/2, con.GetScreenCenterY() - gi.height/2, 255, 255, 255);
  }
  if(m_secondCounter > 3){
    gi = con.GetSprite(1002);
    con.RenderGraphicModulate(gi, - 20 + con.GetScreenCenterX() - gi.width/2, 60 + con.GetScreenCenterY() - gi.height/2, 255, 255, 255);
  }

  con.EndRender();

  m_effect.Render(); 

  //mouse cursor
  con.DrawCircle(m_mouse.MouseX(), m_mouse.MouseY(), 255, 255, 255, 10);

  if(cfg.DisplayDebugInfo == true){
    con.Print("Mouse X", m_mouse.MouseX(), F_V16, 10, 30, 255, 255, 255, 255);
    con.Print("Mouse Y", m_mouse.MouseY(), F_V16, 10, 50, 255, 255, 255, 255);
  }

  //con.Print("Introduction Splash Screen", F_V20B, cfg.ScreenWidth/2 - 120, 100, 255, 255, 0, 255);
  //con.Print("Left Click to Continue", F_V20B, cfg.ScreenWidth/2 - 110, 130, 255, 255, 255, 255);
  //con.Print("FPS", data.m_FPS, F_V16, 30, 50, 255, 255, 255, 255);
  //if(cfg.DisplayDebugInfo == true){
   // con.Print("FPS", data.m_FPS , F_V16, 30, 30,  255, 255, 255, 255);
  //}  

  con.m_d3ddev->EndScene();    // ends the 3D scene
  hr = con.m_d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
 
  return;
}

void CGameStateIntro::ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
}

void CGameStateIntro::Deactivate(CGameData &data, CConfigData &cfg, CGraphics &con){
}
void CGameStateIntro::Resume(){
}
void CGameStateIntro::Pause(){
}
void CGameStateIntro::Save(){
}

void CGameStateIntro::AddTransitionEvent(int event, CGameStateObject* p_Next){
  TRANSITION_EVENT te;
  te.event=event;
  te.p_gso = p_Next;

  m_TE.push_back(te);
}