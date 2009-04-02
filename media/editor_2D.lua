local bMoving = false
local lastX = 0
local lastY = 0

function OnMousePressed(x, y)
	if (jzSetActiveSprite(x, y)) then
		bMoving = true
		lastX = x
		lastY = y
		return true
	else
		bMoving = false
		return false
	end
end

function OnMouseReleased()
    bMoving = false
	return false
end

function OnMouseMove(x, y)
	if (bMoving) then
		jzMoveSpriteScreenSpace(x - lastX, y - lastY)
		lastX = x
		lastY = y
	end		
	return false
end
