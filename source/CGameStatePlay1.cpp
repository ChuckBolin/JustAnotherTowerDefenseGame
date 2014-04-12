#include "..\include\CGameStatePlay1.h"
 
CGameStatePlay1::CGameStatePlay1(){
}

CGameStatePlay1::~CGameStatePlay1()
{
	m_TE.clear();
}

void CGameStatePlay1::Initialize(){

}

void CGameStatePlay1::Activate(CGameData &data, CConfigData &cfg, CGraphics &con){
  CLog *pLog = CLog::Instance();

  //clear tracks
  data.m_tracks.clear();
  
  //clears 
  data.m_waveGen.Clear();

  //clears objects on map
  data.m_pObject.clear();

  data.m_credits = 20000;
  data.m_score = 0;
  data.m_mouseState = -1;

  //reset user interface
  //m_UI.ResetState();
  //m_UI.Reset();
  m_UIstate = 0;

  data.m_weaponUniqueID = -1;

  data.m_projectile.clear();
  data.m_projectileCount = 0;
  data.m_gameTime = 0;

  //pyro setup
  //**********************
  m_effect.Init(con.m_d3ddev);
  m_effect.LoadFile(cfg.EffectsFile);//"assets\\data\\effects.dat");
  m_effectNumber = 0;
  m_effectMax = m_effect.GetNumberEffects();
  pLog->Log("******* Number of effects loaded", m_effectMax);

  if(m_effectMax < 1)
    m_effectNumber = 0;

  //audio setup
  CAudioManager *pAudio = CAudioManager::Instance();
  if(cfg.PlayMusic == 1)
    data.m_playMusic = true;
  else
    data.m_playMusic = false;

  if(cfg.PlaySounds == 1)
    data.m_playSound = true;
  else
    data.m_playSound = false;

  //play level 1
  //*****************************************************
  if(data.m_mapLevelToPlay == 1){
    if(data.LoadGameLevel(cfg.Level1) == false){
      pLog->Log("Failed to load level file", cfg.Level1);
      m_bReadyToRender = false;
    }    
    else
      m_bReadyToRender = true;
  }

  //play level 2
  //*****************************************************
  if(data.m_mapLevelToPlay == 2){
    if(data.LoadGameLevel(cfg.Level2) == false){
      pLog->Log("Failed to load level file", cfg.Level2);
      m_bReadyToRender = false;
    }    
    else
      m_bReadyToRender = true;
  }

  //play level 3
  //*****************************************************
  if(data.m_mapLevelToPlay == 3){
    if(data.LoadGameLevel(cfg.Level3) == false){
      pLog->Log("Failed to load level file", cfg.Level3);
      m_bReadyToRender = false;
    }    
    else
      m_bReadyToRender = true;
  }

  //play level 4
  //*****************************************************
  if(data.m_mapLevelToPlay == 4){
    if(data.LoadGameLevel(cfg.Level4) == false){
      pLog->Log("Failed to load level file", cfg.Level4);
      m_bReadyToRender = false;
    }    
    else
      m_bReadyToRender = true;
  } 

 // m_bPaused = false;
  m_startTime = ::timeGetTime();
  m_bMouseTarget = true;
  data.m_totalOffDamage = 0.0f;
  data.m_totalDefDamage = 0.0f;
  data.m_bGameOver = false;

}

void CGameStatePlay1::Deactivate(CGameData &data, CConfigData &cfg, CGraphics &con){
  //pyro housekeeping
  //********************************
  m_effect.Delete();
}

//***************************************************************
//                         U P D A T E
//***************************************************************
CGameStateObject* CGameStatePlay1::Update(double timeDifference,  CGameData &data, CConfigData &cfg, CGraphics &con){
  m_event = EVENT_GO_NO_WHERE;
  CLog *pLog = CLog::Instance();
  CAudioManager *pAudio = CAudioManager::Instance();
  CFizzix *pFizzix = CFizzix::Instance();  
   
  if(::GetActiveWindow() != data.m_hWnd)
    return NULL;

  m_mouse.SetHandle(data.m_hWnd);//needed for mouse

  //perform this code only once
  //****************************
  static bool bFirstTime = true;
  if(bFirstTime == true){
    bFirstTime = false;
    //m_bPaused = false;
  }

  //temporarily used to allow suer control of colors...keys UIOJKL
  con.m_textureRed = data.m_textureColorRed;
  con.m_textureGreen = data.m_textureColorGreen;
  con.m_textureBlue = data.m_textureColorBlue;

  if(m_bReadyToRender == false)
    return 0;

  //manages time
  timeDifference *= data.m_timeScale; //multiply by 1 or 2
  static double keyTime = 0;
  keyTime += timeDifference;
  static double leftClickTime = 0;
  leftClickTime += timeDifference;
  static double rightClickTime = 0;
  rightClickTime += timeDifference;

  //timers for GMHT keys
  static double gridTime = 0;
  gridTime += timeDifference;
  static double mapTime = 0;
  mapTime += timeDifference;
  static double legalTime = 0;
  legalTime += timeDifference;
  static double trackTime = 0;
  trackTime += timeDifference;
  
  //particle timer
  static double effectTime = 0;
  effectTime += timeDifference;
  static double effectTimeMult = 0;
  effectTimeMult += timeDifference;

  //TEST time for incrementing credits
  //static double creditTime = 0;
  //creditTime += timeDifference;
  //if(creditTime > 1){
  //  creditTime = 0;
  //  data.m_credits += 100;
  //}

  //play sound and music...controlled from icons
  if(data.m_playSound == true)
    cfg.PlaySounds = 1;
  else
    cfg.PlaySounds = 0;
  //if(data.m_playMusic == true)
   // cfg.PlayMusic = 1;
  //else
  //  cfg.PlayMusic = 0;

  if(cfg.PlaySounds == 1)
    pAudio->UnpauseAll();
  else
    pAudio->PauseAll();

  //if (cfg.PlayMusic == 1)
  pAudio->PlayAudio(0);
  //if(cfg.PlayMusic == 1)
  //  pAudio->PlayAudio(0);

  //determine repair/upgrade costs
  //****************************************
  int nIndexRU = -1;
  if(data.m_bPaused == false && data.m_bGameOver == false){
    data.m_costRepair = 0;
    data.m_costUpgrade = 0;
    int my = m_mouse.MouseY();// / 32 + 1) * 32 - 32;
    int mx = m_mouse.MouseX();// / 32 + 1) * 32 - 32;
    for(size_t i = 0; i < data.m_pObject.size(); ++i){
      if(data.m_pObject[i]->m_go.m_bAttacker == false &&  data.m_pObject[i]->m_go.m_posX > mx - 16 && data.m_pObject[i]->m_go.m_posX < mx + 16 &&
        data.m_pObject[i]->m_go.m_posY > my - 16 && data.m_pObject[i]->m_go.m_posY < my + 16 ){
        if(data.m_pObject[i]->m_go.m_upgradeRating == -1){
          data.m_costRepair = ((data.m_pObject[i]->m_go.m_maxHealth - data.m_pObject[i]->m_go.m_health)/data.m_pObject[i]->m_go.m_maxHealth) * data.m_pObject[i]->m_go.m_cost;
          data.m_costUpgrade = data.m_pObject[i]->m_go.m_cost * data.m_balUpgradeCostMult1;
          nIndexRU = i;
          break;          
        }
        else if(data.m_pObject[i]->m_go.m_upgradeRating == 1){
          data.m_costRepair = ((data.m_pObject[i]->m_go.m_maxHealth - data.m_pObject[i]->m_go.m_health)/data.m_pObject[i]->m_go.m_maxHealth) * data.m_pObject[i]->m_go.m_cost * data.m_balRepairCostMult1;
          data.m_costUpgrade = data.m_pObject[i]->m_go.m_cost * data.m_balUpgradeCostMult2;
          nIndexRU = i;
          break;          
        }
        else if(data.m_pObject[i]->m_go.m_upgradeRating == 2){
          data.m_costRepair = ((data.m_pObject[i]->m_go.m_maxHealth - data.m_pObject[i]->m_go.m_health)/data.m_pObject[i]->m_go.m_maxHealth) * data.m_pObject[i]->m_go.m_cost * data.m_balRepairCostMult2;
          data.m_costUpgrade = data.m_pObject[i]->m_go.m_cost * data.m_balUpgradeCostMult3;
          nIndexRU = i;
          break;          
        }
        else if(data.m_pObject[i]->m_go.m_upgradeRating == 3){
          data.m_costRepair = ((data.m_pObject[i]->m_go.m_maxHealth - data.m_pObject[i]->m_go.m_health)/data.m_pObject[i]->m_go.m_maxHealth) * data.m_pObject[i]->m_go.m_cost * data.m_balRepairCostMult3;
          data.m_costUpgrade = 0;//data.m_pObject[i]->m_go.m_cost * 3.0;
          nIndexRU = i;
          break;          
        }
      }
    }
  }

  GAME_OBJECT go;
  std::string parameter;

  //******************************************************* PROCESS INPUT
  if(::GetActiveWindow() == data.m_hWnd){// && data.m_bPaused == false){

    //is game over because there are no more units to spawn?
    if(data.m_waveGen.IsFinished() == true)
      data.m_bGameOver = true;

    STRUCT_PROJECTILE sp;
    bool bDestroy = false;
    //static bool bFlip = false;
    //if(data.m_projectile.size() > 25)
    //  bFlip = !bFlip;

    //manage collision of projectiles and intended targets
    if(data.m_bGameOver == false){
      //for(unsigned int i = (bFlip == true ? 0 : 25; i < data.m_projectile.size(); ++i){
      for(unsigned int i = 0; i < data.m_projectile.size(); ++i){
        if(i >= 0 && i < data.m_projectile.size()){
          sp = data.m_projectile[i].GetProjectileData();

          //inflict damage
          if(data.m_pObject.empty() == false){
            for(unsigned int j = 0; j < (data.m_pObject.size() < 40 ? data.m_pObject.size() : 40); ++j){
              if(j >= 0 && j < data.m_pObject.size()){
                go = data.m_pObject[j]->m_go;
                if((sp.bAttacker != go.m_bAttacker)){// || (sp.bAttacker == true && go.m_platformType == -1)){    

                  if(pFizzix->CalcDistance(sp.projectileX, sp.projectileY, go.m_posX, go.m_posY) < sp.weaponRadius){
                    bDestroy = true;

                    //ADD BALANCING m_balanceAdjustedFactor
                    if(sp.bAttacker == true)
                      data.m_pObject[j]->AssignDamage(sp.damage * data.m_balanceAdjustedFactor);
                    else
                      data.m_pObject[j]->AssignDamage(sp.damage);

                    m_effectNumber = data.m_pObject[j]->m_go.m_impactSmoke;
                    m_effect.Add(m_effectNumber, static_cast<float>(data.m_pObject[j]->m_go.m_posX), static_cast<float>(data.m_pObject[j]->m_go.m_posY), data.m_pObject[j]->m_go.m_weaponRadius * 0.01);
                    m_effectNumber = data.m_pObject[j]->m_go.m_impactEffect;
                    m_effect.Add(m_effectNumber, static_cast<float>(data.m_pObject[j]->m_go.m_posX), static_cast<float>(data.m_pObject[j]->m_go.m_posY), data.m_pObject[j]->m_go.m_weaponRadius * 0.025);


                    //accumulates damage for offensive and defensive units...used for balancing
                    if(sp.bAttacker == true)
                      data.m_totalOffDamage += sp.damage;
                    else
                      data.m_totalDefDamage += sp.damage;
                  }
                }
              }
            }
          }

          //destroy projectile
          if(bDestroy == true)
            data.m_projectile[i].Kill();

          bDestroy = false;
        }
      }
    }

    //deletes projectiles automatically
    //if(data.m_bGameOver == false){
      for(unsigned int i = 0; i < data.m_projectile.size(); ++i){
        if(data.m_projectile[i].GetStatusAlive() == false){
          data.m_projectile.erase(data.m_projectile.begin() + i);
          break;
        }
      }
      if(data.m_bGameOver == true){
        data.m_projectile.clear();
        
      }
    //}

    //updates User Interface
    m_UI.ResetState();
    m_UI.Update(timeDifference, m_mouse.MouseX(), m_mouse.MouseY(), data);
    m_UIstate = m_UI.GetUIState();

    //process panel selection
    //***************************************************************************    
    if(m_UIstate >= 7 && m_UIstate <= 11){  //user has clicked on 1 of 5 sprites
      int nWeapon = m_UIstate - 6;
      data.m_mouseState = 1;       //ready to add something on map by clicking
      data.m_mouseValue = nWeapon; //id of weapon to add when left click happens      
    }    

    //repair defensive units
    //********************************
    if(m_UI.GetUIState() < 1 && data.m_bRightMouseDown == true && rightClickTime  > 0.3  && data.m_bGameOver == false){
      rightClickTime = 0.0;

      //user clicked on map unit to upgrade
      //**************************************
      if(nIndexRU > -1 && nIndexRU < data.m_pObject.size()){
        //pLog->Log("rating", nIndexRU, data.m_pObject[nIndexRU]->m_go.m_upgradeRating);

        if(data.m_credits > data.m_costRepair){
          if(data.m_pObject[nIndexRU]->m_go.m_upgradeRating == -1){ //repair..no upgraded piece
            data.m_credits -= data.m_costRepair;        
            data.m_pObject[nIndexRU]->m_go.m_health = data.m_pObject[nIndexRU]->m_go.m_maxHealth;
          }
          else if(data.m_pObject[nIndexRU]->m_go.m_upgradeRating == 1){ //repair 1
            data.m_credits -= data.m_costRepair;        
            data.m_pObject[nIndexRU]->m_go.m_health = data.m_pObject[nIndexRU]->m_go.m_maxHealth * data.m_balUpgradeMult1;
          }
          else if(data.m_pObject[nIndexRU]->m_go.m_upgradeRating == 2){ //repair 2
            data.m_credits -= data.m_costRepair;        
            data.m_pObject[nIndexRU]->m_go.m_health = data.m_pObject[nIndexRU]->m_go.m_maxHealth * data.m_balUpgradeMult2;
          }             
          else if(data.m_pObject[nIndexRU]->m_go.m_upgradeRating == 3){ //repair 3
            data.m_credits -= data.m_costRepair;        
            data.m_pObject[nIndexRU]->m_go.m_health = data.m_pObject[nIndexRU]->m_go.m_maxHealth * data.m_balUpgradeMult3;
          }             
        }
      }
    }

    //upgrade defensive units
    //**************************************
    if(m_UI.GetUIState() < 1 && data.m_bLeftMouseDown == true && leftClickTime  > 0.3  && data.m_bGameOver == false){
      leftClickTime = 0.0;

      //user clicked on map unit to upgrade
      //**************************************
      if(nIndexRU > -1 && nIndexRU < data.m_pObject.size()  && data.m_pObject[nIndexRU]->m_go.m_platformType != -1){
        //pLog->Log("rating", nIndexRU, data.m_pObject[nIndexRU]->m_go.m_upgradeRating);

        if(data.m_credits > data.m_costUpgrade){
          if(data.m_pObject[nIndexRU]->m_go.m_upgradeRating == -1){ //1st upgrade
            data.m_pObject[nIndexRU]->m_go.m_upgradeRating = 1;
            data.m_credits -= data.m_costUpgrade;        
            data.m_pObject[nIndexRU]->m_go.m_damage = data.m_pObject[nIndexRU]->m_go.m_maxDamage * data.m_balUpgradeMult1;
            data.m_pObject[nIndexRU]->m_go.m_health = data.m_pObject[nIndexRU]->m_go.m_maxHealth * data.m_balUpgradeMult1;
          }
          else if(data.m_pObject[nIndexRU]->m_go.m_upgradeRating == 1){ //2nd upgrade
            data.m_pObject[nIndexRU]->m_go.m_upgradeRating = 2;
            data.m_credits -= data.m_costUpgrade;        
            data.m_pObject[nIndexRU]->m_go.m_damage = data.m_pObject[nIndexRU]->m_go.m_maxDamage * data.m_balUpgradeMult2;
            data.m_pObject[nIndexRU]->m_go.m_health = data.m_pObject[nIndexRU]->m_go.m_maxHealth * data.m_balUpgradeMult2;
          }
          else if(data.m_pObject[nIndexRU]->m_go.m_upgradeRating == 2){ //3rd upgrade
            data.m_pObject[nIndexRU]->m_go.m_upgradeRating = 3;
            data.m_credits -= data.m_costUpgrade;        
            data.m_pObject[nIndexRU]->m_go.m_damage = data.m_pObject[nIndexRU]->m_go.m_maxDamage * data.m_balUpgradeMult3;
            data.m_pObject[nIndexRU]->m_go.m_health = data.m_pObject[nIndexRU]->m_go.m_maxHealth * data.m_balUpgradeMult3;
          }      
        }
      }

      //ready to add something to map
      if(data.m_mouseState == 1){

        //look for map data
        for(int i = 0; i < data.m_catalog.GetTableSize(); ++i){
          parameter = data.m_catalog.GetTerm(i,1);
          if(data.m_mouseValue == atoi(parameter.c_str())){       
            go.m_posX = 32 * (m_mouse.MouseX()/32 + 1) - 16;
            go.m_posY = 32 * (m_mouse.MouseY()/32 + 1) - 16;
            if(data.m_map.IsLegalXY(go.m_posX, go.m_posY) == false)
              break;
            if(data.m_map.IsOccupiedCell(go.m_posX, go.m_posY) == true)
              break;
            
            //prevent defensive weapons from being added to HQ sprite
            if(go.m_posX > data.m_hqX - 32 && go.m_posX < data.m_hqX + 32 && go.m_posY > data.m_hqY - 32 && go.m_posY < data.m_hqY + 32)
              break;
            
            //must have enough credits to buy this weapon
            parameter = data.m_catalog.GetTerm(i, 11);
            data.m_cost = atoi(parameter.c_str());            
            if(data.m_cost > data.m_credits)
              break;
            data.m_credits -= atoi(parameter.c_str());
            
            if(data.m_map.AddWeapon(go.m_posX, go.m_posY, data.m_mouseValue) == false)
              break;
            go.m_angle = 0;
            parameter = data.m_catalog.GetTerm(i, 1);//weapon type
            go.m_weaponType = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 5);//can target air, hq
            go.m_targetAir = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 6);//can target sea, land, hq
            go.m_targetSeaLand = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 10);//ammo
            go.m_maxAmmo = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 9);//rate of fire
            go.m_rateOfFire = atof(parameter.c_str());
            if(go.m_rateOfFire < 1.0)
              go.m_angleRate =  0.05;
            else if(go.m_rateOfFire < 3.0)
              go.m_angleRate = 0.03;
            else
              go.m_angleRate = 0.01;
            parameter = data.m_catalog.GetTerm(i, 12);//health
            go.m_health = atof(parameter.c_str());
            go.m_maxHealth = go.m_health;
            parameter = data.m_catalog.GetTerm(i, 7);//damage
            go.m_damage = atof(parameter.c_str());
            go.m_maxDamage = go.m_damage;
            parameter = data.m_catalog.GetTerm(i, 8);//range..radius
            go.m_range = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 13);//sprite tripod or mount
            go.m_sprite = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 14);//sprite gun
            go.m_spriteGun = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 11);//credit cost
            go.m_cost = atoi(parameter.c_str());

            parameter = data.m_catalog.GetTerm(i, 15);//fire effect at weapon
            go.m_weaponEffect = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 24);//smoke effect at weapon
            go.m_weaponSmoke = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 16);//fire effect at impact
            go.m_impactEffect = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 25);//smoke effect at impact
            go.m_impactSmoke = atoi(parameter.c_str());

            parameter = data.m_catalog.GetTerm(i, 17);//type of platform
            go.m_platformType = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 18);//max velocity
            go.m_maxVelocity = atof(parameter.c_str());
            go.m_velocity = go.m_maxVelocity;
            parameter = data.m_catalog.GetTerm(i, 19);//render layer
            go.m_layer = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 20);//weapon sprite
            go.m_weaponSprite = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 21);//weapon velocity
            go.m_weaponVelocity = atof(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 22);//weapon render layer
            go.m_weaponLayer = atoi(parameter.c_str());
            parameter = data.m_catalog.GetTerm(i, 23);//weapon damage radius
            go.m_weaponRadius = atof(parameter.c_str());

            data.m_weaponUniqueID++;
            go.m_id = data.m_weaponUniqueID;
            go.animTime = 0;
            go.frameCount = 0;
            go.m_bAttacker = false;
            data.m_pObject.push_back(new CWeapon(go));
            //pLog->Log("Weapon added to m_pObject (id, type)", go.m_id, go.m_weaponType);
            //pLog->Log("Position", go.m_posX, go.m_posY);
          }
        }
      }
    }

    //manage time scale
    if(m_UIstate == UI_TIMES1 && data.m_timeScale == 2)
      data.m_timeScale = 1;
    else if(m_UIstate == UI_TIMES2 && data.m_timeScale == 1)
      data.m_timeScale = 2;

    //ESC key  - reset selection of defensive weapons
    if(keyDown(VK_ESCAPE) == true)
      m_UI.ResetState();
      
      //quit map level
      if(m_UIstate == UI_QUIT)
        m_event = EVENT_GO_MAIN; //CREDITS;
        //toggle grid
      else if(keyDown('G') == true && gridTime > 0.2 && cfg.DisplayDebugInfo == true){
        data.m_bGridOn = !data.m_bGridOn;
        gridTime = 0;
      }
      //toggle map
      else if(keyDown('M') == true && mapTime > 0.2 && cfg.DisplayDebugInfo == true){
        data.m_bMapOn = !data.m_bMapOn;
        mapTime = 0;
      }
      //toggle legal grid
      else if(keyDown('H') == true && legalTime > 0.2 && cfg.DisplayDebugInfo == true){
        data.m_bLegalOn = !data.m_bLegalOn;
        legalTime = 0;
      }
      //toggle track display
      else if(keyDown('T') == true && trackTime > 0.2 && cfg.DisplayDebugInfo == true){
        data.m_bTrackOn = !data.m_bTrackOn;
        trackTime = 0;
      }
      //test development of color selection
      else if (keyDown('U') == true && keyTime > 0.2 && cfg.DisplayDebugInfo == true){
        data.m_textureColorRed+= 10;
        if(data.m_textureColorRed > 255)
          data.m_textureColorRed = 255;
        keyTime = 0;
      }
      else if (keyDown('J') == true && keyTime > 0.2 && cfg.DisplayDebugInfo == true){
        data.m_textureColorRed-=10;
        if(data.m_textureColorRed < 0)
          data.m_textureColorRed = 0;
        keyTime = 0;
      }
      else if (keyDown('I') == true && keyTime > 0.2 && cfg.DisplayDebugInfo == true){
        data.m_textureColorGreen+= 10;
        if(data.m_textureColorGreen > 255)
          data.m_textureColorGreen = 255;
        keyTime = 0;
      }
      else if (keyDown('K') == true && keyTime > 0.2 && cfg.DisplayDebugInfo == true){
        data.m_textureColorGreen-=10;
        if(data.m_textureColorGreen < 0)
          data.m_textureColorGreen = 0;
        keyTime = 0;
      }
      else if (keyDown('O') == true && keyTime > 0.2 && cfg.DisplayDebugInfo == true){
        data.m_textureColorBlue+= 10;
        if(data.m_textureColorBlue > 255)
          data.m_textureColorBlue = 255;
        keyTime = 0;
      }
      else if (keyDown('L') == true && keyTime > 0.2 && cfg.DisplayDebugInfo == true){
        data.m_textureColorBlue-=10;
        if(data.m_textureColorBlue < 0)
          data.m_textureColorBlue = 0;
        keyTime = 0;
      }    
      else if(keyDown(VK_SPACE) == true && keyTime > 0.4){
      keyTime = 0;
      m_bMouseTarget = !m_bMouseTarget;
      data.m_credits = 0;
    }
  }  
  
  //manage sprite animation
  GRAPHIC_IMAGE gi;
  if(data.m_bPaused == false && data.m_bGameOver == false){
    for(unsigned int i = 0; i < data.m_pObject.size(); ++i){  //iterate through all objects    
      gi = con.GetSprite(data.m_pObject[i]->m_go.m_sprite);   //load graphic image
      if(gi.frameMax > 1){                          //this is an animated sequence
        //pLog->Log(data.m_pObject[i]->m_go.m_weaponType, 
        //          data.m_pObject[i]->m_go.animTime,
        //          data.m_pObject[i]->m_go.frameCount,
        //          gi.frameMax);
        data.m_pObject[i]->m_go.animTime += timeDifference;    //update frame animation timer
        if(data.m_pObject[i]->m_go.animTime > gi.updateInterval){//time to change
          data.m_pObject[i]->m_go.animTime = 0;                  //clear timer
          data.m_pObject[i]->m_go.frameCount++;             //inc frame number
          if(data.m_pObject[i]->m_go.frameCount > gi.frameMax)
            data.m_pObject[i]->m_go.frameCount = 1;
        }
      }      
    }
  }

  //object updating
  int distX = 0;
  int distY = 0;

  //********************* do if not paused
  if(data.m_bPaused == false && data.m_bGameOver == false){

    //calculate attacker's score of risk...higher..the badder =)
    //***************************************************************
    for(unsigned int i = 0; i < data.m_pObject.size(); ++i){
        
      //calculate risk assessment score for attackers
      if(data.m_pObject[i]->m_go.m_bAttacker == true){
        data.m_pObject[i]->m_go.m_score = data.m_pObject[i]->m_go.m_health + data.m_pObject[i]->m_go.m_damage + data.m_pObject[i]->m_go.m_velocity;
        distX = abs(data.m_hqX - data.m_pObject[i]->m_go.m_posX);
        if(distX < 0)
          distX = 0;
        distY = abs(data.m_hqY - data.m_pObject[i]->m_go.m_posY);
        if(distY < 0)
          distY = 0;
        data.m_pObject[i]->m_go.m_score += distX + distY;
      }

      //defenders are defaulted to a score of 100
      else{
        data.m_pObject[i]->m_go.m_score = 100;
      }
    }  

    //unassign all targets
    for(unsigned int i = 0; i < data.m_pObject.size(); ++i){
      data.m_pObject[i]->UnassignTarget();
    }

    //assign targets
    //************************
    if(data.m_pObject.empty() == false && data.m_bGameOver == false){
      for(unsigned int i = 0; i < data.m_pObject.size(); ++i){
        int highScore = 0;
        long highJ = 0;
        int distance = 0;
        int closeDistance = 2000;

        //defender looking for highest scored attacker
        for(unsigned int j = 0; j < data.m_pObject.size(); ++j){

          //ensure not targeting self or someone on same side
          if(i != j && (data.m_pObject[i]->m_go.m_bAttacker != data.m_pObject[j]->m_go.m_bAttacker)){
            
            //determine if HQ is being attacked
            if(data.m_pObject[i]->m_go.m_bAttacker == true && data.m_pObject[j]->m_go.m_platformType == -1){
              distance = pFizzix->CalcDistance(data.m_pObject[i]->m_go.m_posX, data.m_pObject[i]->m_go.m_posY,
                                               data.m_pObject[j]->m_go.m_posX, data.m_pObject[j]->m_go.m_posY);
              if (distance < data.m_pObject[i]->m_go.m_range){ // && distance < closeDistance){
                highScore = data.m_pObject[j]->m_go.m_score;
                highJ = j;
                closeDistance = 0;  //forces this to be the closest
              }
            }            
            
            if(//defensive artillery attacks tanks and ships
               (data.m_pObject[i]->m_go.m_weaponType > 0 && data.m_pObject[i]->m_go.m_weaponType < 5 && ((data.m_pObject[j]->m_go.m_weaponType > 12 && data.m_pObject[j]->m_go.m_weaponType < 16) || (data.m_pObject[j]->m_go.m_weaponType > 6 && data.m_pObject[j]->m_go.m_weaponType < 11)) ) ||
               //defensive MG, 20mm, and flak attacks aircraft
               ((data.m_pObject[i]->m_go.m_weaponType == 1 || data.m_pObject[i]->m_go.m_weaponType == 2 || data.m_pObject[i]->m_go.m_weaponType == 5) && (data.m_pObject[j]->m_go.m_weaponType == 11 || data.m_pObject[j]->m_go.m_weaponType == 12)) ||
               //tanks attack defensive units and HQ
               ((data.m_pObject[i]->m_go.m_weaponType > 12 && data.m_pObject[i]->m_go.m_weaponType < 16) && (data.m_pObject[j]->m_go.m_weaponType > 0 && data.m_pObject[j]->m_go.m_weaponType < 7)) ||
               //ships attack defensive units and HQ
               ((data.m_pObject[i]->m_go.m_weaponType > 6 && data.m_pObject[i]->m_go.m_weaponType < 11) &&  (data.m_pObject[j]->m_go.m_weaponType > 0 && data.m_pObject[j]->m_go.m_weaponType < 7)) ||
               //jets attack defensive units and HQ
               ((data.m_pObject[i]->m_go.m_weaponType > 10 && data.m_pObject[i]->m_go.m_weaponType < 13) && (data.m_pObject[j]->m_go.m_weaponType > 0 && data.m_pObject[j]->m_go.m_weaponType < 7)) 
               ){

                distance = pFizzix->CalcDistance(data.m_pObject[i]->m_go.m_posX, data.m_pObject[i]->m_go.m_posY,
                                                 data.m_pObject[j]->m_go.m_posX, data.m_pObject[j]->m_go.m_posY);
                if (distance < data.m_pObject[i]->m_go.m_range && distance < closeDistance){
                  highScore = data.m_pObject[j]->m_go.m_score;
                  highJ = j;
                  closeDistance = distance;
                }
            }
          }
        }
        
        if(highScore > 0){
          data.m_pObject[i]->AssignTarget(data.m_pObject[highJ]->m_go.m_posX, data.m_pObject[highJ]->m_go.m_posY, highJ);
        }
      }  
    }

    //update all projectiles
    if(data.m_projectile.empty() == false){// && data.m_bGameOver == false){
      for(unsigned int i = 0; i < data.m_projectile.size(); ++i){
        data.m_projectile[i].Update(timeDifference, data);
      }
    }
  }

  //update all objects
  //************************
  if(data.m_bPaused == false){// && data.m_bGameOver == false){
    for(unsigned int i = 0; i < data.m_pObject.size(); ++i){
      if(data.m_pObject.empty() == false){
        data.m_pObject[i]->Update(timeDifference, data);
      }
    }
  }

  //delete objects...no more health
  if(data.m_bPaused == false && data.m_bGameOver == false){
    for(size_t i = 0; i < data.m_pObject.size(); ++i){
      if(data.m_pObject[i]->m_go.m_bDead == true){
        data.m_map.RemoveWeapon(data.m_pObject[i]->m_go.m_posX, data.m_pObject[i]->m_go.m_posY);
        if(data.m_pObject[i]->m_go.m_platformType == -1){
          data.m_headquartersCount--;
          data.m_bGameOver = true;
        }
        else if(data.m_pObject[i]->m_go.m_bAttacker == true)
          data.IncrementCredits(data.m_pObject[i]->m_go.m_maxHealth);

        data.m_pObject.erase(data.m_pObject.begin() + i);
        break;
      }
    }
  }

  //pyro updating
  //*************************
  if(data.m_bPaused == false)  //allow for special effects if HQ is destroyed
    m_effect.Update(m_startTime);

  // data.m_gameClock.Update();
  if(data.m_bPaused == false && data.m_bGameOver == false)
    data.Update(timeDifference);  

  //manage game state changes
  for(unsigned int i=0; i< m_TE.size(); i++){
    if (m_TE[i].event == m_event){
      return m_TE[i].p_gso;
    }
  }
  return 0;
}

//***************************************************************
//                         R E N D E R 
//***************************************************************
void CGameStatePlay1::Render(CGraphics &con,  CGameData &data, CConfigData &cfg){
  //::ShowCursor(true);
  CLog *pLog = CLog::Instance();
  static bool bLostDevice = false; 
  
  ::SetCursor(false);

  //update sounds
  CAudioManager *pAudio = CAudioManager::Instance();
  pAudio->Update();

  //test to see if graphic device is lost, if so, deal with it
  //**********************************************************
  HRESULT hr;
  if(FAILED(hr = con.m_d3ddev->TestCooperativeLevel())){
    
    //device is lost...cannot be reset...exit Render() function
    if(hr == D3DERR_DEVICELOST){
      if(bLostDevice == false){
        bLostDevice = true;
        pLog->Log("Lost");
        con.DeleteLost();    //delete one time per failure
        m_effect.Delete();
        pLog->Log("Deleted");
      }
      return;
    }

    //device was lost...but now can be RESET
    else if( hr == D3DERR_DEVICENOTRESET ){
      hr = con.m_d3ddev->Reset(&con.GetDPP());      
      if(SUCCEEDED(hr)){
        pLog->Log("Loss of focus...reloading!");
        m_effect.Init(con.m_d3ddev);
        m_effect.LoadFile(cfg.EffectsFile);// "assets\\data\\effects.dat");
        m_effectNumber = 0;
        m_effectMax = m_effect.GetNumberEffects();
        pLog->Log("******* Number of effects reloaded", m_effectMax);

        con.LoadAssetFile(cfg.GamePlayAssetFile);
        con.LoadAssetFile(cfg.FrameworkAssetFile);     
        con.ReloadLost();

        bLostDevice = false;
      }
      return;
    }
  }
  
  // clear the window to a deep blue
  con.m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(data.m_screenColorRed, data.m_screenColorGreen, data.m_screenColorBlue), 1.0f, 0);//0, 111, 255
  con.m_d3ddev->BeginScene();  
  con.BeginRender();
  GRAPHIC_IMAGE gi;

  //render map
  if (data.m_bMapOn == true){
    gi = con.GetSprite(data.m_mapSpriteNumber);//103
    con.RenderGraphicModulate(gi, 0, 0, 255, 255, 255);
  }
 
  //render grid
  if (data.m_bGridOn == true){
    gi = con.GetSprite(100);
    con.RenderGraphicModulate(gi, 0, 0, 255, 255, 255);
  }  

  //render damaged HQ sprite...place under active HQ
  //when HQ is destroyed, this damaged sprite will be seen
  if(data.m_hqSpriteDamaged > -1){
    gi = con.GetSprite(data.m_hqSpriteDamaged);
    con.RenderGraphicModulate(gi,data.m_hqX - gi.width/2, data.m_hqY - gi.height/2, 255, 255, 255);
  }

  //renders all game objects
  for(int layer = 0; layer < 11; ++layer){
    for(unsigned int i = 0; i < data.m_pObject.size(); ++i){
      if(data.m_pObject[i]->GetLayer() == layer)
          data.m_pObject[i]->Render(con, data);
    }
  }
  
  //render all projectiles
  if(data.m_projectile.empty() == false){
    for(unsigned int i = 0; i < data.m_projectile.size(); ++i){
      data.m_projectile[i].Render(con, data);
      if(i > 50) //limit
        break;
    }
  }


  //render gray defensive unit ready for placement on map
  if(data.m_mouseState == 1){
    int posX = 32 * (m_mouse.MouseX()/32 + 1) - 16;
    int posY = 32 * (m_mouse.MouseY()/32 + 1) - 16;
    std::string sSprite = data.m_catalog.GetTerm(data.m_mouseValue - 1, 13);
    gi = con.GetSprite(atoi(sSprite.c_str()));
    gi.alpha = 120;
    con.RenderGraphicModulate(gi, posX - gi.width/2, posY - gi.height/2, 120, 120, 120);  
    std::string sSpriteGun = data.m_catalog.GetTerm(data.m_mouseValue - 1, 14);
    gi = con.GetSprite(atoi(sSpriteGun.c_str()));
    gi.alpha = 120;
    con.RenderGraphicModulate(gi, posX - gi.width/2, posY - gi.height/2, 120, 120, 120);  

  }

  //render UI panels
  m_UI.Render(con, data);

  //paused
  if(data.m_bPaused == true){
    gi = con.GetSprite(1011);
    con.RenderGraphicModulate(gi, con.GetScreenCenterX() - gi.width/2, con.GetScreenCenterY() - gi.height/2, 255, 255, 255);
  }

  //game over you lose
  if(data.m_bGameOver == true && data.m_waveGen.IsFinished() == false){
    gi = con.GetSprite(1402);
    con.RenderGraphicModulate(gi, con.GetScreenCenterX() - gi.width/2, con.GetScreenCenterY() - gi.height/2, 255, 255, 255);
  }

  //game over you win
  if(data.m_bGameOver == true && data.m_waveGen.IsFinished() == true){
    gi = con.GetSprite(1403);
    con.RenderGraphicModulate(gi, con.GetScreenCenterX() - gi.width/2, con.GetScreenCenterY() - gi.height/2, 255, 255, 255);
  }

  if(data.m_bDisplayHelp == true){
    gi = con.GetSprite(1450);
    con.RenderGraphicModulate(gi,0, 0, 255, 255, 255);
  }

  con.EndRender();  
  
  //pyro rendering
  //render shooting effects
  int posX = 0;
  int posY = 0;
  int weaponType = 0;
  float smokeRadius = 0;
  float fireRadius = 0;
  float dx = 0; 
  float dy = 0; 
  bool bAddEffect = false;
  if(data.m_bPaused == false){
    m_effect.Render(); 
    for(unsigned int i = 0; i < data.m_pObject.size(); ++i){
      if(data.m_pObject[i]->RenderEffects(posX, posY, weaponType) == true){ //m_bMouseTarget == true && 
        bAddEffect = false;
        //dx and dy are used to position smoke and fire at barrel end
        dy = cos(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius;
        dx = sin(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius;
        
        //calculate radius of smoke and fire for weapon size
        smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius;// * data.m_pObject[i]->m_go.m_damage * 0.01 ;//0.005;
        fireRadius = data.m_pObject[i]->m_go.m_weaponRadius;// * data.m_pObject[i]->m_go.m_damage * 0.02;//0.04

        if(data.m_pObject[i]->m_go.m_weaponType == 1){//machine gun
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.001;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.005;
          bAddEffect = true;
        }
        else if(data.m_pObject[i]->m_go.m_weaponType == 2){//20 mm machine gun
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.002;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.01;
          bAddEffect = true;
        }
        else if(data.m_pObject[i]->m_go.m_weaponType == 3){//3" cannon
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.0025;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.028;
          dy = cos(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius * 1.1;
          dx = sin(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius * 1.1;
          bAddEffect = true;
        }
        else if(data.m_pObject[i]->m_go.m_weaponType == 4){//6" cannon
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.003;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.03;
          dy = cos(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius * 1.1;
          dx = sin(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius * 1.1;
          bAddEffect = true;
        }
        else if(data.m_pObject[i]->m_go.m_weaponType == 6){//flak
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.006;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.025;
          dy = cos(data.m_pObject[i]->m_go.m_angle - 0.25) * data.m_pObject[i]->m_go.m_weaponRadius * 1.2;
          dx = sin(data.m_pObject[i]->m_go.m_angle - 0.25) * data.m_pObject[i]->m_go.m_weaponRadius * 1.2;
          m_effectNumber = data.m_pObject[i]->m_go.m_weaponSmoke; 
          m_effect.Add(m_effectNumber, static_cast<float>(data.m_pObject[i]->m_go.m_posX) + dx,static_cast<float>(data.m_pObject[i]->m_go.m_posY) - dy, smokeRadius);
          m_effectNumber = data.m_pObject[i]->m_go.m_weaponEffect;
          m_effect.Add(m_effectNumber, static_cast<float>(data.m_pObject[i]->m_go.m_posX) + dx,static_cast<float>(data.m_pObject[i]->m_go.m_posY) - dy, fireRadius);
          dy = cos(data.m_pObject[i]->m_go.m_angle + 0.25) * data.m_pObject[i]->m_go.m_weaponRadius * 1.2;
          dx = sin(data.m_pObject[i]->m_go.m_angle + 0.25) * data.m_pObject[i]->m_go.m_weaponRadius * 1.2;
          m_effectNumber = data.m_pObject[i]->m_go.m_weaponSmoke; 
          m_effect.Add(m_effectNumber, static_cast<float>(data.m_pObject[i]->m_go.m_posX) + dx,static_cast<float>(data.m_pObject[i]->m_go.m_posY) - dy, smokeRadius);
          m_effectNumber = data.m_pObject[i]->m_go.m_weaponEffect;
          m_effect.Add(m_effectNumber, static_cast<float>(data.m_pObject[i]->m_go.m_posX) + dx,static_cast<float>(data.m_pObject[i]->m_go.m_posY) - dy, fireRadius);
        }
        else if(data.m_pObject[i]->m_go.m_weaponType == 7){//riverboat
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.004;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.023;
          bAddEffect = true;
        }
        else if(data.m_pObject[i]->m_go.m_weaponType == 8){//destroyer
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.005;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.025;
          bAddEffect = true;
        }
        else if(data.m_pObject[i]->m_go.m_weaponType == 9){//cruiser
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.006;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.028;
          bAddEffect = true;
        }
        else if(data.m_pObject[i]->m_go.m_weaponType == 10){//battleship
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.007;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.034;
          bAddEffect = true;
        }
        else if(data.m_pObject[i]->m_go.m_weaponType == 13){//tank1
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.004;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.028;
          dy = cos(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius * 1.13;
          dx = sin(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius * 1.13;
          bAddEffect = true;
        }
        else if(data.m_pObject[i]->m_go.m_weaponType == 14){//tank2
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.005;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.028;
          dy = cos(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius * 1.2;
          dx = sin(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius * 1.2;
          bAddEffect = true;
        }
        else if(data.m_pObject[i]->m_go.m_weaponType == 15){//tank3
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.006;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.028;
          dy = cos(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius * 1.3;
          dx = sin(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius * 1.3;
          bAddEffect = true;
        }
        /*
        else if(data.m_pObject[i]->m_go.m_weaponType == -1){//HQ
          smokeRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.008;
          fireRadius = data.m_pObject[i]->m_go.m_weaponRadius * 0.030;
          dy = cos(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius * 1.4;
          dx = sin(data.m_pObject[i]->m_go.m_angle) * data.m_pObject[i]->m_go.m_weaponRadius * 1.4;
          bAddEffect = true;
        }*/

        if(bAddEffect == true){
          m_effectNumber = data.m_pObject[i]->m_go.m_weaponSmoke; 
          m_effect.Add(m_effectNumber, static_cast<float>(data.m_pObject[i]->m_go.m_posX) + dx,static_cast<float>(data.m_pObject[i]->m_go.m_posY) - dy, smokeRadius);
          m_effectNumber = data.m_pObject[i]->m_go.m_weaponEffect;
          m_effect.Add(m_effectNumber, static_cast<float>(data.m_pObject[i]->m_go.m_posX) + dx,static_cast<float>(data.m_pObject[i]->m_go.m_posY) - dy, fireRadius);
        }
      }
    }
  }

  //draws circles around all weapons for ranging
  if(m_bMouseTarget == false){
    if(data.m_pObject.empty() == false){
      for(unsigned int i = 0; i < data.m_pObject.size(); ++i){
        if(data.m_pObject.empty() == false){
        con.DrawCircle(data.m_pObject[i]->m_go.m_posX,
                       data.m_pObject[i]->m_go.m_posY, 0, 0,(int)(data.m_pObject[i]->m_go.m_range/2), data.m_pObject[i]->m_go.m_range);
        }
      }
    }
  }

  //renders UI panel outlines
  m_UI.RenderOutlines(con, data);

  //user has selected a unit from defensive panel...show radius ring
  //doesn't work correctly
  if(data.m_mouseState == 1){
    std::string sRadius = data.m_catalog.GetTerm(data.m_mouseValue - 1, 8);
    con.DrawCircle(m_mouse.MouseX(), m_mouse.MouseY(), 255, 0, 255, atoi(sRadius.c_str()));  
  }

  //draw mouse
  con.DrawCircle(m_mouse.MouseX(), m_mouse.MouseY(), 255, 255, 255, 5);  
  con.DrawRect(m_mouse.MouseX() - 16, m_mouse.MouseY() - 16, m_mouse.MouseX() + 16, m_mouse.MouseY() + 16, 255, 255, 255);

  //renders legal spaces for TESTING
  //if(data.m_bPaused == false){
    if(data.m_bLegalOn == true){
      for (int col = 1; col < 33; col++){
        for (int row = 1; row < 25; row++){
          if(data.m_map.IsLegalCell(row, col) == true)
            con.DrawRect(col * 32 - 32, row * 32 - 32,col * 32, row * 32, 255, 255, 0);
        }
      }
    }
  //}

  //renders AI track waypoint
  //if(data.m_bPaused == false){
    if(data.m_bTrackOn == true){
      TRACK_WAYPOINT twpt;
      TRACK_WAYPOINT last_twpt;
      bool bRet = false;
      if(data.m_tracks.empty() == false){
        for(unsigned int i = 0; i < data.m_tracks.size(); ++i){
          for(unsigned int j = 0; j < data.m_tracks[i].GetNumWaypoints(); ++j){
            bRet = data.m_tracks[i].GetWayPointIndex(twpt, j);
            //twpt = data.m_tracks[i].GetWayPoint(j);
            con.DrawCircle((float)twpt.x, (float)twpt.y, 255, 0, 0, (float)twpt.radius);
            if(j> 0)
              con.DrawLine(twpt.x, twpt.y, last_twpt.x, last_twpt.y, 255, 0, 0);
            if(j == 1){
              con.Print("Track", i, F_V16, twpt.x, twpt.y, 255, 255, 255, 255);
            }
            last_twpt = twpt;
          }
        }
      }
    }
  //}


if(cfg.DisplayDebugInfo == true){
  con.Print("FPS", data.m_FPS, F_V16, 20, 10, 255, 255, 255, 255);
  con.Print ("Number Effects", m_effect.GetEmitterCount(2), F_V16, 20, 30, 255, 255, 255, 255);
  con.Print("Mouse X", m_mouse.MouseX(), F_V16, 860, 10, 255, 255, 255, 255);
  con.Print("Mouse Y", m_mouse.MouseY(), F_V16, 860, 30, 255, 255, 255, 255);
  con.Print("Game Time", data.m_gameTime, F_V16, 860, 50, 255, 255, 255, 255);
  
  con.Print("Off Unit", data.m_totalOffDamage, F_V16, 410, 10, 255, 255, 255, 255);
  con.Print("Def Unit", data.m_totalDefDamage,  F_V16, 410, 30, 255, 255, 255, 255);
  con.Print("Bal", data.m_balanceFactor, F_V16, 410, 50, 255, 255, 255, 255);
  con.Print("Adj. Bal", data.m_balanceAdjustedFactor, F_V16, 410, 70, 255, 255, 255, 255);
  con.Print("Upp Dam", data.m_balDamageUpper, F_V16, 410, 90, 255, 255, 255, 255);
  con.Print("Low Dam", data.m_balDamageLower, F_V16, 410, 110, 255, 255, 255, 255);
}


  /*
  if(m_balanceFactor > m_balDamageUpper)
    m_balanceAdjustedFactor = 1/m_balanceFactor;
  else if(m_balanceFactor < m_balDamageLower)
    m_balanceAdjustedFactor = 1/m_balanceFactor;
  */


  con.m_d3ddev->EndScene();    // ends the 3D scene
  hr = con.m_d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}
  
void CGameStatePlay1::ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){

}



void CGameStatePlay1::Resume(){
}
void CGameStatePlay1::Pause(){
}
void CGameStatePlay1::Save(){
}

void CGameStatePlay1::AddTransitionEvent(int event, CGameStateObject* p_Next){
  TRANSITION_EVENT te;
  te.event=event;
  te.p_gso = p_Next;

  m_TE.push_back(te);
}

