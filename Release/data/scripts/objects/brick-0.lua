--[[
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
  end, --) OnCreate

  OnCollision = function( self, target, flags, vx, vy ) --(
    Debug.Log( 'brick OnCollision, flags: '..flags..', name: '..target.api:GetNameFull() )
    if bit32.band( flags, 1 ) == 1 and self.data.state == 0 then
      local _x, _y = self.api:GetPos()
      self.data.position = {
        x = _x,
        y = _y,
      }
      self.data.timer = 0
      self.api:AddTag( 'push-bottom' )
      Debug.Log( 'brick OnCollision => state 1' )
      self.api:SetAnimation( 'supermario/brick0', 'do', function( self )
          Debug.Log( 'animation callback' )
          Debug.Log( 'animation callback: '..self.api:GetNameFull() )
          self.data.state = 0
          self.api:SetAnimation( 'supermario/brick0', 'default' )
          self.api:RemoveTag( 'push-bottom' )
          self.api:SetPos( self.data.position.x, self.data.position.y )
        end
      )
      self.data.state = 1
    end

    if bit32.band( flags, 4 ) == 4 and self.data.state == 1 then
      Debug.Log( 'test collision' )
      local name = target.api:GetNameFull()
      local isStatic, vx, vy = ObjectAttr( name, { 'collisionStatic', 'collisionVelocity' } )
      if isStatic == false and vy >= 0 then
        vy = -500
        ObjectAttr( name, { collisionVelocity = vx..' '..vy } )
      end
    end
  end, --) OnCollision

  OnUpdate = function( self ) --(
    if self.data.state == 1 then
      self.data.timer = self.data.timer + 3.1415 / ( 0.5 * 60 )
      local y = self.data.position.y - math.sin( self.data.timer ) * 15
      self.api:SetPos( self.data.position.x, y )
    end
  end, --) OnUpdate
}
