return {
  Reset = function( self ) --(
    self.data.savedValue = 0
  end, --)

  OnCreate = function( self ) --(
    Debug.Log( Tools.GetTime( '%Y.%m.%d %H:%M:%S' )..' "'..self.api:GetName()..'" created' )
    if self.data.savedValue == nil then
      self.data.savedValue = 1
    end
    self.data.playerState = {
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
    Debug.Log( 'My name is "'..self.api:GetName()..'" and i\'m borned '..( self.data.savedValue )..' times with memory' )
  end, --) OnCreate

  OnDestroy = function( self ) --(
    self.data.savedValue = self.data.savedValue + 1
    Debug.Log( Tools.GetTime( '%Y.%m.%d %H:%M:%S' )..' "'..self.api:GetName()..'" destroyed' )
  end, --) OnDestroy

  OnCollision = function( self, target, flags, vx, vy ) --(
    local target = target.api:GetNameFull()
    local player = self.api:GetNameFull()
    -- Debug.Log( string.format( 'player OnCollision with "%s": flags[%d] v[%3.1f; %3.1f]', target, flags, vx, vy ) )
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
      self.data.playerState.allowDoubleJump = true
      self.data.playerState.onGroundTime = GetTime()
      if self.data.playerState.currentAction == 2 then
        self.data.playerState.currentAction = 1
        ObjectSetAnimation( player, 'player/mario', ( math.abs( self.data.playerState.lastDirection ) > 1 and 'walk' or 'stay' )..'-'..( self.data.playerState.lastDirection > 0 and 'right' or 'left' ) )
      end
    elseif IsObjectUnderThis( player, target ) then
      --[[
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
        ObjectSetAnimation( target, 'supermario/brick0', 'do', 'stop' )
        ObjectAttr( target, { color = '0 0 0 0' } )
      end
      if ObjectHasTag( target, 'has-mushroom' ) then
        PushMushroom( target )
      end
      if ObjectHasTag( target, 'has-coin' ) then
        PushCoin( target )
      end
      ]]
    end
  end, --) OnCollision

  OnKeyPress = function( self, id ) --(
    if GUIRenderer.focusedItem ~= nil then
      return
    end
    if settings.gamePaused then
    else
      Debug.Log( string.format( 'player key: %d', id ) )
      if id == 0x26 or id == 0x57 then  -- Up
        local curTime = GetTime()
        if curTime - self.data.playerState.onGroundTime < 0.1 or self.data.playerState.allowDoubleJump then  -- do jump
          if curTime - self.data.playerState.onGroundTime >= 0.1 then -- is a double jump
            self.data.playerState.allowDoubleJump = false
          end
          local vx, _ = ObjectAttr( 'player', { 'collisionVelocity' } )
          ObjectAttr( 'player', { collisionVelocity = vx..' '..( self.data.playerState.jumpPower )  } )
          self.data.playerState.isHoldJump = true
          self.data.playerState.longJumpStep = 0
          if self.data.playerState.longJumpTimer ~= -1 then
            Core.StopTimer( self.data.playerState.longJumpTimer )
            self.data.playerState.longJumpTimer = -1
          end
          if self.data.playerState.PlayerEndLongJumpTimer ~= -1 then
            Core.StopTimer( self.data.playerState.PlayerEndLongJumpTimer )
            self.data.playerState.PlayerEndLongJumpTimer = -1
          end
          self.data.playerState.longJumpTimer = Core.SetTimer( 0.1, function( timerId ) self:PlayerDoLongJump( timerId ) end )
          self.data.playerState.PlayerEndLongJumpTimer = Core.SetTimer( 0.5, function( timerId ) self:PlayerEndLongJump( timerId ) end )
          self.data.playerState.onGroundTime = 0
          ObjectSetAnimation( 'player', 'player/mario', 'jump-'..( self.data.playerState.lastDirection > 0 and 'right' or 'left' ) )
          self.data.playerState.currentAction = 2
        end
      end

      if id == 0x25 or id == 0x41 then  -- Left
        local curTime = GetTime()
        RemoveObjectForce( 'player', 2 )
        SetObjectForce( 'player', 4, -300, 0 )
        if self.data.playerState.currentAction ~= 2 then
          self.data.playerState.currentAction = 1
        end
        if curTime - self.data.playerState.onGroundTime < 0.1 then
          ObjectSetAnimation( 'player', 'player/mario', 'walk-left' )
        end
        self.data.playerState.lastDirection = -2
      end

      if id == 0x27 or id == 0x44 then  -- Right
        local curTime = GetTime()
        RemoveObjectForce( 'player', 4 )
        SetObjectForce( 'player', 2, 300, 0 )
        self.data.playerState.lastDirection = 1
        if self.data.playerState.currentAction ~= 2 then
          self.data.playerState.currentAction = 1
        end
        if curTime - self.data.playerState.onGroundTime < 0.1 then
          ObjectSetAnimation( 'player', 'player/mario', 'walk-right' )
        end
        self.data.playerState.lastDirection = 2
      end

      if id == 0x58 then  -- X - fire
        local x, y = Object.Get( 'player' ).api:GetPos()
        local object = 'player-bullet-.'..x..'.'..y..'.'..string.format( '%f', settings.timer )
        local isRight = self.data.playerState.lastDirection > 0 and true or false
        ObjectCreate( object, x + ( isRight and 30 or -30 ), y - 10, 0 )
        ObjectAttr( object, {
          collision = true, collisionSize = '12 12', collisionAcceleration = '0 900', collisionVelocity = ( isRight and 150 or -150 )..' -400', collisionStatic = false,
          lightBlockByCollision = true,
          lightPoint = true, lightPointSize = ( math.random( 0, 1000 ) / 1000.0 ) * 300.0 + 30.0, lightPointColor = string.format( '%f %f %f 1', math.random( 0, 800 ) / 1000.0 + 0.2, math.random( 0, 800 ) / 1000.0 + 0.2, math.random( 0, 800 ) / 1000.0 + 0.2 ), lightPointPenetration = 3
          } )
        ObjectSetAnimation( object, 'bullet/test000', isRight and 'right' or 'left' )
        ListenCollision( object, 'CollisionBullet' )
      end

      if id == 0x10 then  -- test
        local object = 'wall.161.63.2.010000'
        ObjectSetAnimation( object, 'lift/small', 'down' )
      end
    end -- !settings.gamePaused

    if id == 0x8 then  -- BackSpace
      ObjectSetPos( 'player', 0, 0 )
      ObjectAttr( 'player', { collisionVelocity = '0 0' } )
    end
  end, --) OnKeyPress

  OnKeyRelease = function( self, id )
    if GUIRenderer.focusedItem ~= nil then
      return
    end
    if settings.gamePaused then
    else
      if id == 0x26 or id == 0x57 then  -- Up
        self.data.playerState.isHoldJump = false
      end

      if id == 0x25 or id == 0x41 then  -- Left
        local curTime = GetTime()
        if self.data.playerState.lastDirection < 0 then
          RemoveObjectForce( 'player', 4 )
          if self.data.playerState.currentAction ~= 2 then
            self.data.playerState.currentAction = 0
          end
          if curTime - self.data.playerState.onGroundTime < 0.1 then
            ObjectSetAnimation( 'player', 'player/mario', 'stay-left' )
          end
          self.data.playerState.lastDirection = -1
        end
      end

      if id == 0x27 or id == 0x44 then  -- Right
        local curTime = GetTime()
        if self.data.playerState.lastDirection > 0 then
          RemoveObjectForce( 'player', 2 )
          if self.data.playerState.currentAction ~= 2 then
            self.data.playerState.currentAction = 0
          end
          if curTime - self.data.playerState.onGroundTime < 0.1 then
            ObjectSetAnimation( 'player', 'player/mario', 'stay-right' )
          end
          self.data.playerState.lastDirection = 1
        end
      end
    end -- !settings.gamePaused
  end,

  PlayerDoLongJump = function( self, timerId ) --(
    self.data.playerState.longJumpTimer = -1
    if self.data.playerState.isHoldJump then
      local vx, vy = ObjectAttr( 'player', { 'collisionVelocity' } )
      self.data.playerState.longJumpStep = self.data.playerState.longJumpStep + 1
      ObjectAttr( 'player', { collisionVelocity = vx..' '..( vy - self.data.playerState.longJumpPower / self.data.playerState.longJumpStep ) } )
      Core.SetTimer( 0.1, function( timerId ) self:PlayerDoLongJump( timerId ) end )
    end
  end, --) PlayerDoLongJump

  PlayerEndLongJump = function( self, timerId ) --(
    self.data.playerState.isHoldJump = false
    self.data.playerState.PlayerEndLongJumpTimer = -1
    if self.data.playerState.PlayerEndLongJumpTimer ~= -1 then
      -- Core.StopTimer( self.data.playerState.PlayerEndLongJumpTimer )
      -- self.data.playerState.PlayerEndLongJumpTimer = -1
      self.data.playerState.longJumpTimer = -1
    end
  end, --) PlayerEndLongJump

  OnTrigger = function( self, target, isInTrigger ) --(
    Debug.Log( 'OnTrigger' )
  end, --) OnTrigger
}
