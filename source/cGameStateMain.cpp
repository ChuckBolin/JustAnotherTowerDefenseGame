/*
  CGameStateMain.cpp - Controls the user interface and display for the main menu. 

*/
#include "..\include\CGameStateMain.h"
#include "..\include\keystatus.h"

CGameStateMain::CGameStateMain(void)
{
}

CGameStateMain::~CGameStateMain(void)
{
	m_TE.clear();
}

void CGameStateMain::Initialize(){
  m_selection = 0; 
}

void CGameStateMain::Activate(CGameData &data, CConfigData &cfg, CGraphics &con){
 m_bMoveLeft = false;
}

CGameStateObject* CGameStateMain::Update(double timeDifference,  CGameData &data, CConfigData &cfg, CGraphics &con){
  m_event = EVENT_GO_NO_WHERE;

  //first time code
  static bool bFirstTime = true;
  if(bFirstTime == true){
    bFirstTime = false;
    m_bMoveLeft = false;
    m_bMoveRight = false;
    m_bMoveStop = true;
    m_movePos = 0;
  }

  //CAudioManager *pAudio = CAudioManager::Instance();
  m_mouse.SetHandle(data.m_hWnd);
  con.m_textureRed = 155;//data.m_textureColorRed;
  con.m_textureGreen = 155;//data.m_textureColorGreen;
  con.m_textureBlue = 155;//data.m_textureColorBlue;

  //process keys
  /*
  if(::GetActiveWindow() == data.m_hWnd && keyDown(VK_RETURN) == true)
    m_event = EVENT_GO_PLAY1;
  else if(::GetActiveWindow() == data.m_hWnd && keyDown('H') == true)
    m_event = EVENT_GO_HELP;
  else if(::GetActiveWindow() == data.m_hWnd && keyDown(VK_ESCAPE) == true)
    m_event = EVENT_GO_CREDITS;
  */
  static double timeDiff = 0;
  timeDiff += timeDifference;
  static double moveTime = 0;
  moveTime += timeDifference;

  //update position
  if(moveTime > 0.05){
    if(m_bMoveLeft == true){
      m_movePos -=32;
      if (m_movePos < 1 || m_movePos % 512 == 0){
        m_bMoveLeft = false;
        m_bMoveStop = true;
      }
    }
    else if(m_bMoveRight == true){
      m_movePos +=32;
      if (m_movePos > 2047 || m_movePos % 512 == 0){
        m_bMoveRight = false;
        m_bMoveStop = true;
      }
    }
    moveTime = 0;
  }

  //mouse clicking
  //*********************************************************************************************
  if(::GetActiveWindow() == data.m_hWnd && data.m_bLeftMouseDown == true && timeDiff > 0.4 ){
    timeDiff = 0.0;
    
    //left arrow clicked
    if(m_mouse.MouseX() > 190 && m_mouse.MouseX() < 223 && m_mouse.MouseY() > 349 && m_mouse.MouseY() < 449){
      if(m_movePos > 0){
        m_bMoveLeft = true;
        m_bMoveStop = false;
      }
      else{
        m_bMoveLeft = false;
        m_bMoveStop = true;
      }
    }

    //right arrow clicked
    if(m_mouse.MouseX() > 801 && m_mouse.MouseX() < 834 && m_mouse.MouseY() > 349 && m_mouse.MouseY() < 449){
      if(m_movePos < 1536){
        m_bMoveRight = true;
        m_bMoveStop = false;
      }
      else{
        m_bMoveRight = false;
        m_bMoveStop = true;
      }
    }

    //beginner clicked
    if(m_mouse.MouseX() > 383 && m_mouse.MouseX() < 641 && m_mouse.MouseY() > 51 && m_mouse.MouseY() < 123){
      data.m_bAdvanced = false;
    }

    //advanced clicked
    if(m_mouse.MouseX() > 383 && m_mouse.MouseX() < 641 && m_mouse.MouseY() > 642 && m_mouse.MouseY() < 717){
      data.m_bAdvanced = true;
    }

    //play map 
    if(m_mouse.MouseX() > 252 && m_mouse.MouseX() < 770 && m_mouse.MouseY() > 189 && m_mouse.MouseY() < 579){
      if(m_movePos < 512){
        data.m_mapLevelToPlay = 1;
        m_event = EVENT_GO_PLAY1;
      }
      else if(m_movePos >= 512 && m_movePos < 1024){
        data.m_mapLevelToPlay = 2;
        m_event = EVENT_GO_PLAY1;
      }
      else if(m_movePos >= 1024 && m_movePos < 1536){
        data.m_mapLevelToPlay = 3;
        m_event = EVENT_GO_PLAY1;
      }
      else if(m_movePos >= 1536){
        data.m_mapLevelToPlay = 4;
        m_event = EVENT_GO_PLAY1;
      }
    }

    //exit game
    if(m_mouse.MouseX() > 50 && m_mouse.MouseX() < 215 && m_mouse.MouseY() > 568 && m_mouse.MouseY() < 682){
      m_event = EVENT_GO_CREDITS;
    }
  }

  for(unsigned int i=0; i< m_TE.size(); i++){
    if (m_TE[i].event == m_event){
        return m_TE[i].p_gso;
    }
  }

  return 0;
}

void CGameStateMain::Render(CGraphics &con,  CGameData &data, CConfigData &cfg){
  CLog *pLog = CLog::Instance();
  static bool bLostDevice = false;

  ::SetCursor(false);

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
  con.m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(106, 126, 67), 1.0f, 0);
  con.m_d3ddev->BeginScene();    // begins the 3D scene
  con.BeginRender();

  GRAPHIC_IMAGE gi;
  
  //render small map(s)
  if(m_movePos >= 0 &&  m_movePos < 512){
    gi = con.GetSprite(1004);
    con.RenderGraphicModulate(gi, 256, con.GetScreenCenterY() - gi.height/2,255, 255, 255);

    gi = con.GetSprite(1003);
    gi.sourceX += m_movePos;
    gi.width -= m_movePos;
    con.RenderGraphicModulate(gi, 256, con.GetScreenCenterY() - gi.height/2,255, 255, 255);
  }
  else if(m_movePos >= 512 &&  m_movePos < 1024){
    if(m_bMoveRight == true){
      gi = con.GetSprite(1005);
      con.RenderGraphicModulate(gi, 256, con.GetScreenCenterY() - gi.height/2,255, 255, 255);
    }
    else if(m_bMoveLeft == true){
      gi = con.GetSprite(1005);
      con.RenderGraphicModulate(gi, 256, con.GetScreenCenterY() - gi.height/2,255, 255, 255);
    }

    gi = con.GetSprite(1004);
    gi.sourceX += m_movePos - 512;
    gi.width -= m_movePos - 512;
    con.RenderGraphicModulate(gi, 256, con.GetScreenCenterY() - gi.height/2,255, 255, 255);
  }
  else if(m_movePos >= 1024 &&  m_movePos < 1536){
    if(m_bMoveRight == true){
      gi = con.GetSprite(1006);
      con.RenderGraphicModulate(gi, 256, con.GetScreenCenterY() - gi.height/2,255, 255, 255);
    }
    else if(m_bMoveLeft == true){
      gi = con.GetSprite(1006);
      con.RenderGraphicModulate(gi, 256, con.GetScreenCenterY() - gi.height/2,255, 255, 255);
    }

    gi = con.GetSprite(1005);
    gi.sourceX += m_movePos - 1024;
    gi.width -= m_movePos - 1024;
    con.RenderGraphicModulate(gi, 256, con.GetScreenCenterY() - gi.height/2,255, 255, 255);
  }
  else if(m_movePos >= 1536){

    gi = con.GetSprite(1006);
    gi.sourceX += m_movePos - 1536;//536;
    gi.width -= m_movePos - 1536;
    con.RenderGraphicModulate(gi, 256, con.GetScreenCenterY() - gi.height/2,255, 255, 255);
  }

  //render map outline
  gi = con.GetSprite(1401);
  con.RenderGraphicModulate(gi, 234, con.GetScreenCenterY() - gi.height/2, 255, 255, 255);

  //arrows
  if(m_movePos < 1){
    gi = con.GetSprite(1007); //left gray
    con.RenderGraphicModulate(gi, 200, con.GetScreenCenterY() - gi.height/2,255, 255, 255);
  }
  else{
    gi = con.GetSprite(1008); //left red
    con.RenderGraphicModulate(gi, 200, con.GetScreenCenterY() - gi.height/2,255, 255, 255);
  }
  if(m_movePos > 1535){ //2047){
    gi = con.GetSprite(1010); //right gray
    con.RenderGraphicModulate(gi, 811, con.GetScreenCenterY() - gi.height/2,255, 255, 255);
  }
  else{
    gi = con.GetSprite(1009); //right red
    con.RenderGraphicModulate(gi, 811, con.GetScreenCenterY() - gi.height/2,255, 255, 255);
  }

  //add beginner advanced labels
  gi = con.GetSprite(1012);
  con.RenderGraphicModulate(gi, con.GetScreenCenterX() - gi.width/2, 50, 255, 255, 255);
  gi = con.GetSprite(1013);
  con.RenderGraphicModulate(gi, con.GetScreenCenterX() - gi.width/2, 643, 255, 255, 255);
  
  //stars
  gi = con.GetSprite(1014);
  if(data.m_bAdvanced == false)
    con.RenderGraphicModulate(gi, con.GetScreenCenterX() - gi.width/2, 50, 255, 255, 255);
  else
    con.RenderGraphicModulate(gi, con.GetScreenCenterX() - gi.width/2, 643, 255, 255, 255);

  //click to play sprite
  gi = con.GetSprite(1015);
  con.RenderGraphicModulate(gi, con.GetScreenCenterX() - gi.width/2, 370, 255, 255, 255);

  //exit game
  gi = con.GetSprite(1016);
  con.RenderGraphicModulate(gi, 50, 568, 255, 255, 255);

  //instructions
if(cfg.DisplayDebugInfo == true){
  con.Print("Mouse X", m_mouse.MouseX(), F_V16, 10, 30, 255, 255, 255, 255);
  con.Print("Mouse Y", m_mouse.MouseY(), F_V16, 10, 50, 255, 255, 255, 255);
  con.Print("MovePos", data.m_mapLevelToPlay, F_V16, 10, 70, 255, 255, 255, 255);
}
 
  con.EndRender();

  //draws outlines around arrows
  if(m_mouse.MouseX() > 190 && m_mouse.MouseX() < 223 & m_mouse.MouseY() > 349 & m_mouse.MouseY() < 449){
    con.DrawRect(190, 349, 223,420,0, 255, 0);
  }
  if(m_mouse.MouseX() > 801 && m_mouse.MouseX() < 834 & m_mouse.MouseY() > 349 & m_mouse.MouseY() < 449){
    con.DrawRect(801, 349, 834, 420,0, 255, 0);
  }

  //beginner 
  if(m_mouse.MouseX() > 323 && m_mouse.MouseX() < 701 && m_mouse.MouseY() > 51 && m_mouse.MouseY() < 123){
    con.DrawRect(323, 35, 701, 139, 0, 255, 0);
  }

  //advanced 
  if(m_mouse.MouseX() > 323 && m_mouse.MouseX() < 701 && m_mouse.MouseY() > 642 && m_mouse.MouseY() < 717){
    con.DrawRect(323, 624, 701, 736, 0, 255, 0);
  }
 
  //draws outline around small map
  //con.DrawRect(252, 189, 770, 579, 255, 255, 255);
  if(m_mouse.MouseX() > 252 && m_mouse.MouseX() < 770 && m_mouse.MouseY() > 189 && m_mouse.MouseY() < 579){
    con.DrawRect(250, 187, 772, 581, 0, 255, 0);
  }

  //draws outline around exit game
  if(m_mouse.MouseX() > 50 && m_mouse.MouseX() < 215 && m_mouse.MouseY() > 568 && m_mouse.MouseY() < 682){
    con.DrawRect(50, 560, 215, 682, 255, 255, 255);
  }


  //draws outline around click to play
  //if(m_mouse.MouseX() > 306 && m_mouse.MouseX() < 716 && m_mouse.MouseY() > 368 && m_mouse.MouseY() < 561){
  //  con.DrawRect(306, 368, 716, 561, 0, 255, 0);
  //}
  
  //draw mouse cursor
  //draw mouse
  con.DrawCircle(m_mouse.MouseX(), m_mouse.MouseY(), 255, 255, 255, 5);  
  con.DrawRect(m_mouse.MouseX() - 16, m_mouse.MouseY() - 16, m_mouse.MouseX() + 16, m_mouse.MouseY() + 16, 255, 255, 255);

  con.m_d3ddev->EndScene();    // ends the 3D scene
  hr = con.m_d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}

void CGameStateMain::ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
}

void CGameStateMain::Deactivate(CGameData &data, CConfigData &cfg, CGraphics &con){

}
void CGameStateMain::Resume(){
}
void CGameStateMain::Pause(){
}
void CGameStateMain::Save(){
}

void CGameStateMain::AddTransitionEvent(int event, CGameStateObject* p_Next){
  TRANSITION_EVENT te;
  te.event=event;
  te.p_gso = p_Next;

  m_TE.push_back(te);
}