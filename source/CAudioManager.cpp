#include "..\include\CAudioManager.h"

CAudioManager *CAudioManager::pInstance = 0;

CAudioManager *CAudioManager::Instance(){
  if(CAudioManager::pInstance == 0)
    CAudioManager::pInstance = new CAudioManager;

  //else
  return CAudioManager::pInstance;
}

//initialize sound system
CAudioManager::CAudioManager(){
  CLog *pLog = CLog::Instance();

	//allocate memory for the FMOD system object
	FMOD_RESULT  result = FMOD::System_Create(&m_pSystem);
	//if(result == FMOD_OK){
	//	::MessageBox(0, "FMOD System created OK!", "Success!", 0);
	//}

	//initialize the FMOD system object
	m_pSystem->init(32, FMOD_INIT_NORMAL, NULL); 
	if(result == FMOD_OK){
		pLog->Log("FMOD Initialized");
	}
  else
    pLog->Log("FMOD failed to initialize!");

	m_soundCount = 0; //counts number of files loaded

	//created sound object and specify the sound
	//result = m_pSystem->createSound("sounds/ambient1.mp3", FMOD_LOOP_NORMAL,NULL, &m_pSound[0]);
	//result = m_pSystem->createSound("sounds/foreman.mp3", FMOD_LOOP_NORMAL,NULL, &m_pSound[1]);
	m_soundCount = 1;

	// play sound - 1st parameter can be combined flags (| separator)
	for(int i = 0; i < MAX_SOUNDS; ++i){
		m_pChannel[i] = 0;
		m_bPauseSound[i] = false;
	}
	
}

void CAudioManager::LoadFile(std::string filename){
  CLog *pLog = CLog::Instance();
  FMOD_RESULT  result;
	m_soundCount = 0; //counts number of files loaded

	//created sound object and specify the sound
	//load from file
  pLog->Log("Reading audio data file", filename);
	CCSVReader CSV;
	CSV.LoadFile(filename);
  std::string soundFile = "";
	if(CSV.GetTableSize() > 0){
		for(int i = 0; i < CSV.GetTableSize(); ++i){
      
			if(CSV.GetNumberParameters(i) == 3){
        soundFile = CSV.GetTerm(i,1);
        m_filename[m_soundCount] = soundFile;
        pLog->Log("Audio file loaded: ", soundFile);
        //std::cout << i << " " << soundFile << "  " << CSV.GetTerm(i,2) << std::endl;
				result = m_pSystem->createSound(soundFile.c_str(), FMOD_LOOP_OFF, NULL, &m_pSound[i]);			
        m_soundCount++;
			}
		}
	}

	// play sound - 1st parameter can be combined flags (| separator)
	for(int i = 0; i < MAX_SOUNDS; ++i){
		m_pChannel[i] = 0;
		m_bPauseSound[i] = false;
	}
	
}

//close sound system if it initialized correctly
CAudioManager::~CAudioManager(){
	FMOD_RESULT  result;

	// release resources
	for(int i = 0; i < MAX_SOUNDS; ++i)
	  result = m_pSound[i]->release();
	
	result = m_pSystem->close();
	result = m_pSystem->release();
}

void CAudioManager::PlayAudio(int num){
  CLog *pLog = CLog::Instance();
	if(num < 0 || num > m_soundCount - 1)
		return;

  FMOD_RESULT  result = m_pChannel[num]->isPlaying(&m_bPauseSound[num]);
  if(result != FMOD_OK)
    pLog->Log("isPlaying ", num,FMOD_ErrorString(result));
  if(m_bPauseSound[num] == false)			{
	  result = m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pSound[num], false, &m_pChannel[num]);
    pLog->Log("playSound", FMOD_ErrorString(result));
  }
  
}

void CAudioManager::StopSound(int num){
	if(num < 0 || num > m_soundCount - 1)
		return;

	m_pChannel[num]->stop();
}

void CAudioManager::KillAll(){
  for(int i = 0; i < m_soundCount; ++i)
    m_pChannel[i]->stop();
}

void CAudioManager::PauseSound(int num){
	if(num < 0 || num > m_soundCount - 1)
		return;

	m_pChannel[num]->setPaused(true);
}

std::string CAudioManager::GetFilename(int num){
  std::string temp = "";
  if(num < 0 || num > m_soundCount - 1)
		return temp;

  return m_filename[num];
}

void CAudioManager::UnpauseSound(int num){
	if(num < 0 || num > m_soundCount - 1)
		return;
	m_pChannel[num]->setPaused(false);
}

void CAudioManager::PauseAll(){
  for(int i = 0; i < m_soundCount; ++i)
    m_pChannel[i]->setPaused(true);
}

void CAudioManager::UnpauseAll(){
  for(int i = 0; i < m_soundCount; ++i)
    m_pChannel[i]->setPaused(false);
}

void CAudioManager::Update(){
   	m_pSystem->update();
}