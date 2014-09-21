--[[
		FOR DEBUGGING
]]--
function print_r (t, indent, done)
  done = done or {}
  indent = indent or ''
  local nextIndent -- Storage for next indentation value
  for key, value in pairs (t) do
    if type (value) == "table" and not done [value] then
      nextIndent = nextIndent or
          (indent .. string.rep(' ',string.len(tostring (key))+2))
          -- Shortcut conditional allocation
      done [value] = true
      print (indent .. "[" .. tostring (key) .. "] => Table {");
      print  (nextIndent .. "{");
      print_r (value, nextIndent .. string.rep(' ',2), done)
      print  (nextIndent .. "}");
    else
      print  (indent .. "[" .. tostring (key) .. "] => " .. tostring (value).."")
    end
  end
end

require "ouya"

ouya:addEventListener(Event.KEY_DOWN, function(e)
	if e.keyCode == KeyCode.BUTTON_A then
		print("KeyDown BUTTON_A")
	elseif e.keyCode == KeyCode.BUTTON_DPAD_DOWN then
		print("KeyDown BUTTON_DPAD_DOWN")
	elseif e.keyCode == KeyCode.BUTTON_DPAD_LEFT then
		print("KeyDown BUTTON_DPAD_LEFT")
	elseif e.keyCode == KeyCode.BUTTON_DPAD_RIGHT then
		print("KeyDown BUTTON_DPAD_RIGHT")
	elseif e.keyCode == KeyCode.BUTTON_DPAD_UP then
		print("KeyDown BUTTON_DPAD_UP")
	elseif e.keyCode == KeyCode.BUTTON_L1 then
		print("KeyDown BUTTON_L1")
	elseif e.keyCode == KeyCode.BUTTON_L2 then
		print("KeyDown BUTTON_L2")
	elseif e.keyCode == KeyCode.BUTTON_L3 then
		print("KeyDown BUTTON_L3")
	elseif e.keyCode == KeyCode.BUTTON_MENU then
		print("KeyDown BUTTON_MENU")
	elseif e.keyCode == KeyCode.BUTTON_O then
		print("KeyDown BUTTON_O")
	elseif e.keyCode == KeyCode.BUTTON_R1 then
		print("KeyDown BUTTON_R1")
	elseif e.keyCode == KeyCode.BUTTON_R2 then
		print("KeyDown BUTTON_R2")
	elseif e.keyCode == KeyCode.BUTTON_R3 then
		print("KeyDown BUTTON_R3")
	elseif e.keyCode == KeyCode.BUTTON_U then
		print("KeyDown BUTTON_U")
	elseif e.keyCode == KeyCode.BUTTON_Y then
		print("KeyDown BUTTON_Y")
	else
		print("Other key down: "..e.keyCode)
	end
end)

ouya:addEventListener(Event.KEY_UP, function(e)
	if e.keyCode == KeyCode.BUTTON_A then
		print("KeyUp BUTTON_A")
	elseif e.keyCode == KeyCode.BUTTON_DPAD_DOWN then
		print("KeyUp BUTTON_DPAD_DOWN")
	elseif e.keyCode == KeyCode.BUTTON_DPAD_LEFT then
		print("KeyUp BUTTON_DPAD_LEFT")
	elseif e.keyCode == KeyCode.BUTTON_DPAD_RIGHT then
		print("KeyUp BUTTON_DPAD_RIGHT")
	elseif e.keyCode == KeyCode.BUTTON_DPAD_UP then
		print("KeyUp BUTTON_DPAD_UP")
	elseif e.keyCode == KeyCode.BUTTON_L1 then
		print("KeyUp BUTTON_L1")
	elseif e.keyCode == KeyCode.BUTTON_L2 then
		print("KeyUp BUTTON_L2")
	elseif e.keyCode == KeyCode.BUTTON_L3 then
		print("KeyUp BUTTON_L3")
	elseif e.keyCode == KeyCode.BUTTON_MENU then
		print("KeyUp BUTTON_MENU")
	elseif e.keyCode == KeyCode.BUTTON_O then
		print("KeyUp BUTTON_O")
	elseif e.keyCode == KeyCode.BUTTON_R1 then
		print("KeyUp BUTTON_R1")
	elseif e.keyCode == KeyCode.BUTTON_R2 then
		print("KeyUp BUTTON_R2")
	elseif e.keyCode == KeyCode.BUTTON_R3 then
		print("KeyUp UTTON_R3")
	elseif e.keyCode == KeyCode.BUTTON_U then
		print("KeyUp BUTTON_U")
	elseif e.keyCode == KeyCode.BUTTON_Y then
		print("KeyUp BUTTON_Y")
	else
		print("Other key up: "..e.keyCode)
	end
end)

ouya:addEventListener(Event.RIGHT_JOYSTICK, function(e)
	print("RIGHT_JOYSTICK:", "x:"..e.x, "y:"..e.y, "angle:"..e.angle, "strength:"..e.strength)
end)

ouya:addEventListener(Event.LEFT_JOYSTICK, function(e)
	print("LEFT_JOYSTICK:", "x:"..e.x, "y:"..e.y, "angle:"..e.angle, "strength:"..e.strength)
end)

ouya:addEventListener(Event.RIGHT_TRIGGER, function(e)
	print("RIGHT_TRIGGER:", "strength:"..e.strength)
end)

ouya:addEventListener(Event.LEFT_TRIGGER, function(e)
	print("LEFT_TRIGGER:", "strength:"..e.strength)
end)


