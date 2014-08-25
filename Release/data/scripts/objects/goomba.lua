return {
  defaultSpeed = 50,
  defaultHealth = 1,

  Init = function( self ) --(
    self.data = {
      isInitialized = true,
      health = self.defaultHealth,
      state = 0,  -- 0: live, 1:death
      timerDeath = 0,
    }
    self.api:SetForce( 0, -self.defaultSpeed, 0 )
  end, --) Init

  OnCreate = function( self ) --(
    if self.data.isInitialized == nil then
      self:Init()
    end
    self.name = self.api:GetNameFull()
    if( self.data.state == 1 ) then
      self.api:Destroy()
    end
    self.animation = Object.New( 'animation', self.api:GetNameFull(), false )
    self.animation.api:SetAnimation( 'enemy/goomba/0', 'default' )
    Debug.Log( 'goomba renderableOffset...' )
  end, --) OnCreate

  OnCollision = function( self, target, flags, vx, vy ) --(
    if( self.data.state == 0 ) then --(
      local changeSpeed = false
      if ( bit32.band( flags, 8 ) == 8 ) then
        -- vx = -self.defaultSpeed
        -- changeSpeed = true
        self.api:SetForce( 0, -self.defaultSpeed, 0 )
      elseif ( bit32.band( flags, 2 ) == 2 ) then
        -- vx = self.defaultSpeed
        -- changeSpeed = true
        self.api:SetForce( 0, self.defaultSpeed, 0 )
      end
      --[[
      if changeSpeed then
        local _, vy = self.api:Attr({ 'collisionVelocity' })
        self.api:Attr({ collisionVelocity = vx..' '..vy })
      end
      ]]
    end --) state 0
  end, --) OnCollision

  OnUpdate = function( self ) --(
    local _, y = self.api:GetPos()
    if y > 150 then
      self.api:Destroy()
    end
  end, --)

  OnDamage = function( self, target, damage ) --(
    local result = false
    if( self.data.state == 0 ) then --(
      if( self.data.health > 0 ) then --(
        self.data.health = self.data.health - damage
        result = true
        if( self.data.health <= 0 ) then
          self:OnKill( target )
        end
      end --) health
    end --) state 0
    return result
  end, --) OnDamage

  OnKill = function( self, target ) --(
    self.data.state = 1
    local x, y, _, cx, cy = self.api:Attr({ 'position', 'collisionSize' })
    Debug.Log( string.format( 'position[ %3.1f,%3.1f ] collisionSize[ %3.1f; %3.1f ]', x, y, cx, cy ) )
    -- y = y + 5
    cx = cx * 0.7
    cy = cy * 0.5
    self.api:Attr({ collisionSize = cx..' '..cy, renderableOffset = '0 -20' })
    self.api:RemoveForce( 0 )
    self.animation.api:Attr({ position = '0 -6' })
    self.animation.api:SetAnimation( 'enemy/goomba/0', 'death', function()
        self.api:Destroy()
      end
    )
  end, --) OnKill
}
