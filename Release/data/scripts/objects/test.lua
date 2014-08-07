return {
  Init = function( self ) --(
    Debug.Log( 'Init...' )
    self.data = {
      isInitialized = true,
      counter = 0,
      seed = Tools.GetRandomSeed(),
    }
    Debug.Log( 'Init done' )
  end, --) Init

  DoSomething = function( self ) --(
    self.data.counter = self.data.counter + 1
    Debug.Log( 'test.lua: counter = '..( self.data.counter ) )
  end, --) DoSomething

  OnCreate = function( self ) --(
    if self.data.isInitialized == nil then
      self:Init()
    end
    self:DoSomething()
    self.data.timer = 0
  end, --) OnCreate

  OnDestroy = function( self ) --(
  end, --) OnDestroy

  OnUpdate = function( self ) --(
    self.data.timer = self.data.timer + 0.05
    local x, y = math.sin( ( self.data.timer + self.data.seed ) / 2 + 3.141592/4 ) * 60, math.cos( self.data.timer + self.data.seed ) * 30
    self.api:SetPos( x, y )
  end, --) OnUpdate

  OnFrame = function( self ) --(
  end, --) OnFrame
}
