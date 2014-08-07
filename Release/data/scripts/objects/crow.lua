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
    Debug.Log( 'OnCreate crow' )
    self.player = Object.Get( 'player' )
    self.data.jumpCooldown = 0
  end, --) OnCreate

  OnUpdate = function( self )
    local x, y = self.api:GetPos()
    local px, py = Object.Get( 'player' ).api:GetPos()
    local dist = math.sqrt( ( px - x ) * ( px - x ) + ( py - y ) * ( py - y ) )
    if dist < 100 and self.data.jumpCooldown < 1 then
      local name = self.api:GetNameFull()
      local vx, vy = ObjectAttr( name, { 'collisionVelocity' } )
      vy = -700
      ObjectAttr( name, { collisionVelocity = vx..' '..vy } )
      Debug.Log( 'jump' )
    end

    self.data.jumpCooldown = self.data.jumpCooldown + 1
    -- self.api:SetPos( x + 1, y )
  end,

  OnCollision = function( self )
    self.data.jumpCooldown = 0
  end,
}
