return {
  damage = 100000,

  OnTrigger = function( self, target, isInTrigger ) --(
    if( isInTrigger ) then
      if( target.OnDamage ~= nil ) then
        target:OnDamage( self, self.damage )
      end
    end
  end, --) OnTrigger
}
