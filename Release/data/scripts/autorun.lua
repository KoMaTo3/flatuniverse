--[[

Точка вхождения

]]

local playerState = {
  onGroundTime = -1,
  isHoldJump = false,
  longJumpPower = 25,
  PlayerEndLongJumpTimer = -1,
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
  end
end -- CollisionPlayer

--[[ PlayerControl ]]
function PlayerControl( id, isPressed )
  if id == 0x26 or id == 0x57 then  -- Up
    if isPressed then --(
      local curTime = GetTime()
      if curTime - playerState.onGroundTime < 0.1 then  -- do jump
        local vx, _ = ObjectAttr( 'player', { 'collisionVelocity' } )
        ObjectAttr( 'player', { collisionVelocity = vx..' '..( -250 )  } )
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
      SetObjectForce( 'player', 4, -150, 0 )
    else
      RemoveObjectForce( 'player', 4 )
    end
  end

  if id == 0x27 or id == 0x44 then  -- Right
    if isPressed then
      SetObjectForce( 'player', 2, 150, 0 )
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
  if y + rectYd2 <= ty - tRectYd2 then return true end
  return false
end -- IsObjectUpperThis

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
end
