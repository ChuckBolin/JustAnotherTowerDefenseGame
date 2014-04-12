#include "..\include\CMapMgr.h"


CMapMgr::CMapMgr(){
  ClearMap();
}

CMapMgr::~CMapMgr(){

}

//no cells are legal
void CMapMgr::ClearMap(){
  for (int i = 0; i < 769; i++)
    m_map[i] = 0;
}

//all cells are legal
void CMapMgr::FillMap(){
  for (int i = 0; i < 769; i++)
    m_map[i] = 1;
}

//legal rows (1..24)  cols (1..32)  num (1..32)
//plays a 1 into the cell making it a 'legal' 
//location to play offensive game pieces
void CMapMgr::AddCells(int row, int col, int num){
 if(row < 1 || row > 24) 
   return;
 if(col < 1 || col > 32)
   return;
 if(num < 1 || num > 768)
   return;

 //int cell = (row - 1) * 32 + col;
 int cell = 0;

 for(int i = 0; i < num; ++i){
   cell = (row - 1) * 32 + col + i;
   if(cell > -1 && cell < 769)
     m_map[cell] = 1;
 }
}

//legal rows (1..24)  cols (1..32)  num (1..32)
//places a 0 into the cell making it an illegal
//location to play a offensive game piece
void CMapMgr::RemoveCells(int row, int col, int num){
 if(row < 1 || row > 24) 
   return;
 if(col < 1 || col > 32)
   return;
 if(num < 1 || num > 768)
   return;

 //int cell = (row - 1) * 32 + col;
 int cell = 0;

 for(int i = 0; i < num; ++i){
   cell = (row - 1) * 32 + col + i;
   if(cell > -1 && cell < 769)
     m_map[cell] = 0;
 }
}


bool CMapMgr::IsLegalXY(int mx, int my){
  if(mx < 0 || mx > 1023)
    return false;
  if(my < 0 || my > 767)
    return false;

  int row = my / 32 + 1;
  int col = mx / 32 + 1;

  return IsLegalCell(row,col);
}

//returns true if cell is a legal place
//to play an offensive game piece
bool CMapMgr::IsLegalCell(int row, int col){
 if(row < 1 || row > 24) 
   return false;
 if(col < 1 || col > 32)
   return false;

 int cell = (row - 1) * 32 + col;

 if (m_map[cell] == 1)
   return true;
 else
   return false;
}

//adds defensive weapon
bool CMapMgr::AddWeapon(int mx, int my, int weaponID){
  
  //verify correct mx and my values
  //if(mx < 0 || mx > 1023)
  //  return false;
  //if(my < 0 || my > 767)
  //  return false;

  if(IsOccupiedXY(mx, my) == false){
    int row = my / 32 + 1;
    int col = mx / 32 + 1;
    int cell = (row - 1) * 32 + col;
    m_mapContents[cell] = weaponID;
    return true;
  }
  else
    return false;
}

int CMapMgr::GetWeaponInCell(int mx, int my){
  int row = my / 32 + 1;
  int col = mx / 32 + 1;
  if(row < 1 || row > 24) 
    return 0;
  if(col < 1 || col > 32)
    return 0;
  int cell = (row - 1) * 32 + col;
  if(m_mapContents[cell] > 0)
    return m_mapContents[cell];
  else
    return 0;  
}

bool CMapMgr::RemoveWeapon(int mx, int my){
  //verify correct mx and my values
  if(mx < 0 || mx > 1023)
    return false;
  if(my < 0 || my > 767)
    return false;

  if(IsOccupiedXY(mx, my) == true){
    int row = my / 32 + 1;
    int col = mx / 32 + 1;
    int cell = (row - 1) * 32 + col;
    m_mapContents[cell] = 0;
    return true;
  }
  else
    return false;
}

bool CMapMgr::IsOccupiedXY(int mx, int my){
  if(mx < 0 || mx > 1023)
    return false;
  if(my < 0 || my > 767)
    return false;

  int row = my / 32 + 1;
  int col = mx / 32 + 1;

  return IsOccupiedCell(row,col);
}

//returns true if cell is occupied
bool CMapMgr::IsOccupiedCell(int row, int col){
 if(row < 1 || row > 24) 
   return false;
 if(col < 1 || col > 32)
   return false;

 int cell = (row - 1) * 32 + col;

 if (m_mapContents[cell] > 0)
   return true;
 else
   return false;
}