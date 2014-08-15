return {
  timer = 0,
  cooldownTimer = 100,

  OnCreate = function( self ) --(
    self.triggerFieldName = '-trigger-'..self.api:GetNameFull()
  end, --) OnCreate

  OnTrigger = function( self, target, isInTrigger ) --(
    if isInTrigger then
      Debug.Log( 'isInTrigger' )
      local triggerCooldown = target[ self.triggerFieldName ] == nil and -self.cooldownTimer or target[ self.triggerFieldName ]
      Debug.Log( 'triggerCooldown = '..triggerCooldown )
      Debug.Log( 'self.timer = '..self.timer )
      if self.timer > triggerCooldown + self.cooldownTimer then
        local vx, vy = target.api:Attr({ 'collisionVelocity' })
        vy = vy - 1000
        target.api:Attr({ collisionVelocity = vx..' '..vy })
        target[ self.triggerFieldName ] = self.timer
      end
    end
  end, --) OnTrigger

  OnUpdate = function( self ) --(
    self.timer = self.timer + 1
  end, --) OnUpdate
}
