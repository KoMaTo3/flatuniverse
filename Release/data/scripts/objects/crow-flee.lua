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
  end, --) OnCreate

  OnUpdate = function( self )
    local x, y = self.api:GetPos()
    local px, py = Object.Get( 'player' ).api:GetPos()
    local dist = math.sqrt( ( px - x ) * ( px - x ) + ( py - y ) * ( py - y ) )
    if dist < 100 then
      local vx, vy = self.api:Attr({ 'collisionVelocity' })
      vx = ( px < x ) and 100 or -100
      self.api:Attr({ collisionVelocity = vx..' '..vy })
    else
      local _, vy = self.api:Attr({ 'collisionVelocity' })
      self.api:Attr({ collisionVelocity = '0 '..vy })
    end
  end,
}
