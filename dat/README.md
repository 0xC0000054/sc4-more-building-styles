# DAT and Lua files

This folder contains the DAT and Lua files used in the plugin.
The Lua files are extracted from the DAT so that they can be shown with GitHub's Lua syntax highlighting.

## building_style.lua

This file contains the function stubs that the DLL will replace with a native C++ function. The functions
are in a Lua table whose name is known to the DLL.

## building_style_tests.lua

This file contains functions that demonstrate the use of the functions in the building_styles table.
Each function returns the result of the appropriate building_styles function as a string.

In development builds, the DLL will call these functions to verify that the building_styles table
functions are working correctly.