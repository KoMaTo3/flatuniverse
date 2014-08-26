return {
  damage = 100000,

  OnTrigger = function( self, target, isInTrigger ) --(
    if( target.OnDamage ~= nil ) then
      target:OnDamage( self, self.damage )
    end
  end, --) OnTrigger
}
