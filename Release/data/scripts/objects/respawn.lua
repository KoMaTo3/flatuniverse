return {
  OnCreate = function( self ) --(
    self:Init()
  end, --) OnCreate

  Init = function( self ) --(
  end, --) Init

  OnTrigger = function( self, target, isInTrigger ) --(
    if( isInTrigger and target.OnSetRespawn ~= nil ) then
      target:OnSetRespawn( self )
    end
  end, --) OnTrigger
}
