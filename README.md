# sc4-more-building-styles

A DLL Plugin for SimCity 4 that allows the game to use more than 4 building styles.   

This DLL mod is intended as a dependency for other mods that add new building styles beyond the 4 that Maxis included,
it does not add any building styles to the game by itself.

Maxis programmed the Building Style Control UI to only recognize the check boxes of the 4 built-in styles,
no matter how many check boxes are actually present.
This mod patches the game's memory to remove that restriction.

## Adding More Building Styles into SC4 

Adding new style check boxes requires creating a patch that overrides the default
Building Style Control UI, TGI `0x00000000,0x96a006b0,0x6bc61f19` in `SimCity_1.dat`.    
After creating the patch you need to edit the UI to add more check boxes.
The check box ID (e.g. `0x2003`) identifies the building style to the game.    
The building style IDs are assigned in the [SimCity 4 Building Styles Index](https://community.simtropolis.com/forums/topic/763276-building-styles-master-iid/) thread, you will need to use the
16 Maxis/community styles or request your own public and/or private building style ID range in that thread.   
Finally you will need to update existing buildings to use the new building style IDs.

### Auto-Historical Check Box

The DLL supports an optional check box that will automatically mark new growable lots that use building styles as historical.
Industrial buildings are only marked as historical if industrial building styles are enabled for that building type.
This check box uses the reserved id value `0x9476D8DA`.

### Auto-Growify Check Box

The DLL supports an optional check box that will automatically convert plopped growables to use a growable zone type.
The growified lots will be marked as historical based on the value of the Auto-Historical check box described above.
This check box uses the reserved id value `0xB510A368`.

### Wall-to-Wall (W2W) Radio Buttons

The DLL supports optional radio buttons that control how the game handles W2W buildings.
The 3 radio buttons are as follows:

Mixed, both W2W and non-W2W buildings will be built. This is SC4's default behavior, and will also be used if the radio buttons are not present. This radio button uses the reserved id value `0x31150389`.    
W2W Only, only W2W buildings will be built. This radio button uses the reserved id value `0x3115038A`.    
Block W2W, no W2W buildings will be built. This radio button uses the reserved id value `0x3115038B`.

W2W buildings are identified by the presence of a W2W occupant group in the
building exemplar. The following W2W occupant group values are supported:

More Building Style DLL W2W:  `0xD02C802E`    
BTE: Comm. W2W:               `0xB5C00A05`    
BTE: Res. W2W:                `0xB5C00B05`    
BTE: W2W General:             `0xB5C00DDE`    
SFBT: Hamburg W2W:            `0xB5C00F0A`    
SFBT: Paris W2W:              `0xB5C00F0B`    

## Cheat Codes

The plugin provides the following cheat codes:

`ActiveStyle` shows an in-game message box with the ID of the currently active style when
`Change building style every N years` is selected. When `Build all styles at once` is selected, using this cheat will
display a message saying that mode is active.

`DebugActiveStyles` writes the currently enabled style IDs to the plugin's log file.

## Industrial Building Styles

Building style support for the 4 industrial building types are controlled by 4 INI file entries that should be placed
in the style mod DAT files.
See the [industrial-building-styles](industrial-building-styles) folder for a template file and the format details.

## BuildingStyles INI File

This file allows styles to be defined without editing the Building Style Control UI file.
When the check box button ids for the Building Style Control are in the range of 0-127, these ids can be mapped to a
specific style and style name in _BuildingStyles.ini_.

The style entries are placed under a _BuildingStyles_ section and use the following format:
```
<UI button id> = <style id>,<style text format>[,<style name>]

<UI button id> is a style check box id in the range of 0-127.
<style id> is the building style id assigned in the Building Style Index, or
the text Show (case-insensitive) to display a disabled placeholder check box.
<style text format> is one of the following values: N (normal text) or B (bold text).
<style name> is optional, when present it uses one of the following formats:
<name> or CaptionRes:<LTEXT group id>,<LTEXT instance id>.
If <style name> is not present, a default name will be used.

Examples:

0 = 0x2005,B,Mediterranean & Tropical
1 = 0x2006,N,CaptionRes:0x6a231eaa,0x6bbbd875
2 = Show,N,Placeholder Check Box
3 = Show,B
```

## SC4MoreBuildingStyles INI File

This file contains two settings that control whether the growable building style debug logging code will write to the
plugin's log file.
It is recommended that you only enable these options for experimentation. They will slow down your game and quickly produce
a large log file.

As an example from one of my tests, a 22 second run with both options enabled produced a ~2 MB log file with ~15,500 lines.

## For DLL Modders

### Style Changed Message

The DLL dispatches a message that when the user's selected styles change:
```cpp
// Sent when a style is checked on unchecked in the building style list.
// data1: Non-zero if the check box has been checked; otherwise, zero.
// data2: The ID of the style that has been enabled or disabled.
// data3: A cIGZString containing the style name.
static const uint32_t kMessageBuildingStyleCheckboxChanged = 0x573D5E8F;
```

The complete list of active/checked styles can be retrieved from the `cISC4TractDeveloper` class.
```cpp
cISC4AppPtr pSC4App;
if (pSC4App)
{
    cISC4City* pCity = pSC4App->GetCity();
    if (pCity)
    {
        cISC4TractDeveloper* pTractDeveloper = pCity->GetTractDeveloper();
        if (pTractDeveloper)
        {
            eastl::vector<uint32_t> activeStyles = pTractDeveloper->GetActiveStyles();
            // Do something with the active style list
        }
    }
}
```

### cIBuildingStyleInfo GZCOM Class

This class allows other DLLS to query the style ids and names for the building styles that are present in the
game's `Building Style Control` window.    
It also provides a function to get a building occupant's supported styles as a string containing a comma-separated
list of style names, only styles that are present in the game's `Building Style Control` window will be included in
that list.

See [cIBuildingStyleInfo.h](https://github.com/0xC0000054/sc4-more-building-styles/blob/main/src/public/include/cIBuildingStyleInfo.h) for details.

## Download

The plugin can be downloaded from the Releases tab: https://github.com/0xC0000054/sc4-more-building-styles/releases

## System Requirements

* SimCity 4 version 641
* Windows 10 or later

The plugin may work on Windows 7 or later with the [Microsoft Visual C++ 2022 x86 Redistribute](https://aka.ms/vs/17/release/vc_redist.x86.exe) installed, but I do not have the ability to test that.

## Installation

1. Close SimCity 4.
2. Copy `SC4MoreBuildingStyles.dll` and `SC4MoreBuildingStyles.ini` into the Plugins folder in the SimCity 4 installation directory.
3. Start SimCity 4.

## Troubleshooting

The plugin should write a `SC4MoreBuildingStyles.log` file in the same folder as the plugin.    
The log contains status information for the most recent run of the plugin.

# License

This project is licensed under the terms of the MIT License.    
See [LICENSE.txt](LICENSE.txt) for more information.

## 3rd party code

[gzcom-dll](https://github.com/nsgomez/gzcom-dll/tree/master) Located in the vendor folder, MIT License.    
[EABase](https://github.com/electronicarts/EABase) Located in the vendor folder, BSD 3-Clause License.    
[EASTL](https://github.com/electronicarts/EASTL) Located in the vendor folder, BSD 3-Clause License.    
[Windows Implementation Library](https://github.com/microsoft/wil) - MIT License    
[SC4Fix](https://github.com/nsgomez/sc4fix) - MIT License.    
[Frozen](https://github.com/serge-sans-paille/frozen) - Apache 2.0 License.    
[Boost.Algorithm](https://www.boost.org/doc/libs/1_84_0/libs/algorithm/doc/html/index.html) - Boost Software License, Version 1.0.    
[Boost.PropertyTree](https://www.boost.org/doc/libs/1_84_0/doc/html/property_tree.html) - Boost Software License, Version 1.0.

# Source Code

## Prerequisites

* Visual Studio 2022

## Building the plugin

* Open the solution in the `src` folder
* Update the post build events to copy the build output to you SimCity 4 application plugins folder.
* Build the solution

## Debugging the plugin

Visual Studio can be configured to launch SimCity 4 on the Debugging page of the project properties.
I configured the debugger to launch the game in a window with the following command line:    
`-intro:off -CPUcount:1 -w -CustomResolution:enabled -r1920x1080x32`

You may need to adjust the resolution for your screen.
