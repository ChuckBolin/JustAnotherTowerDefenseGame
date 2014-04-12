//CObject.h
//base class...all other game states are derived from here
#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include <time.h>
#include "CTrack.h"

//forward initialization
class CGraphics;
class CGameData;
class CObject;

struct GAME_OBJECT{
  float m_posX;          //center position of weapon
  float m_posY;
  float m_heading;
  float m_angle;         //angle of gun
  float m_angleRate;     //rate gun angle changes
  float m_maxAmmo;       //amount loaded from catalog
  int   m_ammo;          //number of rounds on hand
  float m_rateOfFire;    //time interval between rounds
  float m_health;        //health of weapon remaining
  float m_maxHealth;
  float m_damage;        //damage caused by weapon
  float m_maxDamage;
  int   m_range;         //range of weapon...radius around weapon
                         //impact radius around target is 1/20th m_range
  int   m_sprite;        //-1 if it doesn't exist
  int   m_spriteGun;     //idea of gun
  int   m_platformType;  //0=stationary gun, 1=mobile gun, 2 = rocket, 3=flying
  float m_velocity;
  float m_maxVelocity;   //speed (px/s) 
  int   m_cost;          //cost in credits
  int   m_weaponType;
  int   m_layer;
  bool  m_bDead;
  bool  m_bAttacker;     //true if attacker, false if defender
  double animTime;
  int frameCount;
  int   m_score;
  int m_targetAir;       //1 means it can attack airplanes and rockets
  int m_targetSeaLand;   //1 means it can attack ships and land     
  bool m_bHeadingDirection;

  //effects
  int   m_weaponEffect;  //fire effect at weapon
  int   m_weaponSmoke;   //smoke effect at weapon
  int   m_impactEffect;  //fire effect at impact
  int   m_impactSmoke;   //smoke effect at impact

  //projectile data
  int m_weaponSprite;
  float m_weaponVelocity;
  int m_weaponLayer;
  float m_weaponRadius;

  //upgrade and repair information
  int m_upgradeRating;
  //float m_repairCost;     //calculated
  //float m_upgradeCost1;
  //float m_upgradeCost2;
  //float m_upgradeCost3;
  
  //unique id
  long m_id;
  CTrack m_track;
}; 

class CObject{
public:
  virtual void Update(double timeDifference, CGameData &data) = 0;
  virtual void Render(CGraphics &con, CGameData &data) = 0;
  virtual void SetObject(GAME_OBJECT game_object) = 0;
  virtual void AssignTarget(int x, int y, long id) = 0;
  virtual void UnassignTarget() = 0;
  virtual bool RenderEffects(int &targetX,int &targetY, int &weaponType) = 0;
  virtual int GetLayer() = 0;
  virtual void AssignDamage(float damage) = 0;
  //virtual int GetSpriteID() = 0;
  //virtual void Hit() = 0;
  //virtual void Initialize(int type, float x, float y, float angle, float velocity, int layer, int *arr) = 0;
  

  GAME_OBJECT m_go;
  int m_targetX;
  int m_targetY;
  bool m_renderEffects;

  

  //CWaypoint m_wp; //waypoints

  /*
  virtual void Turn(float value);
  virtual void Move(float value);  
  virtual void RotateTurret(float value);
  */
};

#endif 