--[[

Точка вхождения

]]

local playerState = {
  onGroundTime = -1,
  isHoldJump = false,
  jumpPower = -350,
  longJumpPower = 250,
  longJumpStep = 0,
  longJumpTimer = -1,
  PlayerEndLongJumpTimer = -1,
  lastDirection = 1,  -- direction of face
  currentAction = 0,  -- current action: 0-stay, 1-walk, 2-jump
  allowDoubleJump = true,
}

-- Списки анимаций, привязанных к таймерам
local animation = {
}

function table.copy( t )
  local t2 = {}
  for k,v in pairs( t ) do
    t2[ k ] = v
  end
  return t2
end

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
  ListenCollision( 'player', 'CollisionPlayer' )
  ObjectAddTag( 'player', 'player' )
  ObjectSetAnimation( 'player', 'player/mario', 'stay-'..( playerState.lastDirection > 0 and 'right' or 'left' ) )
  SetTimer( 1 / 10, 'UpdatePlayerAnimation', true )
  ObjectAttr( 'player', { collisionAcceleration = '0 1500' } )
  -- ListenTrigger( 'testFunc', 'wall.424.286.4.030000' )
  -- ObjectRemove( 'camera-mario-style' )
  -- ObjectCreate( 'camera-mario-style', 0, 0, 0 )
  -- SetCamera( 'camera-mario-style' )
  -- SetTimer( 1/60, 'UpdateCamera' )

  -- reset bricks

end -- Main
Main()

function TestFunc( object, trigger, isInTrigger )
  if isInTrigger == 0 then  -- object in trigger
  end
end

function DoKill( object, trigger, isInTrigger )
  if isInTrigger == 0 then  -- object in trigger
    ObjectSetPos( 'player', 0, 0 )
    ObjectAttr( 'player', { collisionVelocity = '0 0' } )
  end
end

--[[ CoinCollect ]]
function CoinCollect( trigger, object, isInTrigger )
  if object == 'player' then
    if isInTrigger == 0 then
      ObjectRemove( trigger )
    end
  end
end -- CoinCollect

--[[ CollisionPlayer ]]
function CollisionPlayer( player, target, flags, vx, vy )
  if ObjectHasTag( target, 'no-reset-player-velocity' ) then
    ObjectAttr( player, { collisionVelocity = vx..' '..vy } )
  end
  if ObjectHasTag( target, 'mushroom' ) then
    ObjectRemove( target )
    return false
    -- local cx, cy, rx, ry = ObjectAttr( player, { 'collisionSize', 'renderableSize' } )
    -- ObjectAttr( player, { collisionSize = string.format( '%f %f', cx * 1.2, cy * 1.2 ), renderableSize = string.format( '%f %f', rx * 1.2, ry * 1.2 ) } )
    -- do return false end
  end
  if IsObjectUpperThis( player, target ) then
    playerState.allowDoubleJump = true
    playerState.onGroundTime = GetTime()
    if playerState.currentAction == 2 then
      playerState.currentAction = 1
      ObjectSetAnimation( 'player', 'player/mario', ( math.abs( playerState.lastDirection ) > 1 and 'walk' or 'stay' )..'-'..( playerState.lastDirection > 0 and 'right' or 'left' ) )
    end
  elseif IsObjectUnderThis( player, target ) then
    if ObjectHasTag( target, 'brick-breakable' ) then
      animation[ 'timer'..SetTimer( 0.5, 'DoAnimationBrickDisplace' ) ] = { step = 1, time = 0, timeMax = 8, object = target }
      ObjectAddTag( target, 'push-bottom' )
      ObjectRemoveTag( target, 'brick-breakable' )
      ObjectStopAnimation( target )
      ObjectSetAnimation( target, 'supermario/brick0', 'do' )
      ObjectAttr( target, { color = '0 0 0 0' } )
    end
    if ObjectHasTag( target, 'has-mushroom' ) then
      PushMushroom( target )
    end
    if ObjectHasTag( target, 'has-coin' ) then
      PushCoin( target )
    end
  end
end -- CollisionPlayer

--[[ PlayerControl ]]
function PlayerControl( id, isPressed )
  if GUIRenderer.focusedItem ~= nil then
    return
  end
  if settings.gamePaused then
  else
    if id == 0x26 or id == 0x57 then  -- Up
      if isPressed then --(
        local curTime = GetTime()
        if curTime - playerState.onGroundTime < 0.1 or playerState.allowDoubleJump then  -- do jump
          if curTime - playerState.onGroundTime >= 0.1 then -- is a double jump
            playerState.allowDoubleJump = false
          end
          local vx, _ = ObjectAttr( 'player', { 'collisionVelocity' } )
          ObjectAttr( 'player', { collisionVelocity = vx..' '..( playerState.jumpPower )  } )
          playerState.isHoldJump = true
          playerState.longJumpStep = 0
          if playerState.longJumpTimer ~= -1 then
            StopTimer( playerState.longJumpTimer )
            playerState.longJumpTimer = -1
          end
          if playerState.PlayerEndLongJumpTimer ~= -1 then
            StopTimer( playerState.PlayerEndLongJumpTimer )
            playerState.PlayerEndLongJumpTimer = -1
          end
          playerState.longJumpTimer = SetTimer( 0.1, 'PlayerDoLongJump' )
          playerState.PlayerEndLongJumpTimer = SetTimer( 0.5, 'PlayerEndLongJump' )
          playerState.onGroundTime = 0
          ObjectSetAnimation( 'player', 'player/mario', 'jump-'..( playerState.lastDirection > 0 and 'right' or 'left' ) )
          playerState.currentAction = 2
        end
      else --) (
        playerState.isHoldJump = false
      end --)
    end

    if id == 0x25 or id == 0x41 then  -- Left
      local curTime = GetTime()
      if isPressed then
        RemoveObjectForce( 'player', 2 )
        SetObjectForce( 'player', 4, -300, 0 )
        if playerState.currentAction ~= 2 then
          playerState.currentAction = 1
        end
        if curTime - playerState.onGroundTime < 0.1 then
          ObjectSetAnimation( 'player', 'player/mario', 'walk-left' )
        end
        playerState.lastDirection = -2
      else
        if playerState.lastDirection < 0 then
          RemoveObjectForce( 'player', 4 )
          if playerState.currentAction ~= 2 then
            playerState.currentAction = 0
          end
          if curTime - playerState.onGroundTime < 0.1 then
            ObjectSetAnimation( 'player', 'player/mario', 'stay-left' )
          end
          playerState.lastDirection = -1
        end
      end
    end

    if id == 0x27 or id == 0x44 then  -- Right
      local curTime = GetTime()
      if isPressed then
        RemoveObjectForce( 'player', 4 )
        SetObjectForce( 'player', 2, 300, 0 )
        playerState.lastDirection = 1
        if playerState.currentAction ~= 2 then
          playerState.currentAction = 1
        end
        if curTime - playerState.onGroundTime < 0.1 then
          ObjectSetAnimation( 'player', 'player/mario', 'walk-right' )
        end
        playerState.lastDirection = 2
      else
        if playerState.lastDirection > 0 then
          RemoveObjectForce( 'player', 2 )
          if playerState.currentAction ~= 2 then
            playerState.currentAction = 0
          end
          if curTime - playerState.onGroundTime < 0.1 then
            ObjectSetAnimation( 'player', 'player/mario', 'stay-right' )
          end
          playerState.lastDirection = 1
        end
      end
    end

    if id == 0x10 and isPressed then  -- test
      local object = 'wall.174.58.1.050000'
      ObjectSetAnimation( object, 'lift/small', 'down' )
    end
  end -- !settings.gamePaused

  if id == 0x8 then  -- BackSpace
    if isPressed then
      ObjectSetPos( 'player', 0, 0 )
      ObjectAttr( 'player', { collisionVelocity = '0 0' } )
    end
  end
end -- PlayerControl

--[[ PlayerDoLongJump ]]
function PlayerDoLongJump( timerId )
  if playerState.isHoldJump then
    local vx, vy = ObjectAttr( 'player', { 'collisionVelocity' } )
    playerState.longJumpStep = playerState.longJumpStep + 1
    ObjectAttr( 'player', { collisionVelocity = vx..' '..( vy - playerState.longJumpPower / playerState.longJumpStep )  } )
    SetTimer( 0.1, 'PlayerDoLongJump' )
  end
end -- PlayerDoLongJump

--[[ PlayerEndLongJump ]]
function PlayerEndLongJump( timerId )
  playerState.isHoldJump = false
  if playerState.PlayerEndLongJumpTimer ~= -1 then
    -- StopTimer( playerState.PlayerEndLongJumpTimer )
    playerState.PlayerEndLongJumpTimer = -1
    playerState.longJumpTimer = -1
  end
end -- PlayerEndLongJump

--[[ UpdatePlayerAnimation ]]
function UpdatePlayerAnimation()
  local curTime = GetTime()
  local timer = 0.2
  if curTime - playerState.onGroundTime < 0.1 then  -- on ground
    if playerState.currentAction == 2 then
      playerState.currentAction = 1
      ObjectSetAnimation( 'player', 'player/mario', ( math.abs( playerState.lastDirection ) > 1 and 'walk' or 'stay' )..'-'..( playerState.lastDirection > 0 and 'right' or 'left' ) )
    end
  else  -- on air
    timer = 0.5
    if playerState.currentAction ~= 2 then
      playerState.currentAction = 2
      ObjectSetAnimation( 'player', 'player/mario', 'jump-'..( playerState.lastDirection > 0 and 'right' or 'left' ) )
    end
  end
  SetTimer( timer, 'UpdatePlayerAnimation', true )
end -- UpdatePlayerAnimation

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
  ObjectAttr( object, { renderable = false } )
  ObjectSetAnimation( object, 'supermario/brick2', 'do' )
  animation[ 'timer'..SetTimer( 0.5, 'DoAnimationBrick' ) ] = { step = 1, time = 0, object = itemName, tile = object }
end -- PushMushroom

--[[ PushCoin]]
function PushCoin( object )
  local itemName = object..'-coin-'..string.format( '%f', GetTime() )
  local brickX, brickY = ObjectGetPos( object )
  ObjectCreate( itemName, brickX, brickY, -1.0, true )
  -- ObjectAttr( itemName, { renderable = true, textureName = 'textures/items/coin0.png', renderableSize = '32 32' } )
  ObjectSetAnimation( itemName, 'supermario/coin', 'do' )
  animation[ 'timer'..SetTimer( 1.0, 'DoAnimationCoin' ) ] = { step = 1, time = 0, object = itemName, tile = object }
  ObjectRemoveTag( object, 'has-coin' )
  ObjectAttr( object, { renderable = false } )
  ObjectSetAnimation( object, 'supermario/brick2', 'do' )
  animation[ 'timer'..SetTimer( 0.5, 'DoAnimationBrick' ) ] = { step = 1, time = 0, object = itemName, tile = object }
  -- ObjectAttr( object, { textureName = 'textures/tiles/bricks/brick2.png' } )
  -- ObjectAttr( itemName, { renderable = true, textureName = 'textures/tiles/coin0.png', renderableSize = '32 32' } )
  -- animation[ 'timer'..SetTimer( 0.1, 'DoAnimationCoin' ) ] = { step = 1, time = 0, object = itemName, tile = object }
  -- ObjectRemoveTag( object, 'has-coin' )
end -- PushCoin

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
      -- ObjectAddTag( anim.tile, 'has-mushroom' )
      animation[ keyByTimer ] = nil
      ObjectAttr( anim.object, { collision = true, collisionSize = '32 32', collisionVelocity = '150 0', collisionAcceleration = '0 1200', collisionStatic = false } )
      ObjectAddTag( anim.object, 'mushroom' )
      ObjectAddTag( anim.object, 'no-reset-player-velocity' )
      ListenCollision( anim.object, 'CollisionMushroom' )
    end
  end
end -- DoAnimationMushroom

--[[ DoAnimationCoin ]]
function DoAnimationCoin( timerId )
  local keyByTimer = 'timer'..timerId
  if animation[ keyByTimer ] == nil then
    do return false end
  end

  local anim = animation[ keyByTimer ]

  if anim.step == 1 then
    ObjectRemove( anim.object )
    animation[ keyByTimer ] = nil
  end
end -- DoAnimationCoin

--[[ DoAnimationBrick ]]
function DoAnimationBrick( timerId )
  local keyByTimer = 'timer'..timerId
  if animation[ keyByTimer ] == nil then
    do return false end
  end

  local anim = animation[ keyByTimer ]

  if anim.step == 1 then
    ObjectSetAnimation( anim.tile, 'supermario/brick2', 'default' )
    ObjectAttr( anim.tile, { renderable = true, textureName = 'textures/tiles/bricks/brick2.png', renderableSize = '32 32' } )
    -- ObjectRemove( anim.object )
    animation[ keyByTimer ] = nil
  end
end -- DoAnimationBrick

--[[ CollisionMushroom ]]
function CollisionMushroom( mushroom, target, flags, vx, vy )
  if bit32.band( flags, 2 ) == 2 or bit32.band( flags, 8 ) == 8 then
    ObjectAttr( mushroom, { collisionVelocity = string.format( '%f %f', -vx, vy ) } )
  elseif bit32.band( flags, 4 ) == 4 then -- stay on target
    if ObjectHasTag( target, 'push-bottom' ) then
      ObjectAttr( mushroom, { collisionVelocity = string.format( '%f %f', vx, -300 ) } )
    end
  end
  --[[
  if IsObjectRightThis( mushroom, target ) or IsObjectLeftThis( mushroom, target ) then
    local vx, vy = ObjectAttr( mushroom, { 'collisionVelocity' } )
    ObjectAttr( mushroom, { collisionVelocity = string.format( '%f %f', -vx, vy ) } )
  end
  ]]
end -- CollisionMushroom

--[[ UpdateCamera ]]
function UpdateCamera()
  local camera = GetCamera( 'camera-mario-style' )
  local _, cy = ObjectGetPos( camera )
  local px, _ = ObjectGetPos( 'player' )
  ObjectSetPos( camera, math.max( px, 0 ), cy )
  SetTimer( 0, 'UpdateCamera', true ) -- стараться избегать 0-таймеров, т.к. они зависят от быстродействия приложения и исполняются каждый тик
end -- UpdateCamera

--[[ DoAnimationBrickDisplace ]]
function DoAnimationBrickDisplace( timerId )
  local keyByTimer = 'timer'..timerId
  if animation[ keyByTimer ] == nil then
    do return false end
  end

  local anim = animation[ keyByTimer ]

  if anim.step == 1 then
    ObjectStopAnimation( anim.object, 'supermario/brick2', 'default' )
    ObjectRemoveTag( anim.object, 'push-bottom' )
    ObjectAddTag( anim.object, 'brick-breakable' )
    ObjectAttr( anim.object, { color = '1 1 1 1' } )
    animation[ keyByTimer ] = nil
    --[[
    anim.time = anim.time + 1
    if anim.time < anim.timeMax then
      ObjectAttr( anim.object, { position = string.format( '%g %g', anim.pos.x, anim.pos.y - math.sin( anim.time / anim.timeMax * 3.14 ) * 10 ) } )
      animation[ 'timer'..SetTimer( 1/60, 'DoAnimationBrickDisplace' ) ] = anim
    else
      ObjectAttr( anim.object, { position = string.format( '%g %g', anim.pos.x, anim.pos.y ) } )
      ObjectRemoveTag( anim.object, 'push-bottom' )
      ObjectAddTag( anim.object, 'brick-breakable' )
    end
    ]]
  end
end -- DoAnimationBrickDisplace
