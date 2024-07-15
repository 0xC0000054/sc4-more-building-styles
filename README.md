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
The check box ID (e.g. `0x2003`) identifies the building style to the game, these IDs should be in the range of 0x2000 to 0x2fff.
Finally you will need to update existing buildings to use the new building style IDs.

### Auto-Historical Check Box

The DLL supports an optional check box that will automatically mark new growable residential and commercial lots as historical.
This check box uses the reserved id value `0x9476D8DA`.

## Cheat Codes

The plugin provides the following cheat codes:

`ActiveStyle` shows an in-game message box with the ID of the currently active style when
`Change building style every N years` is selected. When `Build all styles at once` is selected, using this cheat will
display a message saying that mode is active.

`DebugActiveStyles` writes the currently enabled style IDs to the plugin's log file.

## For DLL Modders

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

## Download

The plugin can be downloaded from the Releases tab: https://github.com/0xC0000054/sc4-more-building-styles/releases

## System Requirements

* SimCity 4 version 641
* Windows 10 or later

The plugin may work on Windows 7 or later with the [Microsoft Visual C++ 2022 x86 Redistribute](https://aka.ms/vs/17/release/vc_redist.x86.exe) installed, but I do not have the ability to test that.

## Installation

1. Close SimCity 4.
2. Copy `SC4MoreBuildingStyles.dll` into the Plugins folder in the SimCity 4 installation directory.
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
