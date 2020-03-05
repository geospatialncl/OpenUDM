#pragma once
#include <string>
#include <vector>

void ExtractCSV(const std::string& ipfile, int numItems, int extractItem, std::vector<std::string>& data);
void ExtractCSVDataColumn(const std::string& ipfile, int numItems, int extractItem, std::vector<std::string>& data);
void ExtractCSVSingleDataColumn(const std::string& ipfile, std::vector<std::string>& data);






