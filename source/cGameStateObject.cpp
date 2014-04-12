#include "..\include\CGameStateObject.h"

CGameStateObject::CGameStateObject(){
}
CGameStateObject::~CGameStateObject(){
}
void CGameStateObject::Initialize(){
}
void CGameStateObject::Activate(CGameData &data, CConfigData &cfg, CGraphics &con){
}


CGameStateObject* CGameStateObject::Update(double timeDifference,  CGameData &data, CConfigData &cfg, CGraphics &con){
  return 0;

}

void CGameStateObject::Render(CGraphics &con, CGameData &data, CConfigData &cfg){

}

void CGameStateObject::ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){


}

void CGameStateObject::Deactivate(CGameData &data, CConfigData &cfg, CGraphics &con){
}
void CGameStateObject::Resume(){
}
void CGameStateObject::Pause(){
}
void CGameStateObject::Save(){
}

void CGameStateObject::AddTransitionEvent(int event, CGameStateObject* p_Next){

}
