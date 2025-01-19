# Building Style Check Box IDs

The building style check boxes ids can be configured in one of two ways.
If you are creating your own custom styles, you will need to get an assigned range from the  [SimCity 4 Building Styles Index](https://community.simtropolis.com/forums/topic/763276-building-styles-master-iid/). 

## Setting The Style ID as the Check Box ID

The check box id is set to the value of the style id. For example, the Maxis _Chicago 1890_ style would use a check box id of _0x2000_. 

## Using BuildingStyles.ini

This allows the building style mapping to be defined in _BuildingStyles.ini_, with the check box names being set from the INI file.
See the _BuildingStyles.ini_ [documentation](https://github.com/0xC0000054/sc4-more-building-styles#buildingstyles-ini-file) for details.

# Optional Check Boxes and Radio Buttons

The DLL supports a number of optional check boxes and radio buttons that modders can add to the Building Style Control UI.

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
This is the preffered method for identifying W2W buildings that do not already have one of the W2W occupant groups.

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
