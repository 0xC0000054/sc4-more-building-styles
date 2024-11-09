# For DLL Plugins

This files lists the messages and GZCOM classes that the MoreBuildingStyles DLL provides for
other DLL plugins to use.

## Style Changed Message

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

## cIBuildingStyleInfo GZCOM Class

This class allows other DLLS to query the style ids and names for the building styles that are present in the
game's `Building Style Control` window.    
It also provides a function to get a building occupant's supported styles as a string containing a list of style names,
only styles that are present in the game's `Building Style Control` window will be included in that list.

See [cIBuildingStyleInfo2.h](https://github.com/0xC0000054/sc4-more-building-styles/blob/main/src/public/include/cIBuildingStyleInfo2.h) for details.

## cIBuildingStyleWallToWall GZCOM Class

This class allows other DLLS to get a list of the supported wall-to-wall (W2W) occupant groups, and query if a building has one of those
occupant groups.

See [cIBuildingStyleWallToWall.h](https://github.com/0xC0000054/sc4-more-building-styles/blob/main/src/public/include/cIBuildingStyleWallToWall.h) for details.