# Industrial Building Style Template

This folder contains a DBPF file with the 4 INI entries that are used to enable building styles for the industrial zones.    
Building style support for the specified zone, the supported values of the _Enabled_ field are:

False - building styles are disabled for the zone.
True  - building styles are enabled for the zone.

Both values are case insensitive.

When building styles are disabled for a zone, the zone will
behave as if it is compatible with all active building styles.
This is the game's built-in behavior for the industrial zones.

# Making Your Own INI Files

The TGI values and INI files section name varies depending on the industrial zone, see the sections
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
