#include "..\include\CWeapon.h"

CWeapon::CWeapon(){
  m_renderEffects = false;
  m_go.m_bDead = false;
  m_fireTime = 0;
  m_go.m_upgradeRating = -1;
  m_go.m_bHeadingDirection = true;
  m_go.m_heading = 0;
  m_bFirstTime = true;
  m_go.m_angle = (float)(rand() % 628) * 0.01f;
}

CWeapon::CWeapon(GAME_OBJECT game_object){
  m_go = game_object;
  m_renderEffects = false;
  m_targetX = -1;
  m_targetY = -1;
  m_fireTime = 0;
  m_go.m_upgradeRating = -1;
  m_go.m_bHeadingDirection = true;
  m_go.m_heading = 0;
  m_bFirstTime = true;
  m_go.m_angle = (float)(rand() % 628) * 0.01f;
}

CWeapon::~CWeapon(){
}

void CWeapon::Update(double timeDifference, CGameData &data){
  CFizzix *pFizzix = CFizzix::Instance();
  CLog *pLog = CLog::Instance();

  //check health and kill 
  if(m_go.m_health <= 0){ // && m_go.m_platformType > -1){ //don't kill HQ
    m_go.m_health = 0;  //see render for rendering of health bar
    m_go.m_bDead = true;     //flag used for deletion
    //if(m_go.m_bAttacker == true)
    //  data.IncrementCredits(m_go.m_maxHealth);
  }

  //track assigned target
  float distance = 0;
  bool bOkToFire = false;  //prevent firing unless aimed at target
  if(m_targetX > -1 && m_targetY > -1 && m_go.m_platformType > -1){
    distance = pFizzix->CalcDistance(m_go.m_posX, m_go.m_posY, m_targetX, m_targetY);
    if(distance < m_go.m_range){
      float angle = pFizzix->CalcGraphAngle2(m_go.m_posX, m_go.m_posY, m_targetX, m_targetY) + 1.5708;
      float delta = angle - m_go.m_angle;
  
      //gun turret and target are aligned
      if(data.m_bGameOver == false)
        bOkToFire = true;
      if(abs(delta) < 3.14){      
        if(m_go.m_angle < angle - 0.010)
          m_go.m_angle += m_go.m_angleRate;// * timeDifference;
        else if(m_go.m_angle > angle + 0.010)
          m_go.m_angle -= m_go.m_angleRate;// * timeDifference;
      }
      else{  //gun angle and target angle are on either side of the +X axis
        if(delta > 0){ 
          angle -= 6.28;
        }
        else if(delta < 0)
          angle += 6.28;

        //if(m_go.m_weaponType == 11 || m_go.m_weaponType == 12){
          //if(m_go.m_angle < angle - 0.05){
          //  m_go.m_angle += m_go.m_heading;// .m_angleRate;
          //}
          //else if(m_go.m_angle > angle + 0.05){
          //  m_go.m_angle -= m_go.m_angleRate;
          //}
        //}
        //else{
          if(m_go.m_angle < angle - 0.05){
            m_go.m_angle += m_go.m_angleRate;
          }
          else if(m_go.m_angle > angle + 0.05){
            m_go.m_angle -= m_go.m_angleRate;
          }
        //}
      }
    }
  }

  //update motion
  if(m_go.m_bAttacker == true && m_go.m_platformType > -1){
    TRACK_WAYPOINT currWaypoint;
    TRACK_WAYPOINT nextWaypoint;
    float vel = m_go.m_velocity * timeDifference;
    if(m_go.m_velocity > 0){
      if(m_go.m_track.GetCurrentWayPoint(currWaypoint) == true &&
        m_go.m_track.GetNextWayPoint(nextWaypoint) == true){      
        m_go.m_heading = pFizzix->CalcGraphAngle2(m_go.m_posX, m_go.m_posY, nextWaypoint.x, nextWaypoint.y);
        m_go.m_posX += vel *  cos(m_go.m_heading);
        m_go.m_posY += vel *  sin(m_go.m_heading);

        //select next waypoint counter
        float distanceToCenter = pFizzix->CalcDistance(m_go.m_posX, m_go.m_posY, nextWaypoint.x, nextWaypoint.y);
        if(distanceToCenter <= nextWaypoint.radius){
          m_go.m_track.IncrementWaypoint();
          if(m_go.m_track.IsLastPoint() == true)
            m_go.m_bDead = true;
        }
      }
    }
  }

  //firing weapon
  m_fireTime += timeDifference;
  CProjectile tempProj;
  STRUCT_PROJECTILE proj;
  float deltaJet = 0;

  if(bOkToFire == true && m_fireTime >= m_go.m_rateOfFire && m_targetX > -1 && m_targetY > -1){ //m_go.m_rateOfFire
    m_fireTime = 0;  
    if(data.m_bAttackerFiring == true && distance < m_go.m_range && m_bFirstTime == false){
      
      proj.bAlive = true;
      proj.damage = m_go.m_damage;
      proj.targetX = m_targetX;
      proj.targetY = m_targetY;
      proj.projectileX = m_go.m_posX;
      proj.projectileY = m_go.m_posY;
      proj.projectileVelocity = m_go.m_weaponVelocity;
      proj.weaponSprite = m_go.m_weaponSprite;
      proj.weaponLayer = m_go.m_weaponLayer;
      proj.weaponRadius = m_go.m_weaponRadius;
      proj.effectAtWeapon = m_go.m_weaponEffect;
      proj.effectAtImpact = m_go.m_weaponEffect;
      proj.weaponRange = m_go.m_range;
      proj.targetID = -1;  //need to resolve later
      proj.bAttacker = m_go.m_bAttacker;
            

      //process jet firing
      if(m_go.m_weaponType == 11 || m_go.m_weaponType == 12){
        deltaJet = m_go.m_angle - m_go.m_heading;
        if(deltaJet > 0){ 
          m_go.m_angle -= 6.28;
        }
        else if(deltaJet < 0){
          m_go.m_angle += 6.28;
        }
        
        //deltaJet = m_go.m_angle - m_go.m_heading;
        //if(abs(deltaJet) > 0.73)
        //  bOkToFire = false;
        
        //if(bOkToFire == true && abs(deltaJet) < 1.57f){
          //m_go.m_angle = m_go.m_heading;
        if(abs(deltaJet) > 2.8f){            
          proj.projectileHeading = m_go.m_angle - 1.57;
          tempProj.AddProjectile(proj);
          data.m_projectile.push_back(tempProj);
          data.m_projectileCount+= 1;
        }
        //}

      }

      //purpose is to get FLAK to fire two bursts or rounds spread out
      else if(m_go.m_weaponType == 5){ 
        proj.projectileHeading = m_go.m_angle - 1.52;// -0.78;
        tempProj.AddProjectile(proj);
        data.m_projectile.push_back(tempProj);
        proj.projectileHeading = m_go.m_angle - 1.57;// +0.78;
        tempProj.AddProjectile(proj);
        data.m_projectile.push_back(tempProj);
        proj.projectileHeading = m_go.m_angle - 1.62;// +0.78;
        tempProj.AddProjectile(proj);
        data.m_projectile.push_back(tempProj);
        data.m_projectileCount+= 3;
      }
      //battleship - 3 rounds
      else if(m_go.m_weaponType == 10){        
        proj.projectileHeading = m_go.m_angle - 1.57;
        tempProj.AddProjectile(proj);
        data.m_projectile.push_back(tempProj);
        float dx = cos(proj.projectileHeading + 1.57) * 10.0f;
        float dy = sin(proj.projectileHeading - 1.57) * 10.0f;
        proj.projectileX = m_go.m_posX + dx;
        proj.projectileY = m_go.m_posY - dy;        
        tempProj.AddProjectile(proj);
        data.m_projectile.push_back(tempProj);      
        proj.projectileX = m_go.m_posX - dx;
        proj.projectileY = m_go.m_posY + dy;        
        tempProj.AddProjectile(proj);
        data.m_projectile.push_back(tempProj);
        data.m_projectileCount+= 3;
      }
      //cruiser - 2 rounds
      else if(m_go.m_weaponType == 9){        
        proj.projectileHeading = m_go.m_angle - 1.57;
        float dx = cos(proj.projectileHeading + 1.57) * 5.0f;
        float dy = sin(proj.projectileHeading - 1.57) * 5.0f;
        proj.projectileX = m_go.m_posX + dx;
        proj.projectileY = m_go.m_posY - dy;        
        tempProj.AddProjectile(proj);
        data.m_projectile.push_back(tempProj);      
        proj.projectileX = m_go.m_posX - dx;
        proj.projectileY = m_go.m_posY + dy;        
        tempProj.AddProjectile(proj);
        data.m_projectile.push_back(tempProj);
        data.m_projectileCount+= 2;
      }
      else{
        proj.projectileHeading = m_go.m_angle - 1.57;
        tempProj.AddProjectile(proj);
        data.m_projectile.push_back(tempProj);
        data.m_projectileCount+= 1;
      }
    
      m_renderEffects = true;      
    }
    m_bFirstTime = false;
  }

  //defensive head quarters...controls speed or rotation
  if(m_go.m_platformType == -1){
    float rate = 0;
    if(m_go.m_bHeadingDirection == true){ //clockwise
      rate = timeDifference * 0.5;//m_go.m_angleRate;
      m_go.m_heading += rate;
      if(m_go.m_heading > 0.78){
         m_go.m_heading = 0.78;
        m_go.m_bHeadingDirection = false;
      }
    }
    else{ //counter clockwise
      rate = timeDifference * 0.5;//m_go.m_angleRate;
      m_go.m_heading -= rate;
      if(m_go.m_heading < -0.78){
        m_go.m_heading = -0.78;
        m_go.m_bHeadingDirection = true;
      }
    }
  }
}

void CWeapon::Render(CGraphics &con, CGameData &data){
  GRAPHIC_IMAGE gi;
  float height = 0;
  int shadowX = 2;
  int shadowY = 2;
  if(m_go.m_weaponType == 11 || m_go.m_weaponType == 12){
    shadowX = 40;
    shadowY = 40;
  }

  //headquarters
  if(m_go.m_platformType == -1){
    gi = con.GetSprite(m_go.m_sprite); //healthy headerquarters
    gi.angle = m_go.m_heading;
    con.RenderGraphicModulate(gi, m_go.m_posX - gi.width/2, m_go.m_posY - gi.height/2, 255, 255, 255);
    height = gi.height;
  }

  //draw weapon
  else{
    //sprite of body
    if(m_go.m_sprite > - 1){
      //shadow
      gi = con.GetSprite(m_go.m_sprite + 200);
      gi.angle = m_go.m_heading + 1.57;
      gi.alpha = 100;
      if(m_go.m_weaponType == 11 || m_go.m_weaponType == 12)
        gi.scale = 0.9;
      //if(gi.frameMax > 1)
        
      con.RenderGraphicModulate(gi, m_go.m_posX - gi.width/2 + shadowX, m_go.m_posY - gi.height/2 + shadowY, 255, 255, 255);
    
      //main body
      gi = con.GetSprite(m_go.m_sprite);
      gi.angle = m_go.m_heading + 1.57;
      gi.alpha = 255;
      gi.frameCount = m_go.frameCount;
      con.RenderGraphicModulate(gi, m_go.m_posX - gi.width/2, m_go.m_posY - gi.height/2, 255, 255, 255);
    }
    float width = gi.width; //width of main body sprite used below for health bar
    height = gi.height; //height
    
    //sprite gun
    if(m_go.m_spriteGun > -1){
      //turret shadow
      gi = con.GetSprite(m_go.m_spriteGun + 200);
      gi.angle = m_go.m_angle;
      gi.alpha = 100;
      con.RenderGraphicModulate(gi, m_go.m_posX - gi.width/2 + shadowX, m_go.m_posY - gi.height/2 + shadowY, 255, 255, 255);

      gi = con.GetSprite(m_go.m_spriteGun);
      gi.angle = m_go.m_angle;
      con.RenderGraphicModulate(gi, m_go.m_posX - gi.width/2, m_go.m_posY - gi.height/2, 255, 255, 255);
    }
  }

  //render upgrade sprite symbols
  gi = con.GetSprite(122);
  if(m_go.m_upgradeRating == 1 || m_go.m_upgradeRating == 2 || m_go.m_upgradeRating == 3)
    con.RenderGraphicModulate(gi, m_go.m_posX, m_go.m_posY, 255, 255, 255);
  if(m_go.m_upgradeRating == 2 || m_go.m_upgradeRating == 3)
    con.RenderGraphicModulate(gi, m_go.m_posX - 7, m_go.m_posY - 7, 255, 255, 255);
  if(m_go.m_upgradeRating == 3)
    con.RenderGraphicModulate(gi, m_go.m_posX - 14, m_go.m_posY - 14, 255, 255, 255);

  //health bar
  gi = con.GetSprite(120);//red bar in background fixed
  gi.alpha = 80;
  con.RenderGraphicModulate(gi, m_go.m_posX - 16, m_go.m_posY - gi.height - height/3, 255, 255, 255);
  float healthFactor = m_go.m_health/m_go.m_maxHealth;
  gi = con.GetSprite(121);//green bar changes width
  gi.alpha = 180;
  float healthWidth = (float)gi.width * healthFactor;
  gi.width = healthWidth;
  con.RenderGraphicModulate(gi, m_go.m_posX - 16 , m_go.m_posY - gi.height - height/3, 255, 255, 255);
}


void CWeapon::SetObject(GAME_OBJECT game_object){
  m_go = game_object;
}

void CWeapon::AssignTarget(int x, int y, long id){
  m_targetX = x;
  m_targetY = y;
}
void CWeapon::UnassignTarget(){
  m_targetX = -1;
  m_targetY = -1;
}

bool CWeapon::RenderEffects(int &targetX,int &targetY, int &weaponType){
  if(m_renderEffects == false)
    return false;

  CFizzix *pFizzix = CFizzix::Instance();
  float distance = pFizzix->CalcDistance(m_go.m_posX, m_go.m_posY, m_targetX, m_targetY);
  
  int offset = (int)distance/10;  
  if(offset > 0){
    targetX = m_targetX - offset + rand() % (offset * 2);
    targetY = m_targetY - offset + rand() % (offset * 2);
  }
  else{
    targetX = m_targetX;
    targetY = m_targetY;
  }
  weaponType = m_go.m_weaponType;
 
  m_renderEffects = false;
  return true;
}

//controlled by m_projectile
void CWeapon::AssignDamage(float damage){
  m_go.m_health -= damage;
}