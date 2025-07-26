# Building Style Exemplars

These exemplars provide a way for building style mods to define the style check boxes without requiring the user to edit _BuildingStyles.ini_.

The exemplar must use the group id `0xB06361D6` with the style id as the instance id.

The styles specified in BuildingStyles.ini take precedence over any conflicting styles specified in an exemplar.
This is done to preserve the behavior of the INI file allowing for fully custom styles.

## Examples

An DAT file that demonstrates how to creat this exemplar is located in the [examples](https://github.com/0xC0000054/sc4-more-building-styles/tree/main/examples) folder.

## Exemplar Properties

The _Building Style Exemplar: Style Name Key_ property is required, all other properties are optional.

| Name | Id | Type | Reps | Description |
|------|----|------|------|-------------|
| Building Style Exemplar: Style Name Key | 0x9CCFAD35 | Uint32 | 3 | Resource key for the building style name that is shown in the UI. |
| Building Style Exemplar: Use Bold Text | 0x9CCFAD36 | Bool | 0 | Indicates if the check box text should be bold. |
| Building Style Exemplar: Tool Tip Key | 0x9CCFAD37 | Uint32 | 3 | Resource key for the building style check box tool tip. |

### Building Style Check Box Tool Tips

The individual building styles can have a tool tip that describes the style when the user hovers over that check box.

Tool tips can optionally have a title in addition to a body, this is done by placing both on a single line with a pipe separator.    
For example, `ToolTip Title|Tool tip body`.   
Note that how the game displays tool tips with both a title and body will depend on the user's game settings.
If the user has unchecked the _expanded tooltips_ check box in the play options dialog, the game will display the tool tip
title (if present) or the body, but not both.

The Building Style Exemplar: Tool Tip Key is one of two methods that a tool tip can be associated with a specific style check box.
The other is to use a LTEXT file, which is documented [here](https://github.com/0xC0000054/sc4-more-building-styles#building-style-check-box-tool-tips).