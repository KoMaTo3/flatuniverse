return {
  defaultSpeed = 50,
  defaultHealth = 1,

  Init = function( self ) --(
    self.data = {
      isInitialized = true,
      health = self.defaultHealth,
    }
  end, --) Init

  OnCreate = function( self ) --(
    if self.data.isInitialized == nil then
      self:Init()
    end
    self.name = self.api:GetNameFull()
  end, --) OnCreate

  OnCollision = function( self, target, flags, vx, vy ) --(
    local changeSpeed = false
    if ( bit32.band( flags, 8 ) == 8 ) then
      vx = -self.defaultSpeed
      changeSpeed = true
    elseif ( bit32.band( flags, 2 ) == 2 ) then
      vx = self.defaultSpeed
      changeSpeed = true
    end
    if changeSpeed then
      local _, vy = self.api:Attr({ 'collisionVelocity' })
      self.api:Attr({ collisionVelocity = vx..' '..vy })
    end
  end, --) OnCollision

  OnUpdate = function( self ) --(
    local _, y = self.api:GetPos()
    if y > 150 then
      self.api:Destroy()
    end
  end, --)
}
