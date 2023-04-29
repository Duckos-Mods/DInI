#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <tuple>
#include <variant>
#include <type_traits>
#include <algorithm>


/*
 *  Made by Duckos Mods
 *
 *  LICENSE: zlib/libpng
 *
 *  This software is provided "as-is", without any express or implied warranty. In no event
 *  will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial
 *  applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not claim that you
 *    wrote the original software. If you use this software in a product, an acknowledgment
 *    in the product documentation would be appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *    as being the original software.
 *
 *    3. This notice may not be removed or altered from any source distribution.
 */

namespace DInI {
    #define DAPI inline
    namespace StringUtil {
        DAPI std::vector < std::string > explode(const std::string& data,
            // Returns a vector of strings, That gets created from one string being split on every user defined character, EG split on spaces 
            const std::string& delimiters) {
            auto is_delim = [&](auto& c) {
                return delimiters.find(c) != std::string::npos;
            };
            std::vector < std::string > result;
            for (std::string::size_type i(0), len(data.length()), pos(0); i <= len; i++) {
                if (is_delim(data[i]) || i == len) {
                    auto tok = data.substr(pos, i - pos);
                    tok.erase(std::remove_if(tok.begin(), tok.end(), isspace), tok.end());
                    if (!tok.empty())
                        result.push_back(tok);
                    pos = i + 1;
                }
            }
            return result;
        }
    }
    struct InISection {
        std::string sectionName = "Section";
        std::unordered_map < std::string, std::variant < std::string, std::vector < std::string >>> sectionData;
    };
    /// <summary>
    /// Loads An Ini file from a path
    /// </summary>
    class ini {
    public: ini(std::string FilePath) {
        _filePath = FilePath;
        _iniFile.open(FilePath);

        if (!_iniFile) {
            throw std::runtime_error("Failed to open file: " + FilePath);
        }
        parseFile();
    }
          ini() {}
          /// <summary>
          /// Opens the specified file
          /// </summary>
          /// <param name="FilePath"></param>
          /// <returns></returns>
          DAPI void open(std::string FilePath) {

              _filePath = FilePath;
              _iniFile.open(FilePath);

              if (!_iniFile) {
                  throw std::runtime_error("Failed to open file: " + FilePath);
              }
          }

          /// <summary>
          /// Make sure to call this function if you used the open function before trying to do anything like get set or save because this gets the data from the ini file and sorts it
          /// </summary>
          /// <returns></returns>
          DAPI void parseFile() {
              std::string Line;
              std::string SectionName;
              while (std::getline(_iniFile, Line)) {
                  if (Line[0] == ';' || Line[0] == '#' || Line.empty()) {
                      continue;
                  }

                  if (Line[0] == '[' && Line.back() == ']') {
                      SectionName = Line.substr(1, Line.size() - 2);
                      continue;
                  }

                  std::stringstream ss(Line);
                  std::string key, value;
                  if (std::getline(ss, key, '=') && std::getline(ss, value)) {

                      std::string trimmedKey = key;
                      size_t lastNonSpaceIndex = trimmedKey.find_last_not_of(" ");
                      if (lastNonSpaceIndex != std::string::npos) {
                          trimmedKey = trimmedKey.substr(0, lastNonSpaceIndex + 1);
                      }
                      std::remove(value.begin(), value.end(), ' ');
                      bool Array;
                      if (value[0] == '[' && value.back() == ']') {
                          Array = true;
                      }
                      else {
                          Array = false;
                      }

                      if (updatedSections.count(SectionName) == 0) {
                          if (!Array) {
                              InISection loadedInIData;
                              loadedInIData.sectionName = SectionName;
                              loadedInIData.sectionData[trimmedKey] = value;
                              updatedSections[SectionName] = loadedInIData;
                          }
                          else {
                              std::string FullArrayString = value.substr(1, value.size() - 2);
                              InISection loadedInIData;
                              loadedInIData.sectionName = SectionName;
                              loadedInIData.sectionData[trimmedKey] = StringUtil::explode(FullArrayString, ",");
                              updatedSections[SectionName] = loadedInIData;
                          }
                      }
                      else {
                          if (!Array) {

                              // Doing it with pointers because i was testing a bug but it is more memory efficent to do it this way anyway 
                              InISection* loadedInIData = &updatedSections[SectionName];
                              loadedInIData->sectionData[trimmedKey] = value;
                          }
                          else {
                              std::string FullArrayString = value.substr(1, value.size() - 2);
                              InISection* loadedInIData = &updatedSections[SectionName];
                              loadedInIData->sectionData[trimmedKey] = StringUtil::explode(FullArrayString, ",");
                          }
                      }
                  }

              }
          }

          /// <summary>
          /// Returns A std::variant only use if you dont know the type of the key and need to get it at run time. Else use the stringGet function and vectorGet
          /// </summary>
          /// <param name="SectionName"></param>
          /// <param name="key"></param>
          /// <returns></returns>
          DAPI const std::variant <std::string, std::vector <std::string>>& get(std::string SectionName, std::string key) {
              if (updatedSections.count(SectionName) == 0) {
                  throw std::runtime_error("Section not found: " + SectionName);
              }
              if (updatedSections.at(SectionName).sectionData.count(key) == 0) {
                  throw std::runtime_error("Key not found: " + key);
              }
              return updatedSections.at(SectionName).sectionData.at(key);
          }
          DAPI const std::string& stringGet(std::string SectionName, std::string key) {
              if (updatedSections.count(SectionName) == 0) {
                  throw std::runtime_error("Section not found: " + SectionName);
              }
              if (updatedSections.at(SectionName).sectionData.count(key) == 0) {
                  throw std::runtime_error("Key not found: " + key);
              }
              return std::get < std::string >(updatedSections.at(SectionName).sectionData.at(key));
          }
          DAPI const std::vector <std::string>& vectorGet(std::string SectionName, std::string key) {
              if (updatedSections.count(SectionName) == 0) {
                  throw std::runtime_error("Section not found: " + SectionName);
              }
              if (updatedSections.at(SectionName).sectionData.count(key) == 0) {
                  throw std::runtime_error("Key not found: " + key);
              }
              return std::get < std::vector < std::string >>(updatedSections.at(SectionName).sectionData.at(key));
          }

          /// <summary>
          /// Changes the Value at the given Section and Key
          /// </summary>
          /// <param name="SectionName"></param>
          /// <param name="key"></param>
          /// <param name="value"></param>
          /// <returns></returns>
          DAPI void set(const std::string& SectionName,
              const std::string& key,
              const std::string& value) {
              updatedSections[SectionName].sectionData[key] = value;
          }

          /// <summary>
          /// Changes the Value at the given Section and Key
          /// </summary>
          /// <param name="SectionName"></param>
          /// <param name="key"></param>
          /// <param name="value"></param>
          /// <returns></returns>
          DAPI void set(const std::string& SectionName,
              const std::string& key,
              const std::vector < std::string > value) {
              updatedSections[SectionName].sectionData[key] = value;
          }

          /// <summary>
          /// Saves the current version of the ini file in memory back to the ini file that was opened
          /// Has an overload that takes in a path to save a new file to
          /// </summary>
          /// <returns></returns>
          DAPI void save() const {

              std::ofstream file(_filePath);
              if (!file) {
                  throw std::runtime_error("Failed to open file: " + _filePath);
              }
              for (auto& [SectionName, sectionData] : updatedSections) {
                  file << "[" << SectionName << "]\n";
                  for (auto& [Key, Val] : sectionData.sectionData) {
                      file << Key << " = ";
                      std::visit(
                          [&](auto&& arg) {

                              if constexpr (std::is_same_v < std::decay_t < decltype(arg) >, std::string >) {
                                  file << arg << '\n';
                              }
                              else {
                                  file << "[";
                                  for (auto it = arg.begin(); it != arg.end(); ++it) {
                                      file << *it;
                                      if (it + 1 != arg.end()) {
                                          file << ", ";
                                      }
                                  }
                                  file << "]\n";
                              }
                          }, Val);
                  }
              }
              file.close();
          }

          DAPI void save(std::string Path) const {

              std::ofstream file(Path);
              if (!file) {
                  throw std::runtime_error("Failed to open file: " + Path);
              }

              for (auto& [SectionName, sectionData] : updatedSections) {
                  file << "[" << SectionName << "]\n";
                  for (auto& [Key, Val] : sectionData.sectionData) {
                      file << Key << " = ";
                      std::visit(
                          [&](auto&& arg) {

                              if constexpr (std::is_same_v < std::decay_t < decltype(arg) >, std::string >) {
                                  file << arg << '\n';
                              }
                              else {
                                  file << "[";

                                  for (auto it = arg.begin(); it != arg.end(); ++it) {
                                      file << *it;
                                      if (it + 1 != arg.end()) {
                                          file << ", ";
                                      }
                                  }
                                  file << "]\n";
                              }
                          }, Val);
                  }
              }
              file.close();
          }

          /// <summary>
          /// Only should be used for debugging. Prints the ini file that it has in memory to the console
          /// </summary>
          /// <returns></returns>
          DAPI void dump() {
              for (auto& [SectionName, sectionData] : updatedSections) {
                  std::cout << "[" << SectionName << "]\n";
                  for (auto& [Key, Val] : sectionData.sectionData) {
                      std::cout << Key << " = ";
                      std::visit(
                          [](auto&& arg) {

                              if constexpr (std::is_same_v < std::decay_t < decltype(arg) >, std::string >) {
                                  std::cout << arg << '\n';
                              }
                              else {
                                  std::cout << "[";
                                  for (auto it = arg.begin(); it != arg.end(); ++it) {
                                      std::cout << *it;
                                      if (it + 1 != arg.end()) {
                                          std::cout << ", ";
                                      }
                                  }
                                  std::cout << "]\n";
                              }
                          }, Val);
                  }
              }
          }

          /// <summary>
          /// Returns the whole ini file as it is stored in memory. Only should be used if you know what your doing. Note also copies the file so is slow
          /// </summary>
          /// <returns></returns>
          DAPI std::unordered_map < std::string,
              DInI::InISection > sectionsC() {
              return updatedSections;
          }

          /// <summary>
          /// Returns the whole ini file as it is stored in memory. Only should be used if you know what your doing
          /// </summary>
          /// <returns></returns>
          DAPI std::unordered_map < std::string,
              DInI::InISection >* sectionsP() {
              return &updatedSections;
          }

          /// <summary>
          /// Returns the ini file as a string
          /// </summary>
          DAPI std::string iniToString()
          {
              std::string returnString = "";
              for (auto& [SectionName, sectionData] : updatedSections) {
                  returnString += "\n[" + SectionName + "]\n";
                  for (auto& [Key, Val] : sectionData.sectionData) {
                      returnString += Key + " = ";
                      std::visit(
                          [&returnString](auto&& arg) {

                              if constexpr (std::is_same_v < std::decay_t < decltype(arg) >, std::string >) {
                                  returnString += arg + '\n';
                              }
                              else {
                                  returnString += "[";
                                  for (auto it = arg.begin(); it != arg.end(); ++it) {
                                      returnString += *it;
                                      if (it + 1 != arg.end()) {
                                          returnString += ", ";
                                      }
                                  }
                                  returnString += "]\n";
                              }
                          }, Val);
                  }
              }
              return returnString;
          }

          std::unordered_map <std::string,
                                DInI::InISection> updatedSections;
    private: std::fstream _iniFile;
           std::string _filePath;
    };
}
