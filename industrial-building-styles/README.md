# Industrial Building Style Template

This folder contains a DBPF file with the 4 INI entries that are used to enable building styles for the industrial building categories.    
Building style support for the specified building category, the supported values of the _Enabled_ field are:

False - building styles are disabled for the building category.
True  - building styles are enabled for the building category.

Both values are case insensitive.

When building styles are disabled for an industrial building category, that building category will
behave as if it is compatible with all active building styles. This is the game's built-in behavior for the
industrial building categories.
If building styles enabled for an industrial building category, it will act as a blocker on industrial development
for that building category unless there is a pool of buildings that are compatible with the active styles. 

# Making Your Own INI Files

The TGI values and INI files section name varies depending on the industrial building category, see the sections
below for details.

All of the INI file sections must have a single field named _Enabled_, whose value is _True_ or _False_.

## Industrial Resource/Agriculture/Farm

The INI File entry has the TGI: 0x00000000, 0x8A5971C5, 0xC82CA0DC.
The INI section must be named _IRBuildingStyles_.

## Industrial Dirty/Processing

The INI File entry has the TGI: 0x00000000, 0x8A5971C5, 0xC82CA0DD.
The INI section must be named _IDBuildingStyles_.

## Industrial Manufacturing

The INI File entry has the TGI: 0x00000000, 0x8A5971C5, 0xC82CA0DE.
The INI section must be named _IMBuildingStyles_.

## Industrial High Tech

The INI File entry has the TGI: 0x00000000, 0x8A5971C5, 0xC82CA0DF.
The INI section must be named _IHTBuildingStyles_.
