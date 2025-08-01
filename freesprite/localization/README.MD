## ◆Contribute to an existing voidsprite translation

If a language you want to add already has its localization file here, you may edit it, then PR your changes.

## ◆Make a new voidsprite translation

1. Copy `localization_english.txt` and rename it.
2. Change this line at the beginning of your copied file appropriately:
```cpp
{ "en-us", { "English", "", {
```
The third string will appear in the settings when the language is chosen and should contain the credits for the translation.  
The language name should be in that language, for example:  
`{ "pl-pl", { "Polski", "Polish translation by me", {`  

> The translation credits you enter will show up in both Preferences and in the Help > About voidsprite menu.

3. Open `localization.cpp` and add an `#include` to your copied file:
```cpp
std::map<std::string, LocalizationData> g_localizations = {
#include "localization_english.txt"
//add your localization file here
//in this case we would add this line:
#include "localization_polish.txt"
};
```

4. Building voidsprite should now make your added language show up in the settings. You may now make edits to your localization file.

> Your localization file needs to be encoded with UTF-8.  
> If the included fonts don't have required regional characters, please open an issue.

## ◆Update a localization file

You may use the included `update_localization.py` script which will add any missing translation keys from `localization_english.txt` into your localization file.  

To use this script, Python 3.7 is required.

Usage:
```bash
python update_localization.py <localization_file_name>
```
or
```bash
python update_localization.py
  Localization file name: > 
(enter your localization file name)
```
