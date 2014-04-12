#include "..\include\CGameData.h"

//game data constructor
//set all initial game data variables here
CGameData::CGameData(){

  //new data
  m_gameTimer.Initialize();
  m_gameSeconds = 2000;
  m_gameTime = 0;
  m_bPaused = false;
  m_bGameOver = false;
  m_bMapOn = false;
  m_bGridOn = false;
  m_bLegalOn = false;
  m_mapSpriteNumber = 101;
  m_bAdvanced = false;

  //default information...may be replaced by level data file
  m_screenColorRed = 0;
  m_screenColorGreen = 111;
  m_screenColorBlue = 255;  

  m_worldX = 0;
  m_worldY = 0;
  m_worldLeft = 0;
  m_worldRight = 72000;
  m_worldTop = 0;
  m_worldBottom = 48000;

  m_bLeftMouseDown = false;
  m_bRightMouseDown = false;
  m_shadowX = 5;
  m_shadowY = 5;
  m_textureColorRed = 150;//30;//255;
  m_textureColorGreen = 100;//20;//255;
  m_textureColorBlue = 130;//60;//255;
  m_mapLevelToPlay = 1;
  m_timeScale = 1.0;
  m_score = 0;
  m_credits = 20000;

  m_playSound = true;
  m_playMusic = true;
  m_bDisplayHelp = false;

  m_mouseState = 0;
  m_mouseValue = -1;
  m_headquartersCount = 0;
  m_weaponUniqueID = -1;
  m_projectileCount = 0;
  m_bAttackerFiring = true; //allows projectiles to fire

  //used to render damaged HQ
  m_hqSpriteDamaged = -1;
  m_hqX = -1;
  m_hqY = -1;
  m_cost = 0;

  //balancing variables used for upgrades, damage calcs, etc.
  //default factors
  m_balWavecountDivisor = 5;
  m_balAdvFactor = 4;
  m_balBegFactor = 1;
  m_balCreditMult = 25;
  m_balScoreMult = 25;
  m_balUpgradeMult1 = 2;
  m_balUpgradeMult2 = 4;
  m_balUpgradeMult3 = 8;
  m_balUpgradeCostMult1 = 0.75;
  m_balUpgradeCostMult2 = 1.5;
  m_balUpgradeCostMult3 = 3.0;
  m_balRepairCostMult1 = 1.0;
  m_balRepairCostMult2 = 1.5;
  m_balRepairCostMult3 = 3.0;

  m_balDamageUpper = 1.1;
  m_balDamageLower = 0.9;
  m_balanceFactor = 1.0;
  m_balanceAdjustedFactor = 1.0;
  m_totalOffDamage = 0.0;
  m_totalDefDamage = 0.0;
}

CGameData::~CGameData(){

}

//need to add sprite data to level data
bool CGameData::AddGraphicDataToLevelData(CGraphics & con){
  CLog *pLog = CLog::Instance();
  pLog->Log("Completed the addition of graphic data to level data");
  return true;
}

bool CGameData::AddObject(CGraphics &con, std::string objectName, int x, int y){
  return true;
}

bool CGameData::LoadObjectFile(std::string filename){
  m_catalog.LoadFile(filename);
  if(m_catalog.GetTableSize() > 0)
    return true;
  else
    return false;
}

void CGameData::Update(double timeDifference){
  CLog *pLog = CLog::Instance();

  static double totalTime = 0;
  totalTime += timeDifference;
   
  if(m_bPaused == false){
    if(totalTime > 1.0){
      m_gameTime++;
      if(m_gameTime % 15 == 1){
        //m_gameTime = 0;

        //calculates balance factor
        //***************************************************
        if (m_totalDefDamage == 0)
          m_totalDefDamage = 0.01;
        m_balanceFactor = m_totalOffDamage/m_totalDefDamage; 
        if(m_balanceFactor < 0.0001)
          m_balanceFactor = 0.0001;
        //m_gameTime = 1;
        
        //used for adjusting damage capability of attackers, inc or dec
        if(m_balanceFactor > m_balDamageUpper)
          m_balanceAdjustedFactor = 1/m_balanceFactor;
        else if(m_balanceFactor < m_balDamageLower)
          m_balanceAdjustedFactor = 1/m_balanceFactor;
        else
          m_balanceAdjustedFactor = m_balanceFactor;

        if(m_balanceAdjustedFactor > 10)
          m_balanceAdjustedFactor = 10;
        else if(m_balanceAdjustedFactor < 0.1)
          m_balanceAdjustedFactor = 0.1;

        //***************************************************
      }
      totalTime = 0;
    }
  }

  //returns true if there is a new object to add
  WAVE_OBJECT_DATA wod;  
  GAME_OBJECT go;
  TRACK_WAYPOINT trackWaypoint;
  bool bRet = false;
  float angle = 0;
  float radius = 0;
  float dx = 0;
  float dy = 0;
  float factor = 1;

  if(m_waveGen.Update(timeDifference, wod) == true){ //there is an object waiting to be spawned
    factor = ((float)(m_waveGen.GetWaveCount()) + 1)/m_balWavecountDivisor ; //used to adjust damage and health for 'wave count'
    std::string parameter;
    for(int i = 0; i < m_catalog.GetTableSize(); ++i){
      parameter = m_catalog.GetTerm(i,1);
      if(wod.objectID == atoi(parameter.c_str())){  //locate spawned object ID in catalog of objects
        //pLog->Log(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>", wod.objectID, m_tracks.size());
        if(wod.trackIndex >= 0 && wod.trackIndex < m_tracks.size()){
          //pLog->Log("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<", wod.objectID);
          go.m_track = m_tracks[wod.trackIndex];    //locate track
          if(go.m_track.GetCurrentWayPoint(trackWaypoint) == true){
            angle = (float)(rand() % 628) * 0.01f;
            radius = (float)(rand() % trackWaypoint.radius);
            dx = 2 * radius * cos(angle);
            dy = 2 * radius * sin(angle);            
            go.m_posX = trackWaypoint.x - trackWaypoint.radius + dx;
            go.m_posY = trackWaypoint.y - trackWaypoint.radius + dy;
            go.m_posX = 32 * (go.m_posX/32 + 1) - 16;  //need to randomize later based upon radius
            go.m_posY = 32 * (go.m_posY/32 + 1) - 16;
            go.m_angle = 0;
            parameter = m_catalog.GetTerm(i, 1);//weapon type
            go.m_weaponType = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 5);//can target air, hq
            go.m_targetAir = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 6);//can target sea, land, hq
            go.m_targetSeaLand = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 7);//damage            
            //advanced vs. beginner
            //applies balancing equations
            //*********************************************************
            if(m_bAdvanced == true)
              go.m_damage = atof(parameter.c_str()) * factor * m_balAdvFactor;// * m_balanceAdjustedFactor;
            else
              go.m_damage = atof(parameter.c_str()) * factor * m_balBegFactor;// * m_balanceAdjustedFactor;
            go.m_maxDamage = go.m_damage;

            parameter = m_catalog.GetTerm(i, 8);//range..radius
            go.m_range = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 9);//rate of fire
            go.m_rateOfFire = atof(parameter.c_str());
            if(go.m_rateOfFire < 1.0)
              go.m_angleRate =  0.05;
            else if(go.m_rateOfFire < 3.0)
              go.m_angleRate = 0.03;
            else
              go.m_angleRate = 0.01;
            //pLog->Log("ROF", go.m_rateOfFire);
            parameter = m_catalog.GetTerm(i, 10);//ammo
            go.m_maxAmmo = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 11);//credit cost
            go.m_cost = atoi(parameter.c_str());            
            parameter = m_catalog.GetTerm(i, 12);//health
            go.m_health = atof(parameter.c_str()) * factor;
            go.m_maxHealth = go.m_health;
            parameter = m_catalog.GetTerm(i, 13);//sprite tripod or mount
            go.m_sprite = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 14);//sprite gun
            go.m_spriteGun = atoi(parameter.c_str());
            
            parameter = m_catalog.GetTerm(i, 15);//fire effect at weapon
            go.m_weaponEffect = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 24);//smoke effect at weapon
            go.m_weaponSmoke = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 16);//fire effect at impact
            go.m_impactEffect = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 25);//smoke effect at impact
            go.m_impactSmoke = atoi(parameter.c_str());

            parameter = m_catalog.GetTerm(i, 17);//type of platform
            go.m_platformType = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 18);//max velocity
            go.m_maxVelocity = atof(parameter.c_str());
            go.m_velocity = go.m_maxVelocity;
            parameter = m_catalog.GetTerm(i, 19);//render layer
            go.m_layer = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 20);//weapon sprite
            go.m_weaponSprite = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 21);//weapon velocity
            go.m_weaponVelocity = atof(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 22);//weapon render layer
            go.m_weaponLayer = atoi(parameter.c_str());
            parameter = m_catalog.GetTerm(i, 23);//weapon damage radius
            go.m_weaponRadius = atof(parameter.c_str());
            m_weaponUniqueID++;
            go.m_id = m_weaponUniqueID;
            go.animTime = 0;
            go.frameCount = 1;
            go.m_bAttacker = true;
            m_pObject.push_back(new CWeapon(go));
            //pLog->Log("************************Spawned item added");
          }
        }
      }
    }
  }
}

bool CGameData::LoadGameLevel(std::string filename){
  CLog *pLog = CLog::Instance();

  std::string value;
  std::string asset;
  std::string parameter;
 
  //used specifically for m_waveGen
  long timeToStart;       //time to start in seconds after start of map level
  int trackIndex;         //index for the track to follow...see CTrack()
  int objectID;           //id of the object to add to m_Object list
  int quantity;           //number of objects to spawn in this wave
  double spawnInterval;   //time interval between spawns

  GAME_OBJECT go;

  //new
  pLog->Log("Loading game level", filename);
  CCSVReader csv;
  csv.LoadFile(filename);
  pLog->Log("Level table size", csv.GetTableSize());
  if(csv.GetTableSize() < 1)
    return false;

  //creates variables row, col, num for legal cells
  int nNumberCells;
  int nRow;
  int nCol;

  for(int i = 0; i < csv.GetTableSize(); ++i){

    asset = csv.GetTerm(i, 0);        
    if(asset == "back_color" && csv.GetNumberParameters(i)== 4 ){
      parameter = csv.GetTerm(i, 1);
      m_screenColorRed = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      m_screenColorGreen = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      m_screenColorBlue = atoi(parameter.c_str());      
    }
    else if(asset == "wave" && csv.GetNumberParameters(i)== 6 ){
      parameter = csv.GetTerm(i, 1);
      timeToStart = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      trackIndex = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      objectID = atoi(parameter.c_str());      
      parameter = csv.GetTerm(i, 4);
      quantity = atoi(parameter.c_str());      
      parameter = csv.GetTerm(i, 5);
      spawnInterval = atof(parameter.c_str());      
      m_waveGen.AddWave(timeToStart, trackIndex, objectID, quantity, spawnInterval);
      //pLog->Log(".................wave added", timeToStart);

    }
    else if(asset == "credits" && csv.GetNumberParameters(i)== 2 ){
      parameter = csv.GetTerm(i, 1);
      m_credits = atoi(parameter.c_str());
    }
    else if(asset == "hq" && csv.GetNumberParameters(i) == 3){
      for(int j = 0; j < m_catalog.GetTableSize(); ++j){
        parameter = m_catalog.GetTerm(j, 1);
        if(parameter == "16"){
          objectID = 16;
          parameter = csv.GetTerm(i, 1);
          go.m_posX = atoi(parameter.c_str());
          parameter = csv.GetTerm(i, 2);
          go.m_posY = atoi(parameter.c_str());
          m_hqX = go.m_posX; //used for faster calculation of scores
          m_hqY = go.m_posY;
          go.m_angle = 0;
          
          go.m_weaponType = objectID; //hq symbol
          parameter = m_catalog.GetTerm(j, 7);//damage
          go.m_damage = atof(parameter.c_str());
          parameter = m_catalog.GetTerm(j, 8);//range..radius
          go.m_range = atoi(parameter.c_str());
          parameter = m_catalog.GetTerm(j, 9);//rate of fire
          go.m_rateOfFire = atof(parameter.c_str());
          if(go.m_rateOfFire < 1.0)
            go.m_angleRate =  0.05;
          else if(go.m_rateOfFire < 3.0)
            go.m_angleRate = 0.03;
          else
            go.m_angleRate = 0.01;
          parameter = m_catalog.GetTerm(j, 10);//ammo
          go.m_maxAmmo = atoi(parameter.c_str());
          parameter = m_catalog.GetTerm(j, 11);//credit cost
          go.m_cost = atoi(parameter.c_str());            
          parameter = m_catalog.GetTerm(j, 12);//health
          go.m_health = atof(parameter.c_str());
          go.m_maxHealth = go.m_health;
          parameter = m_catalog.GetTerm(j, 13);//sprite tripod or mount
          go.m_sprite = atoi(parameter.c_str());
          parameter = m_catalog.GetTerm(j, 14);//sprite gun
          go.m_spriteGun = atoi(parameter.c_str());
          m_hqSpriteDamaged = go.m_spriteGun;
          parameter = m_catalog.GetTerm(j, 15);//type of effect at weapon
          go.m_weaponEffect = atoi(parameter.c_str());
          parameter = m_catalog.GetTerm(j, 17);//type of platform
          go.m_platformType = atoi(parameter.c_str());
          parameter = m_catalog.GetTerm(j, 18);//max velocity
          go.m_maxVelocity = atof(parameter.c_str());
          go.m_velocity = go.m_maxVelocity;
          parameter = m_catalog.GetTerm(j, 19);//render layer
          go.m_layer = atoi(parameter.c_str());
          go.animTime = 0;
          go.frameCount = 0;
          go.m_bAttacker = false;
          m_pObject.push_back(new CWeapon(go));
          m_headquartersCount++;
         // pLog->Log("Head Quarters added...................", go.m_posX, go.m_posY);
         // pLog->Log(go.m_sprite, go.m_spriteGun);
        }
      }
    }
    else if(asset == "screen_color" && csv.GetNumberParameters(i)== 4 ){
      parameter = csv.GetTerm(i, 1);
      m_textureColorRed = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      m_textureColorGreen = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      m_textureColorBlue = atoi(parameter.c_str());      
    }
    else if(asset == "world" && csv.GetNumberParameters(i)  == 5 ){
      parameter = csv.GetTerm(i, 1);
      m_worldLeft = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      m_worldTop = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      m_worldRight = atoi(parameter.c_str());      
      parameter = csv.GetTerm(i, 4);
      m_worldBottom = atoi(parameter.c_str());      
    }
    else if(asset == "position" && csv.GetNumberParameters(i)  == 3 ){
      parameter = csv.GetTerm(i, 1);
      m_worldX = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      m_worldY = atoi(parameter.c_str());
    }
    else if(asset == "shadow" && csv.GetNumberParameters(i)  == 3 ){
      parameter = csv.GetTerm(i, 1);
      m_shadowX = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      m_shadowY = atoi(parameter.c_str());
    }
    else if(asset == "map_file" && csv.GetNumberParameters(i) == 2){
      parameter = csv.GetTerm(i, 1);
      m_mapSpriteNumber = atoi(parameter.c_str());
    }
    else if(asset == "firing" && csv.GetNumberParameters(i) == 2){
      parameter = csv.GetTerm(i, 1);
      if(atoi(parameter.c_str()) == 1)
        m_bAttackerFiring = true;
      else
        m_bAttackerFiring = false;
    }


    else if(asset == "show_grid" && csv.GetNumberParameters(i) == 2){
      parameter = csv.GetTerm(i, 1);
      if(atoi(parameter.c_str()) == 1)
        m_bGridOn = true;
      else
        m_bGridOn = false;
    }
    else if(asset == "show_legal_grid" && csv.GetNumberParameters(i) == 2){
      parameter = csv.GetTerm(i, 1);
      if(atoi(parameter.c_str()) == 1)
        m_bLegalOn = true;
      else
        m_bLegalOn = false;
    }
    else if(asset == "show_map" && csv.GetNumberParameters(i) == 2){
      parameter = csv.GetTerm(i, 1);
      if(atoi(parameter.c_str()) == 1)
        m_bMapOn = true;
      else
        m_bMapOn = false;
    }
    else if(asset == "scale" && csv.GetNumberParameters(i)  == 2 ){//zoom
      parameter = csv.GetTerm(i, 1);
      m_scale = (float)atof(parameter.c_str());
    }
    else if(asset == "time" && csv.GetNumberParameters(i)  == 2 ){
      parameter = csv.GetTerm(i, 1);
      m_gameSeconds = atoi(parameter.c_str());
    }

    else if(asset == "track" && csv.GetNumberParameters(i) > 4){ 
      
      //for track waypoints
      int nTrackNumber;
      int nTrackX;
      int nTrackY;
      int nTrackRadius;
      CTrack track;
      int nNumWpts;

      //track number ignored
      //parameter = csv.GetTerm(i, 1);
      //nTrackNumber = atoi(parameter.c_str());
      //track.SetTrackNumber(nTrackNumber);
      nNumWpts = csv.GetNumberParameters(i);
      nNumWpts -= 2;
      nNumWpts /= 3;
      
      for(unsigned int j = 0; j < nNumWpts;++j){        
        parameter = csv.GetTerm(i, 2 + (j * 3));
        nTrackX = atoi(parameter.c_str());
        parameter = csv.GetTerm(i, 3 + (j * 3));
        nTrackY = atoi(parameter.c_str());
        parameter = csv.GetTerm(i, 4 + (j * 3));
        nTrackRadius = atoi(parameter.c_str());
        track.AddWaypoint(nTrackX, nTrackY, nTrackRadius);        
      }
      m_tracks.push_back(track);      
    }
    else if(asset == "show_tracks" && csv.GetNumberParameters(i) == 2){
      parameter = csv.GetTerm(i, 1);
      if(atoi(parameter.c_str()) == 1)
        m_bTrackOn = true;
      else
        m_bTrackOn = false;
    }
    //legal - clears map
    else if(asset == "legal" && csv.GetNumberParameters(i)  == 2 ){
      parameter = csv.GetTerm(i, 1);
      if(parameter == "clear")
        m_map.ClearMap();
    }
    //legal - sets number of cells beginning at a row/col
    else if(asset == "legal" && csv.GetNumberParameters(i)  == 4 ){
      parameter = csv.GetTerm(i, 1);
      nRow = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      nCol = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      nNumberCells = atoi(parameter.c_str());
      m_map.AddCells(nRow, nCol, nNumberCells);
    }
    else if( (asset == "terrain" || asset == "splatter") && csv.GetNumberParameters(i)  == 6 ){      

    }
    else if(asset == "wavecountdivisor" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balWavecountDivisor = atof(parameter.c_str());
    }
    else if(asset == "advancedfactor" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balAdvFactor = atof(parameter.c_str());
    }
    else if(asset == "beginnerfactor" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balBegFactor = atof(parameter.c_str());
    }
    else if(asset == "creditmultiplier" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balCreditMult = atof(parameter.c_str());
    }
    else if(asset == "scoremultiplier" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balScoreMult = atof(parameter.c_str());
    }
    else if(asset == "upgrademultiplier1" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balUpgradeMult1 = atof(parameter.c_str());
    }
    else if(asset == "upgrademultiplier2" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balUpgradeMult2 = atof(parameter.c_str());
    }
    else if(asset == "upgrademultiplier3" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balUpgradeMult3 = atof(parameter.c_str());
    }
    else if(asset == "upgradecostmultiplier1" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balUpgradeCostMult1 = atof(parameter.c_str());
    }
    else if(asset == "upgradecostmultiplier2" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balUpgradeCostMult2 = atof(parameter.c_str());
    }
    else if(asset == "upgradecostmultiplier3" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balUpgradeCostMult3 = atof(parameter.c_str());
    }
    else if(asset == "repaircostmultiplier1" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balRepairCostMult1 = atof(parameter.c_str());
    }
    else if(asset == "repaircostmultiplier2" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balRepairCostMult2 = atof(parameter.c_str());
    }
    else if(asset == "repaircostmultiplier3" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balRepairCostMult3 = atof(parameter.c_str());
    }
    else if(asset == "damagebalanceupper" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balDamageUpper = atof(parameter.c_str());
    }
    else if(asset == "damagebalancelower" && csv.GetNumberParameters(i)  == 2){
      parameter = csv.GetTerm(i, 1);
      m_balDamageLower = atof(parameter.c_str());
    }

  }
  return true;
}

bool CGameData::SaveLevelFile(int fileNum){
  if(fileNum < 1 || fileNum > 4)
    return false;

  std::string filename;
  if(fileNum == 1)
    filename = "assets\\data\\saved1.dat";
  else if(fileNum == 2)
    filename = "assets\\data\\saved2.dat";
  else if(fileNum == 3)
    filename = "assets\\data\\saved3.dat";
  else if(fileNum == 4)
    filename = "assets\\data\\saved4.dat";

  std::string line;
  std::ostringstream oss;
  
  std::ofstream file(filename.c_str());
  if(file.is_open()){
    //add header
    file << "//Golden Spike Saved Game: " << filename << std::endl;
    file << "//****************************************************" << std::endl;
    
    //game variables
    file << "//miscellaneous game variables" << std::endl;
    file << "******************************" << std::endl;
    oss.str("");
    oss << "back_color" << ", " << m_screenColorRed << ", " << m_screenColorGreen << ", " << m_screenColorBlue << std::endl;
    file << oss.str();
    
    oss.str("");    
    oss << "world" << ", " << m_worldLeft  << ", " <<  m_worldTop  << ", " << m_worldRight  << ", " <<  m_worldBottom << std::endl;
    file << oss.str();

    oss.str("");
    oss << "position" << ", " <<  m_worldX << ", " << m_worldY <<std::endl;
    file << oss.str();


    oss.str("");    
    oss << "scale" << ", " <<  m_scale << std::endl;
    file << oss.str();


    oss.str("");        
    file << std::endl;

    file.close();
  }  

    return true;
}

void CGameData::OpenLevel (int level, CGraphics &con){
  CLog *pLog = CLog::Instance();

  if(level == 1)
    LoadGameLevel("assets\\data\\level1.dat");
  else if(level == 2)
    LoadGameLevel("assets\\data\\level2.dat");
  else if(level == 3)
    LoadGameLevel("assets\\data\\level3.dat");
  else if(level == 4)
    LoadGameLevel("assets\\data\\level4.dat");

  pLog->Log("Level loaded", level);

  AddGraphicDataToLevelData(con);  
}

//primary formula to increase credits and scores
//*******************************************************
void CGameData::IncrementCredits(float health){
  float factor = 1 + (float)m_waveGen.GetWaveCount()/m_balWavecountDivisor;//change line 108
  m_credits +=(long)(health * factor * m_balCreditMult);
  m_score +=(long)(health * factor * m_balScoreMult);
}