--[[

Точка вхождения

]]

--(
function UpdateBackGround( timerId )
  local player = Object.Get( 'editor' ):FindPlayer()
  if player ~= nil then
    local x, y = player.api:GetPos()
    Object.Get( 'background' ).api:SetPos( x, y * 0.5 + 50 )
    Core.SetTimer( 0, UpdateBackGround, true )
  end
end --)

--[[ Main ]]
function Main()
  Core.LoadScript( 'data/scripts/gui.lua' )
  Core.LoadScript( 'data/scripts/editor.lua' )
  EditorInit()

  Core.SetTimer( 0, UpdateBackGround, true )
  Scene.SetLightAmbient( settings.ambientLight.R, settings.ambientLight.G, settings.ambientLight.B, settings.ambientLight.A )
  SetEditorMode( 0 )
  -- ObjectAttr( 'background', { z = 1 } )
end -- Main
Main()

--[[ IsObjectUnderThis ]]
function IsObjectUnderThis( object, target )
  local targetObject = Object.Get( target )
  local _, ty, y = 0, 0, 0
  if targetObject ~= nil then
    _, ty = targetObject.api:GetPos()
  end
  local objectObject = Object.Get( object )
  if objectObject ~= nil then
    _, y = objectObject.api:GetPos()
  end
  -- local _, tRectY = ObjectAttr( target, { 'collisionSize' } )
  -- local _, rectY  = ObjectAttr( object, { 'collisionSize' } )
  local _, tRectY = Object.Get( target ).api:Attr({ 'collisionSize' })
  local _, rectY  = Object.Get( object ).api:Attr({ 'collisionSize' })
  local tRectYd2 = tRectY / 2
  local rectYd2 = rectY / 2
  --if y + rectYd2 <= ty - tRectYd2 then
  if y - rectYd2 >= ty + tRectYd2 then return true end
  return false
end -- IsObjectUnderThis

--[[ IsObjectUpperThis ]]
function IsObjectUpperThis( object, target )
  local targetObject = Object.Get( target )
  local ty, y = 0, 0
  if targetObject ~= nil then
    _, ty = targetObject.api:GetPos()
  end
  local objectObject = Object.Get( object )
  if objectObject ~= nil then
    _, y = objectObject.api:GetPos()
  end
  local _, tRectY = Object.Get( target ).api:Attr({ 'collisionSize' })
  local _, rectY  = Object.Get( object ).api:Attr({ 'collisionSize' })
  local tRectYd2 = tRectY / 2
  local rectYd2 = rectY / 2
  if y + rectYd2 <= ty - tRectYd2 + 1 then return true end
  return false
end -- IsObjectUpperThis

--[[ IsObjectRightThis ]]
function IsObjectRightThis( object, target )
  local targetObject = Object.Get( target )
  local _, tx, x = 0, 0, 0
  if targetObject ~= nil then
    tx, _ = targetObject.api:GetPos()
  end
  local objectObject = Object.Get( object )
  if objectObject ~= nil then
    x, _ = objectObject.api:GetPos()
  end
  local tRectX, _ = Object.Get( target ).api:Attr({ 'collisionSize' })
  local rectX, _  = Object.Get( object ).api:Attr({ 'collisionSize' })
  local tRectXd2 = tRectX / 2
  local rectXd2 = rectX / 2
  if x + rectXd2 <= tx - tRectXd2 + 1 then return true end
  return false
end -- IsObjectRightThis

--[[ IsObjectLeftThis ]]
function IsObjectLeftThis( object, target )
  local targetObject = Object.Get( target )
  local _, tx, x = 0, 0, 0
  if targetObject ~= nil then
    tx, _ = targetObject.api:GetPos()
  end
  local objectObject = Object.Get( object )
  if objectObject ~= nil then
    x, _ = objectObject.api:GetPos()
  end
  local tRectX, _ = Object.Get( target ).api:Attr({ 'collisionSize' })
  local rectX, _  = Object.Get( object ).api:Attr({ 'collisionSize' })
  local tRectXd2 = tRectX / 2
  local rectXd2 = rectX / 2
  if x - rectXd2 >= tx + tRectXd2 then return true end
  return false
end -- IsObjectLeftThis

function pairsByKeys( t, f )
  local a = {}
  for n in pairs( t ) do table.insert( a, n ) end
  table.sort( a, f )
  local i = 0      -- iterator variable
  local iter = function ()   -- iterator function
    i = i + 1
    if a[i] == nil then return nil
    else return a[i], t[a[i]]
    end
  end
  return iter
end -- pairsByKeys
