# DInI
DInI is a header only C++ ini parsing lib and is meant to make parsing ini files easy 

# Docs

```cpp
#include "ini.hpp"

int main()
{
  DInI::ini IniFile("test.ini");
  IniFile.parseFile();
  
  std::cout << IniFile.get("Section", "Key") << "\n; // Returns the value stored in the ini file at the specified Section and Key
  
  IniFile.set("Section", "Key", "Hello World!"/* Allways Has to be a string*/);
  
  IniFile.save(); // Saves the current ini file to the opened ini file (Over writes the file)
  IniFile.save("test2.ini"); // Saves the current ini file to a new ini file (creates a new file)
  
  IniFile.dump() // Only should be used for debugging. Prints the ini file that it has in memory to the console
  
  auto InI = IniFile.Sections(); // Returns the whole ini file as it is stored in memory. Only should be used if you know what your doing. Note also copies the file so is slow
  
}
```
