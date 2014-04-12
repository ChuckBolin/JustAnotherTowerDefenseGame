#include "..\include\CProjectile.h"

CProjectile::CProjectile(){
}

CProjectile::CProjectile(STRUCT_PROJECTILE proj){
  m_proj = proj;
  m_bAlive = true;
}

CProjectile::~CProjectile(){
}

void CProjectile::Update(double & timeDifference, CGameData &data){
  CFizzix *pFizzix = CFizzix::Instance();
  CLog *pLog = CLog::Instance();

  //update projectile position
  m_proj.projectileX += cos(m_proj.projectileHeading) * timeDifference * m_proj.projectileVelocity;
  m_proj.projectileY += sin(m_proj.projectileHeading) * timeDifference * m_proj.projectileVelocity ;

  //weapons out of range
  if(pFizzix->CalcDistance(m_initX, m_initY, m_proj.projectileX, m_proj.projectileY) >= m_proj.weaponRange - 2 && m_proj.bAlive == true){
    m_proj.bAlive = false;
  }

  //weapons near target
  if(pFizzix->CalcDistance(m_proj.projectileX, m_proj.projectileY, m_proj.targetX, m_proj.targetY) <= m_proj.weaponRadius && m_proj.bAlive == true){
    m_proj.bAlive = false;
  }



}

void CProjectile::Render(CGraphics &con, CGameData &data){
  CLog *pLog = CLog::Instance();
  
  GRAPHIC_IMAGE gi;
  gi = con.GetSprite(m_proj.weaponSprite);
  gi.angle = m_proj.projectileHeading + 1.57; //m_proj.angle;
  con.RenderGraphicModulate(gi, m_proj.projectileX - gi.width/2, m_proj.projectileY - gi.height/2, 255, 255, 255);
}

void CProjectile::AddProjectile(STRUCT_PROJECTILE proj){
  m_proj = proj;
  m_initX = m_proj.projectileX;
  m_initY = m_proj.projectileY;
  m_bAlive = true;
}


STRUCT_PROJECTILE CProjectile::GetProjectileData(){
  return m_proj;
}

void CProjectile::Kill(){
  m_bAlive = false;
}