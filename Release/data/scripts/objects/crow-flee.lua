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
    local name = self.api:GetNameFull()
    if dist < 100 then
      local vx, vy = ObjectAttr( name, { 'collisionVelocity' } )
      vx = ( px < x ) and 100 or -100
      ObjectAttr( name, { collisionVelocity = vx..' '..vy } )
    else
      local _, vy = ObjectAttr( name, { 'collisionVelocity' } )
      ObjectAttr( name, { collisionVelocity = '0 '..vy } )
    end
  end,
}
