# sc4-more-building-styles

A DLL Plugin for SimCity 4 that adds support for more building styles.   

This mod patches the game's memory to remove the default restriction on the number of styles
that the Building Select Dialog can support.

The plugin also includes the following features for developers:

A `DebugActiveStyles` cheat, this cheat will write the currently enabled style IDs to the log file.    
A message that fires when the user's selected styles change that other DLLs can subscribe to:
```cpp
// Sent when a style is checked on unchecked in the building style list.
// data1: Non-zero if the check box has been checked; otherwise, zero.
// data2: The ID of the style that has been enabled or disabled.
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
