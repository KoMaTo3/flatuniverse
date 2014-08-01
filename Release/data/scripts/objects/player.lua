return {
  TestUserData = function( self )
    Debug.Log( 'is a TestUserData' )
  end,

  Reset = function( self )
    self.data.savedValue = 0
  end,

  OnCreate = function( self )
    Debug.Log( Tools.GetTime( '%Y.%m.%d %H:%M:%S' )..' "'..self.api:GetName()..'" created' )
    if self.data.savedValue == nil then
      self.data.savedValue = 1
    end
    Debug.Log( 'My name is "'..self.api:GetName()..'" and i\'m borned '..( self.data.savedValue )..' times with memory' )
  end,

  OnDestroy = function( self )
    self.data.savedValue = self.data.savedValue + 1
    Debug.Log( Tools.GetTime( '%Y.%m.%d %H:%M:%S' )..' "'..self.api:GetName()..'" destroyed' )
  end,
}
