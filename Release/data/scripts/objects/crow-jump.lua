return {
  Init = function( self ) --(
    self.data = {
      isInitialized = true,
    }
  end, --) Init

  OnCreate = function( self ) --(
    if self.data.isInitialized == nil then
      self:Init()
    end
    self.player = Object.Get( 'player' )
    self.data.jumpCooldown = 0
  end, --) OnCreate

  OnUpdate = function( self )
    local x, y = self.api:GetPos()
    local px, py = Object.Get( 'player' ).api:GetPos()
    local dist = math.sqrt( ( px - x ) * ( px - x ) + ( py - y ) * ( py - y ) )
    if dist < 100 and self.data.jumpCooldown < 1 then
      local vx, vy = self.api({ 'collisionVelocity' })
      vy = -700
      self.api:Attr({ collisionVelocity = vx..' '..vy })
    end

    self.data.jumpCooldown = self.data.jumpCooldown + 1
    -- self.api:SetPos( x + 1, y )
  end,

  OnCollision = function( self, target, flags, vx, vy )
    self.data.jumpCooldown = 0
  end,
}
