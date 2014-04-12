//Keystatus.cpp
#include "..\include\Keystatus.h"



CMouse::CMouse(){

}

CMouse::CMouse(HWND hWnd){
  m_hWnd = hWnd;
}

void CMouse::SetHandle(HWND hWnd){
  m_hWnd = hWnd;
}

int CMouse::MouseX(){
  POINT pt;
  POINT *ppt = &pt;
  ::GetCursorPos(ppt);
  RECT rct;
  ::GetWindowRect(m_hWnd, &rct);
  return (pt.x - rct.left);
}

int CMouse::MouseY(){
  POINT pt;
  POINT *ppt = &pt;
  ::GetCursorPos(ppt);
  RECT rct;
  ::GetWindowRect(m_hWnd, &rct);
  return (pt.y - rct.top);
}

bool keyDown(int key)
{
  if(GetAsyncKeyState(key) == 0) //key is not pressed, return false
    return false;
  else                           //key IS pressed, return true 
    return true;
}