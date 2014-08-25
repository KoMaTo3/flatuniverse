return {
  OnTrigger = function( self, target, isInTrigger ) --(
    if( target.OnCoin ~= nil ) then
      target:OnCoin( self )
      self.api:Destroy()
    end
  end, --) OnTrigger
}
