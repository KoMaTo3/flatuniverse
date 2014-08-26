return {
  OnCollision = function( self, target, flags, vx, vy ) --(
    if ( bit32.band( flags, 4 ) == 4 ) then
      local vx, vy = self.api:Attr({ 'collisionVelocity' })
      vy = -300
      self.api:Attr({ collisionVelocity = vx..' '..vy })
    end
    if ( bit32.band( flags, 2 ) == 2 ) or ( bit32.band( flags, 8 ) == 8 ) then
      self.api:Destroy()
    end
  end, --) OnCollision

  OnUpdate = function( self ) --(
  end, --) OnUpdate
}
