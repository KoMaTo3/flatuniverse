--[[
  обычные кирпичи, можно бить головой
  state:
    0 - default
    1 - смещение из-за удара
    2 - неподвижный после удара
]]

return {
  OnCreate = function( self ) --(
    if self.data.state == nil then
      self.data = {
        timer = 0,
      }
    end
    self.data.state = 0
    self.moveDistance = 5
    self.moveSpeed = 3.1415 / ( 0.5 * 60 )
  end, --) OnCreate

  OnCollision = function( self, target, flags, vx, vy ) --(
    -- push brick
    if( self.data.state == 0 ) and ( bit32.band( flags, 4 ) == 4 ) then
      local _x, _y = self.api:GetPos()
      self.data.position = {
        x = _x,
        y = _y,
      }
      self.data.timer = 0
      self.api:AddTag( 'push-bottom' )
      local vx, vy = target.api:Attr({ 'collisionVelocity' })
      if( vy < 0 ) then
        vy = 0
        target.api:Attr({ collisionVelocity = vx..' '..vy })
      end
      --[[ self.api:SetAnimation( 'supermario/brick0', 'do', function( self )
          Debug.Log( 'animation callback' )
          Debug.Log( 'animation callback: '..self.api:GetNameFull() )
          self.data.state = 0
          self.api:SetAnimation( 'supermario/brick0', 'default' )
          self.api:RemoveTag( 'push-bottom' )
          self.api:SetPos( self.data.position.x, self.data.position.y )
        end
      )
      ]]
      self.data.state = 1

      local cx, cy = self.api:Attr({ 'collisionSize' })
      self.api:Attr({ trigger = true, triggerSize = cx..' '..cy --[[ , triggerOffset = '40 0' ]] })
    end
  end, --) OnCollision

  OnUpdate = function( self ) --(
    if self.data.state == 1 then
      self.data.timer = self.data.timer + self.moveSpeed
      if self.data.timer < 3.1415 then
        local y = self.data.position.y - math.sin( self.data.timer ) * self.moveDistance
        self.api:SetPos( self.data.position.x, y )
      else
        self.data.state = 0
        self.api:RemoveTag( 'push-bottom' )
        self.api:SetPos( self.data.position.x, self.data.position.y )
      end
    end
  end, --) OnUpdate

  OnTrigger = function( self, target, isInTrigger ) --(
    if self.data.state == 1 then
      local vx, vy = target.api:Attr({ 'collisionVelocity' })
      if( vy >= 0 ) then
        vy = vy - 300
        target.api:Attr({ collisionVelocity = vx..' '..vy })
      end
    end
  end, --) OnTrigger
}
