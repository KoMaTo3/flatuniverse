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

end -- Main
Main()

--[[ CollisionPlayer ]]
function CollisionPlayer( player, target )
  local tx, ty    = ObjectGetPos( target )
  local x, y      = ObjectGetPos( player )
  local _, tRectY = ObjectAttr( target, { 'collisionSize' } )
  local _, rectY  = ObjectAttr( player, { 'collisionSize' } )
  LogWrite( (y + rectY/2)..' : '..( ty - tRectY/2 ) )
  if y + rectY/2 <= ty - tRectY/2 then
    playerState.onGroundTime = GetTime()
  end
end -- CollisionPlayer

--[[ PlayerControl ]]
function PlayerControl( id, isPressed )
  if id == 0x26 then  -- Up
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

  if id == 0x25 then  -- Left
    if isPressed then
      SetObjectForce( 'player', 4, -150, 0 )
    else
      RemoveObjectForce( 'player', 4 )
    end
  end

  if id == 0x27 then  -- Right
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
    StopTimer( playerState.PlayerEndLongJumpTimer )
    playerState.PlayerEndLongJumpTimer = -1
  end
end -- PlayerEndLongJump
