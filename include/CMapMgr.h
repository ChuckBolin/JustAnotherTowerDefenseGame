/****************************************************************************
  Class: CMapMgr
  Purpose: Game map is laid on a grid (32 columns x 24 rows). Each grid cell
           is 32 x 32 pixels.  There are 768 cells.
           A '1' assigned to a particular cell indicates that it is a legal
           position for the player to place a game piece.

****************************************************************************/
#ifndef CMAPMGR_H
#define CMAPMGR_H

class CMapMgr{
public:
  CMapMgr();
  ~CMapMgr();
  void ClearMap();
  void FillMap();
  void AddCells(int row, int col, int num);
  void RemoveCells(int row, int col, int num);
  bool IsLegalXY(int mx, int my);
  bool IsLegalCell(int row, int col);
  bool AddWeapon(int mx, int my, int weaponID);
  int GetWeaponInCell(int mx, int my);
  bool RemoveWeapon(int mx, int my);
  bool IsOccupiedXY(int mx, int my);
  bool IsOccupiedCell(int row, int col);
private:
  int m_map[769];
  int m_mapContents[769];
};

#endif