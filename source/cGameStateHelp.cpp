#include "..\include\CGameStateHelp.h"

CGameStateHelp::CGameStateHelp(){
}

CGameStateHelp::~CGameStateHelp()
{
	m_TE.clear();
}

void CGameStateHelp::Initialize(){

}

void CGameStateHelp::Activate(CGameData &data, CConfigData &cfg, CGraphics &con){
  m_screenNumber = 0;
}

CGameStateObject* CGameStateHelp::Update(double timeDifference,  CGameData &data, CConfigData &cfg, CGraphics &con){
  m_event = EVENT_GO_NO_WHERE;

//  CAudioManager *pAudio = CAudioManager::Instance();

  //play background music
 // if(cfg.PlayMusic == true && pAudio->IsPlaying(4) == false)
  //  pAudio->PlayAudioClip(4, 1);

  //first time
  static bool bFirstTime = true;
  if(bFirstTime == true){
    bFirstTime = false;
    m_screenNumber = 0;
    m_arrowX = 150;
    m_arrowY = 150;
  }


  //CAudioManager *pAudio = CAudioManager::Instance();
  m_mouse.SetHandle(data.m_hWnd);


  static double screenTime = 0;
  screenTime += timeDifference;
  
  if(::GetActiveWindow() == data.m_hWnd && screenTime > 0.4 && data.m_bLeftMouseDown == true){
    
    //previous button
    if(m_mouse.MouseX() > 212 && m_mouse.MouseX() < 312 && 
      m_mouse.MouseY() > 650 && m_mouse.MouseY() < 700){
        m_screenNumber--;
        if(m_screenNumber < 0)
          m_screenNumber = 0;
    }
    //next button
    else if(m_mouse.MouseX() > 712 && m_mouse.MouseX() < 812 && 
      m_mouse.MouseY() > 650 && m_mouse.MouseY() < 700){
        m_screenNumber++;
        if(m_screenNumber > 9)
          m_screenNumber = 9;
    }
    
    screenTime = 0;
  }
  
  //animate arrow
  static double arrowTime = 0;
  arrowTime += timeDifference;
  if(arrowTime > 0.05 && m_screenNumber == 1){
    m_arrowX += 10;
    m_arrowY += 5;

    if(m_arrowX > 750){
      m_arrowX = 150;
      m_arrowY = 150;
    }    
    arrowTime = 0;
  }

  if(::GetActiveWindow() == data.m_hWnd && data.m_bLeftMouseDown == true){
    //main menu
   // if(m_mouse.MouseX() > 812 && m_mouse.MouseX() < 1012 && 
   //   m_mouse.MouseY() > 50 && m_mouse.MouseY() < 100){
      m_event = EVENT_GO_MAIN;    
   // }
  }


  for(unsigned int i=0; i< m_TE.size(); i++){
    if (m_TE[i].event == m_event){
      return m_TE[i].p_gso;
    }
  }
  return 0;
}

void CGameStateHelp::Render(CGraphics &con,  CGameData &data, CConfigData &cfg){
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
      }
      return;
    }

    //device was lost...but now can be RESET
    else if( hr == D3DERR_DEVICENOTRESET ){
      hr = con.m_d3ddev->Reset(&con.GetDPP());      
      if(SUCCEEDED(hr)){
        con.LoadAssetFile(cfg.GamePlayAssetFile);
        con.LoadAssetFile(cfg.FrameworkAssetFile);     
        con.ReloadLost();
        bLostDevice = false;
      }
      return;
    }
  }

  // clear the window to a deep blue
  con.m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
  con.m_d3ddev->BeginScene();   
  con.BeginRender();

  GRAPHIC_IMAGE gi;

  if(m_screenNumber == 0){
    gi = con.GetSprite(135);//title
    con.RenderGraphicModulate(gi, 270, 100, 255, 255, 255);
    gi = con.GetSprite(136);//opening narrative
    con.RenderGraphicModulate(gi, 110, 200, 255, 255, 255);
  }
  else if(m_screenNumber == 1){
    gi = con.GetSprite(137);//map
    con.RenderGraphicModulate(gi, 155, 50, 255, 255, 255);
    gi = con.GetSprite(143);//begin 
    con.RenderGraphicModulate(gi, 180, 75, 255, 255, 255);
    gi = con.GetSprite(140);//red circle
    con.RenderGraphicModulate(gi, 125, 70, 255, 255, 255);
    gi = con.GetSprite(138);//pleasant valley
    con.RenderGraphicModulate(gi, 5, 150, 255, 255, 255);
    gi = con.GetSprite(144);//end
    con.RenderGraphicModulate(gi, 620, 550, 255, 255, 255);
    gi = con.GetSprite(141);//blue circle
    con.RenderGraphicModulate(gi, 820, 550, 255, 255, 255);
    gi = con.GetSprite(139);//johnson city
    con.RenderGraphicModulate(gi, 840, 400, 255, 255, 255);
    gi = con.GetSprite(142);//red arrow
//    gi.angle = .52;
    con.RenderGraphicModulate(gi, m_arrowX, m_arrowY, 255, 255, 255);    
  }
  else if(m_screenNumber == 2){
    gi = con.GetSprite(145);//lay track narr
    con.RenderGraphicModulate(gi, 100, 100, 255, 255, 255);
    gi = con.GetSprite(146);//lay 1
    con.RenderGraphicModulate(gi, 112, 300, 255, 255, 255);
    gi = con.GetSprite(147);//lay 2
    con.RenderGraphicModulate(gi, 612, 150, 255, 255, 255);
  }
  else if(m_screenNumber == 3){
    gi = con.GetSprite(148);//lantern
    con.RenderGraphicModulate(gi, 317, 100, 255, 255, 255);
    gi = con.GetSprite(149);//lantern narr
    con.RenderGraphicModulate(gi, 268, 200, 255, 255, 255);
  }
  else if(m_screenNumber == 4){
    gi = con.GetSprite(150);//track sample
    con.RenderGraphicModulate(gi, 50, 250, 255, 255, 255);
    gi = con.GetSprite(151);//4 sections
    con.RenderGraphicModulate(gi, 512, 100, 255, 255, 255);
    gi = con.GetSprite(152);//4 sections
    con.RenderGraphicModulate(gi, 512, 100, 255, 255, 255);
  }
  else if(m_screenNumber == 5){
    gi = con.GetSprite(153);//terrain type narr
    con.RenderGraphicModulate(gi, 150, 200, 255, 255, 255);
    gi = con.GetSprite(154);//terrain
    con.RenderGraphicModulate(gi, 150, 375, 255, 255, 255);
    gi = con.GetSprite(161);//terrain
    con.RenderGraphicModulate(gi, 500, 200, 255, 255, 255);
  }
  else if(m_screenNumber == 6){
    gi = con.GetSprite(155);//pause
    con.RenderGraphicModulate(gi, 191, 160, 255, 255, 255);
  }
  else if(m_screenNumber == 7){
    gi = con.GetSprite(157);//multicontrols
    con.RenderGraphicModulate(gi, 278, 100, 255, 255, 255);
  }
  else if(m_screenNumber == 8){
    gi = con.GetSprite(158);//check mark
    con.RenderGraphicModulate(gi, 100, 100, 255, 255, 255);
    gi = con.GetSprite(159);//waypoint example
    con.RenderGraphicModulate(gi, 100, 380, 255, 255, 255);
    gi = con.GetSprite(160);//waypoint narr
    con.RenderGraphicModulate(gi, 250, 120, 255, 255, 255);
  }
  else if(m_screenNumber == 9){
    gi = con.GetSprite(166);//sound music directions
    con.RenderGraphicModulate(gi, 100, 100, 255, 255, 255);
    gi = con.GetSprite(169);
    con.RenderGraphicModulate(gi, 100, 200, 255, 255, 255);
  }

  //previous button
  if(m_screenNumber == 0)
    gi = con.GetSprite(132);
  else
    gi = con.GetSprite(130);
  con.RenderGraphicModulate(gi, 212, 650, 255, 255, 255);

  //next button
  if(m_screenNumber == 9)
    gi = con.GetSprite(133);
  else
    gi = con.GetSprite(131);
  con.RenderGraphicModulate(gi, 712, 650, 255, 255, 255);

  //main menu
  gi = con.GetSprite(134);
  con.RenderGraphicModulate(gi, 812, 50, 255, 255, 255);

  //mouse cursor
  gi = con.GetSprite(108);  
  con.RenderGraphicModulate(gi, m_mouse.MouseX() - gi.width/2,m_mouse.MouseY() - gi.height/2, 255, 255, 255);
  
  con.EndRender();
  con.Print("Help Screen", m_screenNumber, F_V20B, cfg.ScreenWidth/2 - 120, 50, 255, 255, 255, 255);
  

  if(cfg.DisplayDebugInfo == true){
    con.Print("Version", cfg.ProgramVersion, F_V16, 40, 730,  255, 255, 255, 255);
    con.Print("FPS", data.m_FPS , F_V16, 930, 730,  255, 255, 255, 255);
  }  

  con.m_d3ddev->EndScene();    // ends the 3D scene
  hr = con.m_d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen

}

void CGameStateHelp::ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
}

void CGameStateHelp::Deactivate(CGameData &data, CConfigData &cfg, CGraphics &con){
//  CAudioManager *pAudio = CAudioManager::Instance();
 // pAudio->KillAll();

}
void CGameStateHelp::Resume(){
}
void CGameStateHelp::Pause(){
}
void CGameStateHelp::Save(){
}

void CGameStateHelp::AddTransitionEvent(int event, CGameStateObject* p_Next){
  TRANSITION_EVENT te;
  te.event=event;
  te.p_gso = p_Next;

  m_TE.push_back(te);
}
