return {
  Reset = function( self ) --(
    -- self.data.savedValue = 0
  end, --)

  OnCreate = function( self ) --(
    Debug.Log( Tools.GetTimeFormatted( '%Y.%m.%d %H:%M:%S' )..' "'..self.api:GetName()..'" created' )
    if self.data.savedValue == nil then
      self.data.savedValue = 1
      self.data.playerState = {
        onGroundTime = -1,
        isHoldJump = false,
        jumpPower = -200,
        longJumpPower = 50,
        longJumpStep = 0,
        longJumpTimer = -1,
        PlayerEndLongJumpTimer = -1,
        lastDirection = 1,  -- direction of face
        currentAction = 0,  -- current action: 0-stay, 1-walk, 2-jump
        allowDoubleJump = true,
        jumpBeginTime = 0,
      }
      self.api:Attr({ collisionVelocity = '0 0' })
      self.api:AddTag( 'player' )
      self.api:Attr({ collisionAcceleration = '0 1500', lightPoint = true, lightPointSize = 400, lightPointColor = '1 1 1 1', lightPointPenetration = 3 })
    end
    Debug.Log( 'currentAction = '..self.data.playerState.currentAction )
    if( self.data.playerState.currentAction == 0 ) then
      self.api:SetAnimation( 'player/mario', 'stay-'..( self.data.playerState.lastDirection > 0 and 'right' or 'left' ) )
    elseif( self.data.playerState.currentAction == 2 ) then
      self.api:SetAnimation( 'player/mario', 'jump-'..( self.data.playerState.lastDirection > 0 and 'right' or 'left' ) )
    end
    Debug.Log( 'My name is "'..self.api:GetName()..'" and i\'m borned '..( self.data.savedValue )..' times with memory' )

    -- reset
    -- self.data.playerState.jumpPower = -200
    -- self.data.playerState.longJumpPower = 50
    -- local tmp = Object.New( self.api:GetNameFull()..'/tmp', '', false )
    -- tmp.api:Attr({ renderable = true, renderableSize = '10 10', textureName = 'temp/alik16.bmp', position = '20 0', renderableColor = '1 1 1 1' })
  end, --) OnCreate

  OnDestroy = function( self ) --(
    self.data.savedValue = self.data.savedValue + 1
    Debug.Log( Tools.GetTimeFormatted( '%Y.%m.%d %H:%M:%S' )..' "'..self.api:GetName()..'" destroyed' )
  end, --) OnDestroy

  OnCollision = function( self, target, flags, vx, vy ) --(
    local targetName = target.api:GetNameFull()
    local player = self.api:GetNameFull()
    -- Debug.Log( string.format( 'player OnCollision with "%s": flags[%d] v[%3.1f; %3.1f]', targetName, flags, vx, vy ) )
    if target.api:HasTag( 'no-reset-player-velocity' ) then
      self.api:Attr({ collisionVelocity = vx..' '..vy })
    end
    if target.api:HasTag( 'mushroom' ) then
      target.api:Destroy()
      return false
      -- local cx, cy, rx, ry = ObjectAttr( player, { 'collisionSize', 'renderableSize' } )
      -- ObjectAttr( player, { collisionSize = string.format( '%f %f', cx * 1.2, cy * 1.2 ), renderableSize = string.format( '%f %f', rx * 1.2, ry * 1.2 ) } )
      -- do return false end
    end
    if IsObjectUpperThis( player, targetName ) then
      self.data.playerState.allowDoubleJump = true
      self.data.playerState.onGroundTime = Core.GetTime()
      if self.data.playerState.currentAction == 2 then
        self.data.playerState.currentAction = 1
        self.api:SetAnimation( 'player/mario', ( math.abs( self.data.playerState.lastDirection ) > 1 and 'walk' or 'stay' )..'-'..( self.data.playerState.lastDirection > 0 and 'right' or 'left' ) )
      end
    elseif IsObjectUnderThis( player, targetName ) then
      --[[
      if ObjectHasTag( targetName, 'brick-breakable' ) then
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
        ) ] = { step = 1, time = 0, timeMax = 8, object = targetName }
        ObjectAddTag( targetName, 'push-bottom' )
        ObjectRemoveTag( targetName, 'brick-breakable' )
        ObjectStopAnimation( targetName )
        ObjectSetAnimation( targetName, 'supermario/brick0', 'do', 'stop' )
        ObjectAttr( targetName, { color = '0 0 0 0' } )
      end
      if ObjectHasTag( targetName, 'has-mushroom' ) then
        PushMushroom( targetName )
      end
      if ObjectHasTag( targetName, 'has-coin' ) then
        PushCoin( targetName )
      end
      ]]
    end

    if target.api:HasTag( 'kill' ) then
      self:DoKill( target, flags, vx, vy )
    end
  end, --) OnCollision

  OnKeyPress = function( self, id ) --(
    if GUIRenderer.focusedItem ~= nil then
      return
    end
    if settings.gamePaused then
    else
      if id == 0x26 or id == 0x57 then  -- Up
        local curTime = Core.GetTime()
        if curTime - self.data.playerState.onGroundTime < 0.1 or self.data.playerState.allowDoubleJump then  -- do jump
          if curTime - self.data.playerState.onGroundTime >= 0.1 then -- is a double jump
            self.data.playerState.allowDoubleJump = false
          end
          local vx, _ = self.api:Attr({ 'collisionVelocity' })
          self.api:Attr({ collisionVelocity = vx..' '..( self.data.playerState.jumpPower )  })
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
          --[[
          self.data.playerState.longJumpTimer = Core.SetTimer( 0.1, function( timerId )
              self.PlayerDoLongJump( self, timerId )
            end )
          self.data.playerState.PlayerEndLongJumpTimer = Core.SetTimer( 0.5, function( timerId ) self:PlayerEndLongJump( timerId ) end )
          ]]
          self.data.playerState.jumpBeginTime = Core.GetTime()
          self.data.playerState.onGroundTime = 0
          self.api:SetAnimation( 'player/mario', 'jump-'..( self.data.playerState.lastDirection > 0 and 'right' or 'left' ) )
          self.data.playerState.currentAction = 2
        end
      end

      if id == 0x25 or id == 0x41 then  -- Left
        local curTime = Core.GetTime()
        self.api:RemoveForce( 2 )
        self.api:SetForce( 4, -300, 0 )
        if self.data.playerState.currentAction ~= 2 then
          self.data.playerState.currentAction = 1
        end
        if curTime - self.data.playerState.onGroundTime < 0.1 then
          self.api:SetAnimation( 'player/mario', 'walk-left' )
        end
        self.data.playerState.lastDirection = -2
      end

      if id == 0x27 or id == 0x44 then  -- Right
        local curTime = Core.GetTime()
        self.api:RemoveForce( 4 )
        self.api:SetForce( 2, 300, 0 )
        self.data.playerState.lastDirection = 1
        if self.data.playerState.currentAction ~= 2 then
          self.data.playerState.currentAction = 1
        end
        if curTime - self.data.playerState.onGroundTime < 0.1 then
          self.api:SetAnimation( 'player/mario', 'walk-right' )
        end
        self.data.playerState.lastDirection = 2
      end

      if id == 0x58 then  -- X - fire
        local x, y = self.api:GetPos()
        local objectName = 'player-bullet-.'..x..'.'..y..'.'..string.format( '%f', settings.timer )
        local isRight = self.data.playerState.lastDirection > 0 and true or false
        -- ObjectCreate( object, x + ( isRight and 30 or -30 ), y - 10, 0 )
        local newObject = Object.New( objectName )
        newObject.api:SetPos( x + ( isRight and 30 or -30 ), y - 10, 0 )
        newObject.api:Attr({
          collision = true, collisionSize = '12 12', collisionAcceleration = '0 900', collisionVelocity = ( isRight and 150 or -150 )..' -400', collisionStatic = false,
          lightBlockByCollision = true,
          lightPoint = true, lightPointSize = ( math.random( 0, 1000 ) / 1000.0 ) * 300.0 + 30.0, lightPointColor = string.format( '%f %f %f 1', math.random( 0, 800 ) / 1000.0 + 0.2, math.random( 0, 800 ) / 1000.0 + 0.2, math.random( 0, 800 ) / 1000.0 + 0.2 ), lightPointPenetration = 3
          })
        newObject.api:SetAnimation( 'bullet/test000', isRight and 'right' or 'left' )
        newObject.api:SetScript( 'data/scripts/objects/bullet.lua' )
        -- ListenCollision( object, 'CollisionBullet' )
      end
    end -- !settings.gamePaused

    if id == 0x8 then  -- BackSpace
      self.api:SetPos( 0, 0 )
      self.api:Attr({ collisionVelocity = '0 0' })
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
        local curTime = Core.GetTime()
        if self.data.playerState.lastDirection < 0 then
          self.api:RemoveForce( 4 )
          if self.data.playerState.currentAction ~= 2 then
            self.data.playerState.currentAction = 0
          end
          if curTime - self.data.playerState.onGroundTime < 0.1 then
            self.api:SetAnimation( 'player/mario', 'stay-left' )
          end
          self.data.playerState.lastDirection = -1
        end
      end

      if id == 0x27 or id == 0x44 then  -- Right
        local curTime = Core.GetTime()
        if self.data.playerState.lastDirection > 0 then
          self.api:RemoveForce( 2 )
          if self.data.playerState.currentAction ~= 2 then
            self.data.playerState.currentAction = 0
          end
          if curTime - self.data.playerState.onGroundTime < 0.1 then
            self.api:SetAnimation( 'player/mario', 'stay-right' )
          end
          self.data.playerState.lastDirection = 1
        end
      end
    end -- !settings.gamePaused
  end,

  --[[
  PlayerDoLongJump = function( self, timerId ) --(
    self.data.playerState.longJumpTimer = -1
    if self.data.playerState.isHoldJump then
      local vx, vy = self.api:Attr({ 'collisionVelocity' })
      self.data.playerState.longJumpStep = self.data.playerState.longJumpStep + 1
      self.api:Attr({ collisionVelocity = vx..' '..( vy - self.data.playerState.longJumpPower / self.data.playerState.longJumpStep ) })
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
  ]]

  DoKill = function( self, target, flags, vx, vy ) --(
    if self.data.respawn ~= nil then
      local x, y = Object.Get( self.data.respawn ).api:GetPos()
      self.api:SetPos( x, y )
    end
  end, --) DoKill

  OnUpdate = function( self ) --(
    local currentTime = Core.GetTime()
    if( self.data.playerState.currentAction == 2 ) then
      local dt = currentTime - self.data.playerState.jumpBeginTime
      if( dt > 0.5 ) then --( end long jump
        self.data.playerState.isHoldJump = false
      else --)( long jump
        if self.data.playerState.isHoldJump then
          self.data.playerState.longJumpStep = self.data.playerState.longJumpStep + 1
          local vx, vy = self.api:Attr({ 'collisionVelocity' })
          self.api:Attr({ collisionVelocity = vx..' '..( vy - self.data.playerState.longJumpPower * ( 0.5 - dt ) ) })
        end
      end --)
    end
    if currentTime - self.data.playerState.onGroundTime < 0.1 then  -- on ground
      if self.data.playerState.currentAction == 2 then
        self.data.playerState.currentAction = 1
        self.api:SetAnimation( 'player/mario', ( math.abs( self.data.playerState.lastDirection ) > 1 and 'walk' or 'stay' )..'-'..( self.data.playerState.lastDirection > 0 and 'right' or 'left' ) )
      end
    else  -- on air
      if self.data.playerState.currentAction ~= 2 then
        self.data.playerState.currentAction = 2
        self.api:SetAnimation( 'player/mario', 'jump-'..( self.data.playerState.lastDirection > 0 and 'right' or 'left' ) )
      end
    end
  end, --) OnUpdate
}
