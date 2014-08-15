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

--[[ UpdatePlayerAnimation ]]
function UpdatePlayerAnimation( timerid )
  local curTime = Core.GetTime()
  local timer = 0.2
  if curTime - playerState.onGroundTime < 0.1 then  -- on ground
    if playerState.currentAction == 2 then
      playerState.currentAction = 1
      Object.Get( 'player' ).api:SetAnimation( 'player/mario', ( math.abs( playerState.lastDirection ) > 1 and 'walk' or 'stay' )..'-'..( playerState.lastDirection > 0 and 'right' or 'left' ) )
    end
  else  -- on air
    timer = 0.5
    if playerState.currentAction ~= 2 then
      playerState.currentAction = 2
      Object.Get( 'player' ).api:SetAnimation( 'player/mario', 'jump-'..( playerState.lastDirection > 0 and 'right' or 'left' ) )
    end
  end
  Core.SetTimer( timer, UpdatePlayerAnimation, true )
end -- UpdatePlayerAnimation

--(
function UpdateBackGround( timerId )
  local x, y = Object.Get( 'player' ).api:GetPos()
  Object.Get( 'background' ).api:SetPos( x, y * 0.5 + 50 )
  Core.SetTimer( 0, UpdateBackGround, true )
end --)

--[[ Main ]]
function Main()
  Core.LoadScript( 'data/scripts/gui.lua' )
  Core.LoadScript( 'data/scripts/editor.lua' )
  EditorInit()

  --[[
  ObjectAttr( 'wall', { trigger = true, triggerSize = 26 } )
  ]]

  -- create temp object
  --[[
  local cameraX, cameraY = Camera.GetPos()
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
  -- Debug.Alert( '', r..':'..g..':'..b..':'..a )
  -- Keyboard.Listen( 'PlayerControl' )
  -- ListenCollision( 'player', 'CollisionPlayer' )
  Object.Get( 'player' ).api:AddTag( 'player' )
  Object.Get( 'player' ).api:SetAnimation( 'player/mario', 'stay-'..( playerState.lastDirection > 0 and 'right' or 'left' ) )
  Core.SetTimer( 1 / 10, UpdatePlayerAnimation, true )
  Core.SetTimer( 0, UpdateBackGround, true )
  Object.Get( 'player' ).api:Attr({ collisionAcceleration = '0 1500', lightPoint = true, lightPointSize = 400, lightPointColor = '1 1 1 1', lightPointPenetration = 3 })
  -- ObjectAttr( 'player', { collisionAcceleration = '0 1500', lightPoint = true, lightPointSize = 400, lightPointColor = '1 1 1 1', lightPointPenetration = 3 } )
  Scene.SetLightAmbient( settings.ambientLight.R, settings.ambientLight.G, settings.ambientLight.B, settings.ambientLight.A )
  -- ObjectAttr( 'background', { z = 1 } )
  -- ListenTrigger( 'testFunc', 'wall.424.286.4.030000' )
  -- ObjectCreate( 'camera-mario-style', 0, 0, 0 )
  -- Camera.Set( 'camera-mario-style' )
  -- Core.SetTimer( 1/60, 'UpdateCamera' )
  -- local dump = serialize( Main )
  -- Debug.Alert( '', dump )
  -- local test = load( dump )()
  -- Debug.Alert('',test('player','player',false))
  -- Core.SetTimer( 2, function() Debug.Log( 'gettime = '..Core.GetTime() ) end )
end -- Main
Main()

function TestFunc( object, trigger, isInTrigger )
  if isInTrigger == 0 then  -- object in trigger
  end
end

function DoKill( object, trigger, isInTrigger )
  if isInTrigger == 0 then  -- object in trigger
    Object.Get( 'player' ).api:SetPos( 0, 0 )
    Object.Get( 'player' ).api:Attr({ collisionVelocity = '0 0' })
  end
end

--[[ CoinCollect ]]
function CoinCollect( trigger, object, isInTrigger )
  if object == 'player' then
    if isInTrigger == 0 then
      Object.Get( trigger ).api:Destroy()
    end
  end
end -- CoinCollect

--[[ CollisionPlayer ]]
--[[
function CollisionPlayer( player, target, flags, vx, vy )
  if ObjectHasTag( target, 'no-reset-player-velocity' ) then
    ObjectAttr( player, { collisionVelocity = vx..' '..vy } )
  end
  if ObjectHasTag( target, 'mushroom' ) then
    Object.Get( target ).api:Destroy()
    return false
    -- local cx, cy, rx, ry = ObjectAttr( player, { 'collisionSize', 'renderableSize' } )
    -- ObjectAttr( player, { collisionSize = string.format( '%f %f', cx * 1.2, cy * 1.2 ), renderableSize = string.format( '%f %f', rx * 1.2, ry * 1.2 ) } )
    -- do return false end
  end
  if IsObjectUpperThis( player, target ) then
    playerState.allowDoubleJump = true
    playerState.onGroundTime = Core.GetTime()
    if playerState.currentAction == 2 then
      playerState.currentAction = 1
      Object.Get( 'player' ).api:SetAnimation( 'player/mario', ( math.abs( playerState.lastDirection ) > 1 and 'walk' or 'stay' )..'-'..( playerState.lastDirection > 0 and 'right' or 'left' ) )
    end
  elseif IsObjectUnderThis( player, target ) then
    if ObjectHasTag( target, 'brick-breakable' ) then
      animation[ 'timer'..Core.SetTimer( 0.5, function( timerId ) --(
          local keyByTimer = 'timer'..timerId
          if animation[ keyByTimer ] == nil then
            do return false end
          end

          local anim = animation[ keyByTimer ]

          if anim.step == 1 then
            ObjectStopAnimation( anim.object )
            ObjectRemoveTag( anim.object, 'push-bottom' )
            ObjectAddTag( anim.object, 'brick-breakable' )
            ObjectAttr( anim.object, { color = '1 1 1 1', renderableRotation = 0 } )
            animation[ keyByTimer ] = nil
          end
        end --)
      ) ] = { step = 1, time = 0, timeMax = 8, object = target }
      ObjectAddTag( target, 'push-bottom' )
      ObjectRemoveTag( target, 'brick-breakable' )
      ObjectStopAnimation( target )
      Object.Get( target ).api:SetAnimation( 'supermario/brick0', 'do', 'stop' )
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
]]

--[[ PlayerControl ]]
--[[
function PlayerControl( id, isPressed )
  if GUIRenderer.focusedItem ~= nil then
    return
  end
  if settings.gamePaused then
  else
    if id == 0x26 or id == 0x57 then  -- Up
      if isPressed then --(
        local curTime = Core.GetTime()
        if curTime - playerState.onGroundTime < 0.1 or playerState.allowDoubleJump then  -- do jump
          if curTime - playerState.onGroundTime >= 0.1 then -- is a double jump
            playerState.allowDoubleJump = false
          end
          local vx, _ = ObjectAttr( 'player', { 'collisionVelocity' } )
          ObjectAttr( 'player', { collisionVelocity = vx..' '..( playerState.jumpPower )  } )
          playerState.isHoldJump = true
          playerState.longJumpStep = 0
          if playerState.longJumpTimer ~= -1 then
            Core.StopTimer( playerState.longJumpTimer )
            playerState.longJumpTimer = -1
          end
          if playerState.PlayerEndLongJumpTimer ~= -1 then
            Core.StopTimer( playerState.PlayerEndLongJumpTimer )
            playerState.PlayerEndLongJumpTimer = -1
          end
          playerState.longJumpTimer = Core.SetTimer( 0.1, PlayerDoLongJump )
          playerState.PlayerEndLongJumpTimer = Core.SetTimer( 0.5, PlayerEndLongJump )
          playerState.onGroundTime = 0
          Object.Get( 'player' ).api:SetAnimation( 'player/mario', 'jump-'..( playerState.lastDirection > 0 and 'right' or 'left' ) )
          playerState.currentAction = 2
        end
      else --) (
        playerState.isHoldJump = false
      end --)
    end

    if id == 0x25 or id == 0x41 then  -- Left
      local curTime = Core.GetTime()
      if isPressed then
        RemoveObjectForce( 'player', 2 )
        SetObjectForce( 'player', 4, -300, 0 )
        if playerState.currentAction ~= 2 then
          playerState.currentAction = 1
        end
        if curTime - playerState.onGroundTime < 0.1 then
          Object.Get( 'player' ).api:SetAnimation( 'player/mario', 'walk-left' )
        end
        playerState.lastDirection = -2
      else
        if playerState.lastDirection < 0 then
          RemoveObjectForce( 'player', 4 )
          if playerState.currentAction ~= 2 then
            playerState.currentAction = 0
          end
          if curTime - playerState.onGroundTime < 0.1 then
            Object.Get( 'player' ).api:SetAnimation( 'player/mario', 'stay-left' )
          end
          playerState.lastDirection = -1
        end
      end
    end

    if id == 0x27 or id == 0x44 then  -- Right
      local curTime = Core.GetTime()
      if isPressed then
        RemoveObjectForce( 'player', 4 )
        SetObjectForce( 'player', 2, 300, 0 )
        playerState.lastDirection = 1
        if playerState.currentAction ~= 2 then
          playerState.currentAction = 1
        end
        if curTime - playerState.onGroundTime < 0.1 then
          Object.Get( 'player' ).api:SetAnimation( 'player/mario', 'walk-right' )
        end
        playerState.lastDirection = 2
      else
        if playerState.lastDirection > 0 then
          RemoveObjectForce( 'player', 2 )
          if playerState.currentAction ~= 2 then
            playerState.currentAction = 0
          end
          if curTime - playerState.onGroundTime < 0.1 then
            Object.Get( 'player' ).api:SetAnimation( 'player/mario', 'stay-right' )
          end
          playerState.lastDirection = 1
        end
      end
    end

    if id == 0x58 then  -- X - fire
      if isPressed then
        local x, y = Object.Get( 'player' ).api:GetPos()
        local object = 'player-bullet-.'..x..'.'..y..'.'..string.format( '%f', settings.timer )
        local isRight = playerState.lastDirection > 0 and true or false
        ObjectCreate( object, x + ( isRight and 30 or -30 ), y - 10, 0 )
        ObjectAttr( object, {
          collision = true, collisionSize = '12 12', collisionAcceleration = '0 900', collisionVelocity = ( isRight and 150 or -150 )..' -400', collisionStatic = false,
          lightBlockByCollision = true,
          lightPoint = true, lightPointSize = ( math.random( 0, 1000 ) / 1000.0 ) * 300.0 + 30.0, lightPointColor = string.format( '%f %f %f 1', math.random( 0, 800 ) / 1000.0 + 0.2, math.random( 0, 800 ) / 1000.0 + 0.2, math.random( 0, 800 ) / 1000.0 + 0.2 ), lightPointPenetration = 3
          } )
        Object.Get( object ).api:SetAnimation( 'bullet/test000', isRight and 'right' or 'left' )
        ListenCollision( object, 'CollisionBullet' )
      end
    end

    if id == 0x10 and isPressed then  -- test
      local object = 'wall.161.63.2.010000'
      Object.Get( 'object' ).api:SetAnimation( 'lift/small', 'down' )
    end
  end -- !settings.gamePaused

  if id == 0x8 then  -- BackSpace
    if isPressed then
      ObjectSetPos( 'player', 0, 0 )
      ObjectAttr( 'player', { collisionVelocity = '0 0' } )
    end
  end
end -- PlayerControl
]]

--[[
function PlayerDoLongJump( timerId )
  playerState.longJumpTimer = -1
  if playerState.isHoldJump then
    local vx, vy = ObjectAttr( 'player', { 'collisionVelocity' } )
    playerState.longJumpStep = playerState.longJumpStep + 1
    ObjectAttr( 'player', { collisionVelocity = vx..' '..( vy - playerState.longJumpPower / playerState.longJumpStep ) } )
    Core.SetTimer( 0.1, PlayerDoLongJump )
  end
end -- PlayerDoLongJump

function PlayerEndLongJump( timerId )
  playerState.isHoldJump = false
  playerState.PlayerEndLongJumpTimer = -1
  if playerState.PlayerEndLongJumpTimer ~= -1 then
    -- Core.StopTimer( playerState.PlayerEndLongJumpTimer )
    -- playerState.PlayerEndLongJumpTimer = -1
    playerState.longJumpTimer = -1
  end
end -- PlayerEndLongJump
]]

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

--[[ PushMushroom ]]
--[[
function PushMushroom( object )
  local itemName = object..'-mushroom-'..string.format( '%f', Core.GetTime() )
  local brickX, brickY = Object.Get( object ).api:GetPos()
  Object.New( itemName ).api:SetPos( brickX, brickY, -0.1 )
  -- ObjectAttr( itemName, { renderable = true, textureName = 'textures/items/mushroom.png', renderableSize = '32 32', collision = true, collisionSize = '32 32', collisionStatic = true } )
  Object.Get( itemName ).api:Attr({ renderable = true, textureName = 'textures/items/mushroom.png', renderableSize = '32 32', collision = true, collisionSize = '32 32', collisionStatic = true })
  animation[ 'timer'..Core.SetTimer( 0.1, DoAnimationMushroom ) ] = { step = 1, time = 0, object = itemName, tile = object }
  ObjectRemoveTag( object, 'has-mushroom' )
  -- ObjectAttr( object, { renderable = false } )
  Object.Get( object ).api:Attr({ renderable = false })
  ObjectSetAnimation( object, 'supermario/brick2', 'do' )
  animation[ 'timer'..Core.SetTimer( 0.5, DoAnimationBrick ) ] = { step = 1, time = 0, object = itemName, tile = object }
end -- PushMushroom
]]

--[[ PushCoin]]
--[[
function PushCoin( object )
  local itemName = object..'-coin-'..string.format( '%f', Core.GetTime() )
  local brickX, brickY = Object.Get( object ).api:GetPos()
  -- ObjectCreate( itemName, brickX, brickY, -1.0, true )
  Object.New( itemName, '', false ).api:SetPos( brickX, brickY, -1.0 )
  -- ObjectAttr( itemName, { renderable = true, textureName = 'textures/items/coin0.png', renderableSize = '32 32' } )
  ObjectSetAnimation( itemName, 'supermario/coin', 'do' )
  animation[ 'timer'..Core.SetTimer( 1.0, DoAnimationCoin ) ] = { step = 1, time = 0, object = itemName, tile = object }
  ObjectRemoveTag( object, 'has-coin' )
  -- ObjectAttr( object, { renderable = false, lightPoint = false } )
  Object.Get( object ).api:Attr({ renderable = false, lightPoint = false })
  ObjectSetAnimation( object, 'supermario/brick2', 'do' )
  animation[ 'timer'..Core.SetTimer( 0.5, DoAnimationBrick ) ] = { step = 1, time = 0, object = itemName, tile = object }
  -- ObjectAttr( object, { textureName = 'textures/tiles/bricks/brick2.png' } )
  -- ObjectAttr( itemName, { renderable = true, textureName = 'textures/tiles/coin0.png', renderableSize = '32 32' } )
  -- animation[ 'timer'..Core.SetTimer( 0.1, 'DoAnimationCoin' ) ] = { step = 1, time = 0, object = itemName, tile = object }
  -- ObjectRemoveTag( object, 'has-coin' )
end -- PushCoin
]]

--[[ DoAnimationMushroom ]]
--[[
function DoAnimationMushroom( timerId )
  local keyByTimer = 'timer'..timerId
  if animation[ keyByTimer ] == nil then
    do return false end
  end

  local anim = animation[ keyByTimer ]

  if anim.step == 1 then
    anim.time = anim.time + 1
    local x, y = Object.Get( anim.object ).api:GetPos()
    Object.Get( anim.object ).api:SetPos( x, y - 1 )
    if anim.time < 32 then
      animation[ 'timer'..Core.SetTimer( 1/40, DoAnimationMushroom ) ] = anim
    else
      -- ObjectAddTag( anim.tile, 'has-mushroom' )
      animation[ keyByTimer ] = nil
      -- ObjectAttr( anim.object, { collisionVelocity = '150 0', collisionAcceleration = '0 1200', collisionStatic = false } )
      Object.Get( anim.object ).api:Attr({ collisionVelocity = '150 0', collisionAcceleration = '0 1200', collisionStatic = false })
      ObjectAddTag( anim.object, 'mushroom' )
      ObjectAddTag( anim.object, 'no-reset-player-velocity' )
      ListenCollision( anim.object, 'CollisionMushroom' )
    end
  end
end -- DoAnimationMushroom
]]

--[[ DoAnimationCoin ]]
function DoAnimationCoin( timerId )
  local keyByTimer = 'timer'..timerId
  if animation[ keyByTimer ] == nil then
    do return false end
  end

  local anim = animation[ keyByTimer ]

  if anim.step == 1 then
    Object.Get( anim.object ).api:Destroy()
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
    Object.Get( anim.tile ).api:SetAnimation( 'supermario/brick2', 'default' )
    -- ObjectAttr( anim.tile, { renderable = true, textureName = 'textures/tiles/bricks/brick2.png', renderableSize = '32 32' } )
    Object.Get( anim.tile ).api:Attr({ renderable = true, textureName = 'textures/tiles/bricks/brick2.png', renderableSize = '32 32' })
    -- ObjectRemove( anim.object )
    animation[ keyByTimer ] = nil
  end
end -- DoAnimationBrick

--[[ CollisionMushroom ]]
--[[
function CollisionMushroom( mushroom, target, flags, vx, vy )
  if bit32.band( flags, 2 ) == 2 or bit32.band( flags, 8 ) == 8 then
    -- ObjectAttr( mushroom, { collisionVelocity = string.format( '%f %f', -vx, vy ) } )
    Object.Get( mushroom ).api:Attr({ collisionVelocity = string.format( '%f %f', -vx, vy ) })
  elseif bit32.band( flags, 4 ) == 4 then -- stay on target
    if ObjectHasTag( target, 'push-bottom' ) then
      -- ObjectAttr( mushroom, { collisionVelocity = string.format( '%f %f', vx, -300 ) } )
      Object.Get( mushroom ).api:Attr({ collisionVelocity = string.format( '%f %f', vx, -300 ) })
    end
  end
end -- CollisionMushroom
]]

--(
function CollisionBullet( bullet, target, flags, vx, vy )
  local isJumped = false
  if bit32.band( flags, 4 ) == 4 then
    vy = -vy * 0.9
    local x, y = Object.Get( bullet ).api:GetPos()
    y = y - 1
    Object.Get( bullet ).api:SetPos( x, y )
    isJumped = true
  end
  if bit32.band( flags, 2 ) == 2 and vx > 0 then
    vx = -math.abs( vx )
    x = x - 1
  elseif bit32.band( flags, 8 ) == 8 and vx < 0 then
    vx = math.abs( vx )
    x = x + 1
  end
  if isJumped and math.abs( vy ) < 100 then
    Object.Get( bullet ).api:Destroy()
  else
    -- ObjectAttr( bullet, { collisionVelocity = string.format( '%f %f', vx, vy ) } )
    Object.Get( bullet ).api:Attr({ collisionVelocity = string.format( '%f %f', vx, vy ) })
  end
end --)

--[[ UpdateCamera ]]
function UpdateCamera( timerId )
  local camera = Camera.Get( 'camera-mario-style' )
  local _, cy = Object.Get( camera ).api:GetPos()
  local px, _ = Object.Get( 'player' ).api:GetPos()
  Object.Get( camera ).api:SetPos( math.max( px, 0 ), cy )
  Core.SetTimer( 0, UpdateCamera, true ) -- стараться избегать 0-таймеров, т.к. они зависят от быстродействия приложения и исполняются каждый тик
end -- UpdateCamera


--[[
local objectsList = {}
for q = 1,10 do
  local object = Object.New( 'testobject123-14-'..q, '', true )
  object.api:SetAnimation( 'bullet/test000', 'right' )
  table.insert( objectsList, object )
end

function LuaObjectTest()
  local num = 1
  for id, object in pairs( objectsList ) do
    local obj = Object.Get( 'testobject123-14-'..num )
    Debug.Log( 'type of object = '..type( obj )..', fn = '..type( obj.fn ) )
    obj.api:Destroy()
    -- object.api:Destroy()
    num = num + 1
  end
end
Core.SetTimer( 3, 'LuaObjectTest' )
]]

Core.SetTimer( 0.1, function()
    --[[
    while true do
      local object = Object.Get( 'game-object' )
      if object == nil then
        break
      end
      object.api:Destroy()
    end
    local object = Object.New( 'game-object' )
    object.api:SetAnimation( 'editor/respawn', 'default' )
    object.api:SetPos( 0, 0 )
    object = object.api:SetScript( 'data/scripts/objects/test.lua' )
    object:Init()
    ]]

    -- local object = Object.Get( 'game-object' )
    -- object:DoSomething()
  end
)
