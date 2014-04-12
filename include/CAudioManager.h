//****************************************************************************
// Class: CAudioManager
// Purpose: Manages FMOD Library for Chuck's Game, September 23, 2007
// Websites used as references:
//		http://www.fmod.org/docs/HTML/	FMOD Organization
//link to fmodex_vc.lib
//add fmodex.dll to current folder for distributions
//
//    CAudioManager *pAudio = CAudioManager::Instance();
//
//Revision:
//  March 28, 2008  Added fade in and fade out features  CB
//  March 29, 2008  Add stream support
//  June 16, 2008   Loaded assets from file.
//  May 31, 2009	Replaced FMOD 3 with FMOD Ex
//****************************************************************************

//CAudioManager.h
#ifndef CAUDIOMANAGER_H
#define CAUDIOMANAGER_H

#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <string>
#include <cstring>
#include <vector>
#include <fmod.h>
#include <fmod.hpp>
#include <fmod_errors.h>
#include "CCSVReader.h"
#include "CLog.h"

const int MAX_SOUNDS = 32;

class CAudioManager{
public:
	static CAudioManager *Instance();
	void PlayAudio(int num);
	void StopSound(int num);
	void PauseSound(int num);
	void UnpauseSound(int num);
	void Update();
  void LoadFile(std::string filename);
  void KillAll();
  void PauseAll();
  void UnpauseAll();
  std::string GetFilename(int num);
 
protected:
	CAudioManager();
	~CAudioManager();

private:
	static CAudioManager *pInstance;
	FMOD::System* m_pSystem;
	FMOD::Sound* m_pSound[MAX_SOUNDS];//declare variable for the sound object	
	FMOD::Channel* m_pChannel[MAX_SOUNDS];
  bool m_bPauseSound[MAX_SOUNDS];
  std::string m_filename[MAX_SOUNDS];
	int m_soundCount;
};

#endif

