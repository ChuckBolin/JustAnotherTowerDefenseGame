#ifndef CPROJECTILE_H
#define CPROJECTILE_H

#include "CGameData.h"
#include "CGraphics.h"
#include "CFizzix.h"
#include "CLog.h"

struct STRUCT_PROJECTILE{
  float projectileX;
  float projectileY;
  float projectileVelocity;
  float projectileHeading;
  float weaponRange;
  float targetX;
  float targetY;
  int weaponLayer;
  int weaponSprite;
  float weaponRadius;
  int effectAtWeapon;
  int effectAtImpact;
  float damage;
  bool bAlive; 
  //float angle; //orientation of sprite
  long targetID; //for rocket use
  bool bAttacker; //true if fired by an attacker
};

class CProjectile{
public:
  CProjectile();
  CProjectile(STRUCT_PROJECTILE proj);
  ~CProjectile();
  void Update(double & timeDifference, CGameData &data);
  void Render(CGraphics &con, CGameData &data);
  void AddProjectile(STRUCT_PROJECTILE proj);
  STRUCT_PROJECTILE GetProjectileData();
  void Kill();
  bool GetStatusAlive(){return m_proj.bAlive;}
  bool m_bAlive;
  
private:
  STRUCT_PROJECTILE m_proj;
  float m_initX;
  float m_initY;
};

#endif