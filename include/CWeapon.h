#ifndef CWEAPON_H
#define CWEAPON_H

#include "CObject.h"
#include "CGraphics.h"
#include "CFizzix.h"
#include "CProjectile.h"

class CWeapon: public CObject{
public:
  CWeapon();
  CWeapon(GAME_OBJECT game_object);
  ~CWeapon();
  void Update(double timeDifference, CGameData &data);
  void Render(CGraphics &con, CGameData &data);
  void SetObject(GAME_OBJECT game_object);
  void AssignTarget(int x, int y, long id);
  void UnassignTarget();
  bool RenderEffects(int &targetX,int &targetY, int &weaponType);
  int GetLayer(){return m_go.m_layer;}
  void AssignDamage(float damage);
private:
  double m_fireTime;
  bool m_bFirstTime;
};

#endif
