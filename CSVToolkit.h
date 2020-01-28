#pragma once
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

int GetIntFromString(std::string input);
float GetFloatFromString(std::string input);
double GetDoubleFromString(std::string input);

void ExtractCSV(std::string ipfile, int numItems, int extractItem, std::string* data);
void ExtractCSVDataColumn(std::string ipfile, int numItems, int extractItem, std::string* data);
void ExtractCSVSingleDataColumn(std::string ipfile, std::string* data);






