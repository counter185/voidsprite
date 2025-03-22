## Contribute to an existing voidsprite translation

If a language you want to add already has its localization file here, you may edit it, then PR your changes.

## Make a new voidsprite translation

1. Copy `localization_english.txt` and rename it.
2. Change this line at the beginning of your copied file appropriately:
```cpp
{ "en-us", { "English", {
```
The language name should be in that language, for example:  
`{ "pl-pl", { "Polski" {`  

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
> If there's an issue with fonts not having required regional characters, please open an issue.