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
After creating the patch you need to edit the UI to add more check boxes, or use the _BuildingStyles.ini_ file with a
suitable Building Style Control UI template.
The check box or style ID (e.g. `0x2003`) identifies the building style to the game.    
The building style IDs are assigned in the [SimCity 4 Building Styles Index](https://community.simtropolis.com/forums/topic/763276-building-styles-master-iid/) thread, you will need to use the
16 Maxis/community styles or request your own public and/or private building style ID range in that thread.   
Finally you will need to update existing buildings to use the new building style IDs.

## Building Styles Exemplar Property (0xAA1DD400)

This property allows the custom building styles to be specified separately from the Maxis styles
in the Occupant Groups property (0xAA1DD396).

### Residential and Commercial Buildings

The _Building Styles_ property is optional for custom building style support in residential and commercial buildings. 

When the property is present, the building will use custom styles specified in the property and ignore
the styles in the Occupant Groups property (0xAA1DD396).    
When the property is not present, the building will use the custom and Maxis styles specified in the
Occupant Groups property (0xAA1DD396).

### Industrial Buildings

The _Building Styles_ property is mandatory for custom building style support in industrial buildings. 

When the property is present, the building will use custom styles specified in the property.    
When the property is not present, the building will be treated as compatible with all styles.

## Auto-Historical Check Box

The DLL supports an optional check box that will automatically mark new growable lots that use building styles as historical.
Industrial buildings are only marked as historical if industrial building styles are enabled for that building type.
This check box uses the reserved id value `0x9476D8DA`.

## Auto-Growify Check Box

The DLL supports an optional check box that will automatically convert plopped growables to use a growable zone type.
The growified lots will be marked as historical based on the value of the Auto-Historical check box described above.
This check box uses the reserved id value `0xB510A368`.

## Wall-to-Wall (W2W) Radio Buttons

The DLL supports optional radio buttons that control how the game handles W2W buildings.
The 3 radio buttons are as follows:

Mixed, both W2W and non-W2W buildings will be built. This is SC4's default behavior, and will also be used if the radio buttons are not present. This radio button uses the reserved id value `0x31150389`.    
W2W Only, only W2W buildings will be built. This radio button uses the reserved id value `0x3115038A`.    
Block W2W, no W2W buildings will be built. This radio button uses the reserved id value `0x3115038B`.

### Identifying W2W buildings 

W2W buildings are identified using either of the following methods.

#### Building Is Wall-to-Wall Exemplar Property (0xAA1DD401)

This is a Boolean property, a value of true indicates that the building is W2W.
If the value is false or the property is not present, the building will not be considered W2W.

#### Wall-to-Wall Occupant Groups

If the building has any of the following values in its Occupant Groups property (0xAA1DD396),
it indicates that the building is W2W.

BTE: Ind. W2W:     `0xD02C802E`    
BTE: Comm. W2W:    `0xB5C00A05`    
BTE: Res. W2W:     `0xB5C00B05`    
BTE: W2W General:  `0xB5C00DDE`    
SFBT: Hamburg W2W: `0xB5C00F0A`    
SFBT: Paris W2W:   `0xB5C00F0B`    

## Keep Lot Zone Sizes Check Box

The DLL supports an optional check box that stops the game from changing the user's zoned lot size in residential and
commercial zones to fit the lot it picks.
This check box combines the functionality of the _Disable Lot Aggregation_ and _Disable Lot Subdivision_ check boxes
into a single control.

When this option is enabled, it forces SC4 to only pick residential and/or commercial lots which have sizes matching
the zoned area.
When this option is disabled (the default), SC4 will use its standard behavior of aggregating or subdividing the
residential and commercial zones to fit its chosen lot.    
This check box uses the reserved id value `0x3621731B`.

## Disable Lot Aggregation Check Box

The DLL supports an optional check box that stops the game from aggregating multiple smaller lots into one larger lot
in residential and commercial zones to fit the lot it picks.

When this option is enabled, it forces SC4 to only pick residential and/or commercial lots which are the same size
or smaller than the zoned area.
When this option is disabled (the default), SC4 will use its standard behavior of aggregating the residential and
commercial zones to fit its chosen lot.    
This check box uses the reserved id value `0x102`.

## Disable Lot Subdivision Check Box

The DLL supports an optional check box that stops the game from subdividing one larger lot into multiple smaller lots
in residential and commercial zones to fit the lot it picks.

When this option is enabled, it forces SC4 to only pick residential and/or commercial lots which are the same size
or larger than the zoned area.
When this option is disabled (the default), SC4 will use its standard behavior of subdividing the residential and
commercial zones to fit its chosen lot.    
This check box uses the reserved id value `0x103`.

## Prevent Cross-Style Redevelopment Check Box

The DLL supports an optional check box that attempts to prevent the game from replacing an existing lot with one
that has a different building style.
This check box uses the reserved id value `0x104`.

## Kick Out Lower Wealth Check Boxes

The DLL supports optional check boxes that allow the user to change whether the game will kick out lower
wealth occupants when redeveloping.
These check boxes come in two versions that have the opposite behavior when checked, the UI developer can
pick which one to use.

### Kick Out Lower Wealth Check Box

This check box will allow the game to kick out lower wealth occupants when checked, and prevent that behavior when
unchecked. This check box uses the reserved id value `0x100`.

### No Kick Out Lower Wealth Check Box

This check box will prevent the game from kicking out lower wealth occupants when checked, and allow that behavior when
unchecked. This check box uses the reserved id value `0x101`.

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
