# sc4-more-building-styles

A DLL Plugin for SimCity 4 that allows the game to use more than 4 building styles.   

This DLL mod is intended as a dependency for other mods that add new building styles beyond the 4 that Maxis included,
it does not add any building styles to the game by itself.

Maxis programmed the Building Style Control UI to only recognize the check boxes of the 4 built-in styles,
no matter how many check boxes are actually present.
This mod patches the game's memory to remove that restriction.

## Adding More Building Styles into SC4 

The building style IDs are assigned in the [SimCity 4 Building Styles Index](https://community.simtropolis.com/forums/topic/763276-building-styles-master-iid/) thread, you will need to use the
Maxis/community styles or request your own public and/or private building style ID range in that thread.  

### Modifying the Building Styles UI

The easiest way to get started is to download the Building Style UI template, but you can also create your own Building Style Control override plugin.

#### Building Style UI Template

A UI template is available [here](https://community.simtropolis.com/forums/topic/762969-allow-more-building-styles-dll-plugin/?do=findComment&comment=1800509), it uses [BuildingStyles.ini](https://github.com/0xC0000054/sc4-more-building-styles#buildingstyles-ini-file),
and supports all of the [new controls](https://github.com/0xC0000054/sc4-more-building-styles/blob/main/docs/Building_Style_UI_Controls.md) the DLL adds. 
Once you have configured your styles in _BuildingStyles.ini_, see the section below on assigning buildings to custom styles.

#### Creating a Custom Building Style UI

1. Create a patch that overrides the default Building Style Control UI, TGI `0x00000000,0x96a006b0,0x6bc61f19` in `SimCity_1.dat`.    
2. Edit the UI to add the style check boxes, and optionally the [new check boxes and radio buttons](https://github.com/0xC0000054/sc4-more-building-styles/blob/main/docs/Building_Style_UI_Controls.md) that the DLL supports.
The check box or style ID (e.g. `0x2003`) identifies the building style to the game.  
3. See the section below on assigning buildings to custom styles.

### Adding Buildings to Custom Building Styles

This process is performed with ilive's Reader or PIMX.

The building's intended styles should be added to the building exemplar using the _Building Styles_ property (property id 0xAA1DD400).
The building will only grow when one or more of the custom or Maxis styles in that property is selected in the game's UI.
The DLL will ignore the building styles in the _Occupant Groups_ property (property id 0xAA1DD396) when the _Building Styles_ property is present.

For compatibility with players who are not using the DLL, the Maxis styles in the _Occupant Groups_ property should be left alone.
If you are creating a new building, add the _Building Styles_ property for users with the DLL and set the _Occupant Groups_ property to one
or more of the 4 Maxis styles for users without the DLL.

For Wall-to-Wall (W2W) buildings, add the _Building Is Wall-to-Wall_ property (property id 0xAA1DD401) to the building's exemplar with the
value set to _true_.
If the _Building Is Wall-to-Wall_ property is not present or the value is set to false, the building will be treated as non-W2W.

See the _Building Styles Exemplar Property (0xAA1DD400)_ and _Building Is Wall-to-Wall Exemplar Property (0xAA1DD401)_ sections for more details.

## New Building Exemplar Properties

The DLL adds the following exemplar properties to configure the custom building styles.

### Building Styles Exemplar Property (0xAA1DD400)

This property allows the custom building styles to be specified separately from the Maxis styles
in the Occupant Groups property (0xAA1DD396).

#### Residential and Commercial Buildings

When the property is present, the building will use custom styles specified in the property and ignore
the styles in the Occupant Groups property (0xAA1DD396).    
When the property is not present, the building will use the custom and Maxis styles specified in the
Occupant Groups property (0xAA1DD396).

#### Industrial Buildings

When the property is present, the building will use custom styles specified in the property.    
When the property is not present, the building will be treated as compatible with all styles.

### Building Is Wall-to-Wall Exemplar Property (0xAA1DD401)

This is a Boolean property, a value of true indicates that the building is W2W.
If the value is false or the property is not present, the building will not be considered W2W.
This is the preferred method for identifying W2W buildings that do not already have one of the W2W occupant groups.

## Building Style Check Box Tool Tips

The individual building styles can have a tool tip that describes the style when the user hovers over that check box.

All of these tool tips use the LTEXT group id `0x3EE5B610`, the group id supports the standard language offset scheme.
The style id is used as the LTEXT instance id.

For example, the 0x2004 building style in would use the following TGI in German: `0x2026960B, 0x3EE5B614, 0x00002004`.

Tool tips can optionally have a title in addition to a body, this is done by placing both on a single line with a pipe separator.    
For example, `ToolTip Title|Tool tip body`.   
Note that how the game displays tool tips with both a title and body will depend on the user's game settings.
If the user has unchecked the _expanded tooltips_ check box in the play options dialog, the game will display the tool tip
title (if present) or the body, but not both.

## Cheat Codes

The plugin provides the following cheat codes:

`ActiveStyle` shows an in-game message box with the ID of the currently active style when
`Change building style every N years` is selected. When `Build all styles at once` is selected, using this cheat will
display a message saying that mode is active.

`DebugActiveStyles` writes the currently enabled style IDs to the plugin's log file.

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

This file contains settings that control whether the growable debug logging code will write to the plugin's log file.
It is recommended that you only enable these options for experimentation. They will slow down your game and quickly produce
a large log file.

As an example from one of my tests, a 22 second run with the _BuildingStyleSelection_ and _LotStyleSelection_ options enabled 
produced a ~2 MB log file with ~15,500 lines.

## For DLL Modders

This DLL provides several types for use by other DLL plugins, see the [documentation](https://github.com/0xC0000054/sc4-more-building-styles/blob/main/docs/For_DLL_Plugins.md) for more details.

## Download

The plugin can be downloaded from the Releases tab: https://github.com/0xC0000054/sc4-more-building-styles/releases

## System Requirements

* SimCity 4 version 641
* Windows 10 or later

The plugin may work on Windows 7 or later with the [Microsoft Visual C++ 2022 x86 Redistribute](https://aka.ms/vs/17/release/vc_redist.x86.exe) installed, but I do not have the ability to test that.

## Installation

1. Close SimCity 4.
2. Copy _SC4MoreBuildingStyles.dll_, _SC4MoreBuildingStyles.ini_ and _BuildingStyles.ini_ into the top-level of the Plugins folder in the SimCity 4 installation directory or Documents/SimCity 4 directory.
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
