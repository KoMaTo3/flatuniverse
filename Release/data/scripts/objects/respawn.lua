return {
  OnCreate = function( self ) --(
    self:Init()
  end, --) OnCreate

  Init = function( self ) --(
  end, --) Init

  OnTrigger = function( self, target ) --(
    if target.api:HasTag( 'bullet' ) then
      local player = Object.Get( 'editor' ):FindPlayer()
      player.data.respawn = self.api:GetNameFull()
    end
  end, --) OnTrigger
}
