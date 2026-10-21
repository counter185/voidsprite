# Contribution guidelines

voidsprite accepts community-contributed help in different forms, such as reporting bugs, suggestions or opening PRs.

English is preferred for Issues and Pull requests, however the following languages are also allowed:  
Polish, Russian, Japanese, Romanian*.

*: regarding translation Issues/PRs only.

## Featured art

Art for the `Featured` section can be submitted on our Discord server.  
[More about Featured art...](https://cntrpl.itch.io/voidsprite/devlog/1644534/featured-art-submit-your-art-to-be-shown-in-voidsprite)

## Custom patterns and templates

Custom patterns and templates can be submitted using Pull requests.  
[More about custom patterns...](community-content/community-patterns)  
[More about custom templates...](community-content/community-templates/)

## Bug reports/suggestions

Report bugs and your feature suggestions using Issues. When reporting a bug, please include the following:
- the platform it occurred on
- the version of the app (seen in the bottom left corner of voidsprite Launchpad)
- the steps to reproduce it (if known)
- log file (`voidsprite_log.txt` inside the application data directory)
- any other relevant data like .voidsn project files or images. (for example, if the report is about a crash when loading a file, include a relevant sample file if possible)

## Translations

Translations can be contributed using Pull requests. Credits for translations are automatically added to the `Help` > `About voidsprite` > `Contributors` tab and do not need to be added to `CONTRIBUTORS.txt`.   
[More about translations...](freesprite/localization)

## Code

Code can be contributed using Pull requests. Before opening a PR, consider adding your name to the [CONTRIBUTORS.txt](CONTRIBUTORS.txt) file, which will show up in the `Help` > `About voidsprite` > `Contributors` tab.  
Make your changes on a new branch separate from `main`, that way we can modify the code should we need to.

### What we definitely want

voidsprite is a project centered around fun and creativity, so features that are wanted include:

- import/export support for files from other applications (especially if it's interesting legacy software)
- filters and tools that transform images in creative ways
- features that support interoperability with game engines and other creative tools
- usability improvements and comfort features

### What we do not want

- Import/export/interoperability with purely vibecoded or AI-oriented tools
- AI features like MCP servers, image or palette generators
- Integrations with non-free online services or ones that require an API key
- Code directly copied from decompilation of other programs

Every case will be different, so most things on this list aren't outright banned. If you aren't sure if a feature is something we would want, open a Draft PR.

### Code guidelines

- No specific style of code is required. However, prefer clean code over overoptimized code, unless it would be a significant improvement in speed or memory usage that would affect usability.
- If a feature is platform-specific, use compile-time if blocks to separate it: 
```cpp
#if VSP_PLATFORM != VSP_PLATFORM_WIN32
//...
#endif
```
- Any new .cpp or .h file needs to be added to the Visual Studio project (if not using Visual Studio, use a text editor to manually add it to the .vcxproj and .vcxproj.filters XMLs).
- voidsprite targets Windows 7, so try not to use functions from newer Windows APIs. If you really want to, make sure it doesn't break compatibility (for example by dynamic loading).
- When copying code from or adding an open-source library, be sure to follow its copying rules. Add its license text file to the [OPEN_SOURCE_LICENSES](OPEN_SOURCE_LICENSES) folder. 


## Generative AI (Claude, etc.)

Using generative AI tools and services to generate content including code is not allowed.  

This does not apply to machine translation tools.
