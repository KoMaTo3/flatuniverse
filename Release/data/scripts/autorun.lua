--[[

Точка вхождения

]]

local playerState = {
  onGroundTime = -1,
  isHoldJump = false,
  longJumpPower = 25,
  PlayerEndLongJumpTimer = -1,
}

-- Списки анимаций, привязанных к таймерам
local animation = {
}

--[[ Main ]]
function Main()
  LoadScript( 'data/scripts/gui.lua' )
  LoadScript( 'data/scripts/editor.lua' )
  EditorInit()

  --[[
  ObjectAttr( 'wall', { trigger = true, triggerSize = 26 } )
  ]]

  -- create temp object
  --[[
  local cameraX, cameraY = GetCameraPos()
  local name = 'wall-new'
  local tileSize = GetTileSize()
  ObjectCreate( name, cameraX, cameraY, -1 )
  ObjectAttr( name, { renderable = true, textureName = 'temp/enemy0.png', renderableSize = tileSize..' '..tileSize } )
  ObjectAttr( name, { collision = true, collisionSize = tileSize..' '..tileSize, collisionStatic = isStatic } )
  ObjectAttr( name, { trigger = true, triggerSize = ( tileSize + 2 )..' '..( tileSize + 2 ) } )
  --]]

  -- ObjectAttr( 'wall', { 'textureName' } )
  -- ObjectAttr( 'wall', { 'textureName' } )
  -- local r,g,b,a = ObjectAttr( 'wall', { 'color' } )
  -- Alert( '', r..':'..g..':'..b..':'..a )
  ListenKeyboard( 'PlayerControl' )
  ListenCollision( 'CollisionPlayer', 'player' )
  ObjectAddTag( 'player', 'player' )

  -- reset bricks

end -- Main
Main()

--[[ CollisionPlayer ]]
function CollisionPlayer( player, target )
  if IsObjectUpperThis( player, target ) then
    playerState.onGroundTime = GetTime()
  elseif IsObjectUnderThis( player, target ) then
    if ObjectHasTag( target, 'brick-breakable' ) then
      ObjectAttr( target, { textureName = 'temp/brick3.png' } )
    end
    if ObjectHasTag( target, 'has-mushroom' ) then
      PushMushroom( target )
    end
  end
end -- CollisionPlayer

--[[ PlayerControl ]]
function PlayerControl( id, isPressed )
  if GUIRenderer.focusedItem ~= nil then
    return
  end
  if id == 0x26 or id == 0x57 then  -- Up
    if isPressed then --(
      local curTime = GetTime()
      if curTime - playerState.onGroundTime < 0.1 then  -- do jump
        local vx, _ = ObjectAttr( 'player', { 'collisionVelocity' } )
        ObjectAttr( 'player', { collisionVelocity = vx..' '..( -300 )  } )
        playerState.isHoldJump = true
        SetTimer( 0.1, 'PlayerDoLongJump' )
        playerState.PlayerEndLongJumpTimer = SetTimer( 0.5, 'PlayerEndLongJump' )
        playerState.onGroundTime = 0
      end
    else --) (
      playerState.isHoldJump = false
    end --)
  end

  if id == 0x25 or id == 0x41 then  -- Left
    if isPressed then
      SetObjectForce( 'player', 4, -200, 0 )
    else
      RemoveObjectForce( 'player', 4 )
    end
  end

  if id == 0x27 or id == 0x44 then  -- Right
    if isPressed then
      SetObjectForce( 'player', 2, 200, 0 )
    else
      RemoveObjectForce( 'player', 2 )
    end
  end
end -- PlayerControl

--[[ PlayerDoLongJump ]]
function PlayerDoLongJump( timerId )
  if playerState.isHoldJump then
    local vx, vy = ObjectAttr( 'player', { 'collisionVelocity' } )
    ObjectAttr( 'player', { collisionVelocity = vx..' '..( vy - playerState.longJumpPower )  } )
    SetTimer( 0.1, 'PlayerDoLongJump' )
  end
end -- PlayerDoLongJump

--[[ PlayerEndLongJump ]]
function PlayerEndLongJump( timerId )
  playerState.isHoldJump = false
  if playerState.PlayerEndLongJumpTimer ~= -1 then
    -- StopTimer( playerState.PlayerEndLongJumpTimer )
    playerState.PlayerEndLongJumpTimer = -1
  end
end -- PlayerEndLongJump

--[[ IsObjectUnderThis ]]
function IsObjectUnderThis( object, target )
  local _, ty    = ObjectGetPos( target )
  local _, y      = ObjectGetPos( object )
  local _, tRectY = ObjectAttr( target, { 'collisionSize' } )
  local _, rectY  = ObjectAttr( object, { 'collisionSize' } )
  local tRectYd2 = tRectY / 2
  local rectYd2 = rectY / 2
  --if y + rectYd2 <= ty - tRectYd2 then
  if y - rectYd2 >= ty + tRectYd2 then return true end
  return false
end -- IsObjectUnderThis

--[[ IsObjectUpperThis ]]
function IsObjectUpperThis( object, target )
  local _, ty    = ObjectGetPos( target )
  local _, y      = ObjectGetPos( object )
  local _, tRectY = ObjectAttr( target, { 'collisionSize' } )
  local _, rectY  = ObjectAttr( object, { 'collisionSize' } )
  local tRectYd2 = tRectY / 2
  local rectYd2 = rectY / 2
  if y + rectYd2 <= ty - tRectYd2 + 1 then return true end
  return false
end -- IsObjectUpperThis

--[[ IsObjectRightThis ]]
function IsObjectRightThis( object, target )
  local tx, _    = ObjectGetPos( target )
  local x, _      = ObjectGetPos( object )
  local tRectX, _ = ObjectAttr( target, { 'collisionSize' } )
  local rectX, _  = ObjectAttr( object, { 'collisionSize' } )
  local tRectXd2 = tRectX / 2
  local rectXd2 = rectX / 2
  if x + rectXd2 <= tx - tRectXd2 + 1 then return true end
  return false
end -- IsObjectRightThis

--[[ IsObjectLeftThis ]]
function IsObjectLeftThis( object, target )
  local tx, _    = ObjectGetPos( target )
  local x, _      = ObjectGetPos( object )
  local tRectX, _ = ObjectAttr( target, { 'collisionSize' } )
  local rectX, _  = ObjectAttr( object, { 'collisionSize' } )
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

--[[ PushMushroom ]]
function PushMushroom( object )
  local itemName = object..'-mushroom-'..string.format( '%f', GetTime() )
  local brickX, brickY = ObjectGetPos( object )
  ObjectCreate( itemName, brickX, brickY, -0.1 )
  ObjectAttr( itemName, { renderable = true, textureName = 'textures/items/mushroom.png', renderableSize = '32 32' } )
  animation[ 'timer'..SetTimer( 0.1, 'DoAnimationMushroom' ) ] = { step = 1, time = 0, object = itemName, tile = object }
  ObjectRemoveTag( object, 'has-mushroom' )
end -- PushMushroom

--[[ DoAnimationMushroom ]]
function DoAnimationMushroom( timerId )
  local keyByTimer = 'timer'..timerId
  if animation[ keyByTimer ] == nil then
    do return false end
  end

  local anim = animation[ keyByTimer ]

  if anim.step == 1 then
    anim.time = anim.time + 1
    local x, y = ObjectGetPos( anim.object )
    ObjectSetPos( anim.object, x, y - 1 )
    if anim.time < 32 then
      animation[ 'timer'..SetTimer( 1/40, 'DoAnimationMushroom' ) ] = anim
    else
      ObjectAddTag( anim.tile, 'has-mushroom' )
      animation[ keyByTimer ] = nil
      ObjectAttr( anim.object, { collision = true, collisionSize = '32 32', collisionVelocity = '150 0', collisionAcceleration = '0 400', collisionStatic = false } )
      ObjectAddTag( anim.object, 'mushroom' )
      ListenCollision( 'CollisionMushroom', anim.object )
    end
  end
end -- DoAnimationMushroom

function CollisionMushroom( mushroom, target )
  if ObjectHasTag( target, 'player' ) then
    ObjectRemove( mushroom )
    local cx, cy, rx, ry = ObjectAttr( target, { 'collisionSize', 'renderableSize' } )
    ObjectAttr( target, { collisionSize = string.format( '%f %f', cx * 1.2, cy * 1.2 ), renderableSize = string.format( '%f %f', rx * 1.2, ry * 1.2 ) } )
    do return false end
  end
  if IsObjectRightThis( mushroom, target ) or IsObjectLeftThis( mushroom, target ) then
    local vx, vy = ObjectAttr( mushroom, { 'collisionVelocity' } )
    ObjectAttr( mushroom, { collisionVelocity = string.format( '%f %f', -vx, vy ) } )
  end
end
