#pragma once
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

int GetIntFromString(const std::string& input);
float GetFloatFromString(const std::string& input);
double GetDoubleFromString(const std::string& input);

void ExtractCSV(const std::string& ipfile, int numItems, int extractItem, std::vector<std::string>& data);
void ExtractCSVDataColumn(const std::string& ipfile, int numItems, int extractItem, std::vector<std::string>& data);
void ExtractCSVSingleDataColumn(const std::string& ipfile, std::vector<std::string>& data);






