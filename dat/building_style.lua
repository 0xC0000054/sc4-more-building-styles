--#-package:045bf110# -- package signature --
-- Any Lua files that depend on this file must have higher package numbers.

building_style_ui_buttons = {}

    building_style_ui_buttons.NO_KICK_OUT = 0
make_table_const(building_style_ui_buttons)

building_style_wall_to_wall_mode = {}

    -- Wall to wall items are built alongside other building and lot types.
    -- This is the game's standard behavior.
    building_style_wall_to_wall_mode.MIXED = 0
    -- Only wall to wall buildings and lots will be built.
    building_style_wall_to_wall_mode.ONLY = 1
    -- Block wall to wall buildings and lots from being built.
    building_style_wall_to_wall_mode.BLOCK = 2
make_table_const(building_style_wall_to_wall_mode)

building_style = {}

    -- Gets a value indicating if 'Build all styles at once' is enabled.
    building_style.is_build_all_styles_at_once = function() return false end
    -- Gets the number of years between styles changes when 'Build all styles at once' is disabled.
    -- When 'Build all styles at once' is enabled, it will return nil.
    building_style.get_years_between_style_changes = function() return nil end
    -- Gets a table containing the numeric values of the available styles.
    building_style.get_available_style_ids = function() return {} end
    -- Gets a table containing the available styles as a name/id pair.
    -- The name is the first item in the pair, and the style id is the second.
    -- The name is UTF-8 encoded.
    building_style.get_available_style_name_id_pairs = function() return {} end
    -- Gets a value indicating if the specified style id is available in the building style UI.
    building_style.is_style_available = function(styleId) return false end
    -- Gets a table containing the numeric values of the active styles.
    -- Equivalent to cISC4TractDeveloper::GetActiveStyles() in C++.
    building_style.get_active_style_ids = function() return {} end
    -- Gets a table containing the active styles as a name/id pair.
    -- The name is the first item in the pair, and the style id is the second.
    -- The name is UTF-8 encoded.
    building_style.get_active_style_name_id_pairs = function() return {} end
    -- Sets the active styles to the values specified in styleIDTable.
    -- The table must contain only numbers.
    -- Equivalent to cISC4TractDeveloper::SetActiveStyles(vector<uint32_t>) in C++.
    building_style.set_active_style_ids = function(styleIdTable) return false end
    -- Gets the style name for the specified style id.
    -- If styleID was not found, it will return nil.
    -- The name is UTF-8 encoded.
    building_style.get_style_name = function(styleId) return "" end 
    -- Gets a value indicating the active wall to wall mode in the building style UI.
    building_style.get_wall_to_wall_mode = function() return building_style_wall_to_wall_mode.MIXED end
    -- Gets a value indicating if the specified option is active in the building style UI.
    -- Uses the building_style_ui_buttons table above.
    building_style.is_ui_button_checked = function(buildingStyleUIButton) return false end
