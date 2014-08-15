return {
  OnCreate = function( self ) --(
    Debug.Log( 'arrow-up: created' )
    self.api:Attr({
      renderable = true, textureName = 'textures/tiles/tile000b.png', renderableSize = '10 10'
    })
    self.mousePos = { x = 0, y = 0 }
    self.isFocused = false
    self.state = 0
  end, --) OnCreate

  OnDestroy = function( self )
    Debug.Log( 'arrow-up: destroyed' )
  end,

  Init = function( self, x, y ) --(
    self:Move( x, y )
  end, --) Init

  OnMouseKeyPress = function( self, id ) --(
    local result = false
    if self.isFocused then
      result = true
      settings.editorMode = 3
      settings.move.blockAxis.x = true
      self.state = 1
    end
    return result
  end, --) OnMouseKeyPress

  OnMouseKeyRelease = function( self, id ) --(
    if self.state == 1 then
      settings.move.blockAxis.x = false
    end
    self.state = 0
  end, --) OnMouseKeyRelease

  OnMouseMove = function( self, x, y ) --(
    self.mousePos.x = x
    self.mousePos.y = y
    local mx, my = GetScreenToWorld( self.mousePos.x, self.mousePos.y )
    local x, y = self.api:GetPos()
    local lastIsFocused = self.isFocused
    self.isFocused = ( mx >= x - 5 and mx <= x + 5 and my >= y - 5 and my <= y + 5 )
    if lastIsFocused ~= self.isFocused then
      self.api:Attr({ renderableSize = self.isFocused and '15 15' or '10 10' })
    end
  end, --) OnMouseMove

  Move = function( self, x, y ) --(
    self.api:SetPos( x, y - 55 )
  end, --) Move
}
