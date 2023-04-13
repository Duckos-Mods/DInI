#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iostream>

/*
* Made by Duckos Mods
* 
* Feel free to do what you want with this just credit me when using it
* 
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

namespace DInI
{
	#define DAPI inline
	/// <summary>
	/// Loads An Ini file from a path ONLY supports loading the ini for a std::string path might add support for ofstreams
	/// </summary>
	class ini
	{
	public:
		ini(std::string FilePath)
		{
			_filePath = FilePath;
			_iniFile.open(FilePath);

			if (!_iniFile) {
				throw std::runtime_error("Failed to open file: " + FilePath);
			}
		}
		ini() {}
		/// <summary>
		/// Opens the specified file
		/// </summary>
		/// <param name="FilePath"></param>
		/// <returns></returns>
		DAPI void open(std::string FilePath)
		{

			_filePath = FilePath;
			_iniFile.open(FilePath);

			if (!_iniFile) {
				throw std::runtime_error("Failed to open file: " + FilePath);
			}
		}

		/// <summary>
		/// Make sure to call this function before trying to do anything like get set or save because this gets the data from the ini file and sorts it
		/// </summary>
		/// <returns></returns>
		DAPI void parseFile()
		{
			std::string Line;
			std::string SectionName;
			while (std::getline(_iniFile, Line))
			{
				if (Line[0] == ';' || Line[0] == '#' || Line.empty())
				{
					continue;
				}

				if (Line[0] == '[' || Line.back() == ']')
				{
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
					_sections[SectionName][trimmedKey] = value;
				}

			}
		}

		/// <summary>
		/// Returns as a string the value the given Section and key. 
		/// </summary>
		/// <param name="SectionName"></param>
		/// <param name="key"></param>
		/// <returns></returns>
		DAPI const std::string& get(std::string SectionName, std::string key)
		{
			if (_sections.count(SectionName) == 0) {
				throw std::runtime_error("Section not found: " + SectionName);
			}
			if (_sections.at(SectionName).count(key) == 0) {
				throw std::runtime_error("Key not found: " + key);
			}
			return _sections.at(SectionName).at(key);
		}

		/// <summary>
		/// Changes the Value at the given Section and Key
		/// </summary>
		/// <param name="SectionName"></param>
		/// <param name="key"></param>
		/// <param name="value"></param>
		/// <returns></returns>
		DAPI void set(const std::string& SectionName, const std::string& key, const std::string& value) {
			_sections[SectionName][key] = value;
		}

		/// <summary>
		/// Saves the current version of the ini file in memory back to the ini file that was opened
		/// Has an overload that takes in a path to save a new file to
		/// </summary>
		/// <returns></returns>
		DAPI void save() const
		{

			std::ofstream file(_filePath);
			if (!file) {
				throw std::runtime_error("Failed to open file: " + _filePath);
			}
			for (const auto& [SectionName, section_data] : _sections) {
				file << "[" << SectionName << "]\n";
				for (const auto& [key, value] : section_data) {
					file << key << "=" << value << "\n";
				}
				file << "\n";
			}
		}

		DAPI void save(std::string Path) const
		{

			std::ofstream file(Path);
			if (!file) {
				throw std::runtime_error("Failed to open file: " + Path);
			}
			for (const auto& [SectionName, section_data] : _sections) {
				file << "[" << SectionName << "]\n";
				for (const auto& [key, value] : section_data) {
					file << key << "=" << value << "\n";
				}
				file << "\n";
			}
		}


		/// <summary>
		/// Only should be used for debugging. Prints the ini file that it has in memory to the console
		/// </summary>
		/// <returns></returns>
		DAPI void dump()
		{
			for (const auto& section : _sections) {
				std::cout << "[" << section.first << "]\n";
				for (const auto& keyValuePair : section.second) {
					std::cout << keyValuePair.first << " = " << keyValuePair.second << "\n";
				}
			}
		}

		/// <summary>
		/// Returns the whole ini file as it is stored in memory. Only should be used if you know what your doing. Note also copies the file so is slow
		/// </summary>
		/// <returns></returns>
		DAPI std::unordered_map<std::string, std::unordered_map<std::string, std::string>> Sections()
		{
			return _sections;
		}

	private:
		std::ifstream _iniFile;
		std::string _filePath;
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> _sections;
	};
}
