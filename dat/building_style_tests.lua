--#-package:045bf120# -- package signature --
-- Any Lua files that depend on this file must have higher package numbers.

-- The following code is used to test the building_style functions.
-- It also serves as examples of their use.

function null45_array_tostring(t)
  return table.concat(t, ", ")
end

function null45_pairs_tostring(t)
  local temp = {}
  
  for k,v in pairs(t) do
    table.insert(temp, tostring(k) .. " - " .. tostring(v))
  end
  
  return null45_array_tostring(temp)
end

function null45_building_style_test_is_build_all_styles_at_once()
  return tostring(building_style.is_build_all_styles_at_once())
end

function null45_building_style_test_get_years_between_style_changes()
  return tostring(building_style.get_years_between_style_changes())
end
	
function null45_building_style_test_get_available_style_ids()
  return null45_array_tostring(building_style.get_available_style_ids())
end

function null45_building_style_test_get_available_style_name_id_pairs()
  return null45_pairs_tostring(building_style.get_available_style_name_id_pairs())
end

function null45_building_style_test_is_style_available()
  local chicago_1890_style_id = hex2dec('2000')
  
  return tostring(building_style.is_style_available(chicago_1890_style_id))
end

function null45_building_style_test_get_active_style_ids()
  return null45_array_tostring(building_style.get_active_style_ids())
end

function null45_building_style_test_get_active_style_name_id_pairs()
  return null45_pairs_tostring(building_style.get_active_style_name_id_pairs())
end

function null45_building_style_test_set_active_style_ids_1()
  -- Test reading from the active styles.
  local activeStyles = building_style.get_active_style_ids()
  
  return tostring(building_style.set_active_style_ids(activeStyles))
end

function null45_building_style_test_set_active_style_ids_2()
  -- Test reading from the active style pairs.
  local activeStyles = building_style.get_active_style_name_id_pairs()
  
  return tostring(building_style.set_active_style_ids(activeStyles))
end

function null45_building_style_test_set_active_style_ids_3()
  -- Test reading from a user-created table.
  local activeStyles = {hex2dec('2000'), hex2dec('2001'), hex2dec('2002')}
  
  return tostring(building_style.set_active_style_ids(activeStyles))
end

function null45_building_style_test_get_style_name()
  local chicago_1890_style_id = hex2dec('2000')
  
  return building_style.get_style_name(chicago_1890_style_id)
end

function null45_building_style_test_get_wall_to_wall_mode()
  local mode = building_style.get_wall_to_wall_mode()

  if mode == building_style_wall_to_wall_mode.ONLY then
    return "Only"
  elseif mode == building_style_wall_to_wall_mode.BLOCK then
    return "Block"
  else
    return "Mixed"
  end
end

function null45_building_style_test_is_ui_button_checked()
  local checked = building_style.is_ui_button_checked(building_style_ui_buttons.NO_KICK_OUT)
 
  return "No Kick Out checked = " .. tostring(checked)
end
