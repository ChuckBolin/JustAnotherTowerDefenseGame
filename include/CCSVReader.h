/*************************************************************************
  CCSVReader.h - Written by Chuck Bolin, January 2009
  Purpose:  Reads CSV data files and stores information into an
  std::vector.
*************************************************************************/
#ifndef CCSV_READER_H
#define CCSV_READER_H

#include <string>
#include <vector>
#include <fstream>
#include "CLog.h"

struct PARAMETERS{
  std::vector<std::string> line;
};

class CCSVReader{
public:
  CCSVReader();
  std::string GetTerm(int row, int col);
  void LoadFile(std::string filename);
  int GetNumberParameters(int row);
  int GetTableSize(){return m_tableSize;}
  std::string RemoveComments(std::string in);

private:
  std::string CCSVReader::RemoveSpaces(std::string in);
  std::vector<PARAMETERS> m_table;
  int m_tableSize;
  int m_numberParameters;
};

#endif