GUISliderVertical = {}


--[[ GUISliderVertical:GetType ]]
function GUISliderVertical:GetType()
  return 'SliderVertical'
end -- GUISliderVertical:GetType


--[[ GUISliderVertical:SetText ]]
function GUISliderVertical:SetText( setText )
  if setText < self.values.min then
    setText = self.values.min
  elseif setText > self.values.max then
    setText = self.values.max
  end
  self.value = setText
end -- GUISliderVertical:SetText


--[[ GUISliderVertical:GetText ]]
function GUISliderVertical:GetText()
  return self.value
end -- GUISliderVertical:GetText

--[[ GUISliderVertical:GetValue ]]
function GUISliderVertical:GetValue()
  return self.value
end -- GUISliderVertical:GetValue


--[[ GUISliderVertical:SetEnabled ]]
function GUISliderVertical:SetEnabled( setIsEnabled )
  self.isEnabled = setIsEnabled
end -- GUISliderVertical:SetEnabled


function GUISliderVertical:GetOffset()
  local dx, dy = 0, 0
  if self.parent ~= nil then
    dx, dy = self.parent:GetOffset()
  end
  return self.rect.left + dx, self.rect.top + dy
end


--[[ GUISliderVertical:Create ]]
function GUISliderVertical:Create( x0, y0, setHeight, setMinValue, setMaxValue, setValue, setTitle, setOnChangeHandler, parent, setIsEnabled, setTextFormat )
  local obj = {
    childs = {},
    parent = parent,
    rect = {
      left = x0,
      top = y0,
      right = x0 + 16,
      bottom = y0,
    },
    state = 0,  -- 0:free, 1:press, 2:hover, 3:press+out
    isHover = false,
    hoverPos = { x = 0, y = 0 },
    OnChangeHandler = setOnChangeHandler,
    value = setValue,
    valueDefault = setValue,
    values = {
      min = setMinValue,
      max = setMaxValue,
    },
    title = setTitle,
    height = setHeight,
    rulerOffset = 0,
    scale = {
      offset = 0,
    },
    textFormat = setTextFormat == nil and '%d' or setTextFormat,
    isEnabled = setIsEnabled == nil and true or setIsEnabled,
  }
  self.__index = self
  local res = setmetatable( obj, self )
  if parent == nil then
    GUIRenderer.GUIElements[ #GUIRenderer.GUIElements + 1 ] = obj
  else
    parent.childs[ #parent.childs + 1 ] = obj
  end
  return res
end -- GUISliderVertical:Create

function GUISliderVertical:Render( dx, dy )
  if dx == nil then
    dx = 0
    dy = 0
  end
  local x0 = self.rect.left + dx
  local y0 = self.rect.top + dy
  local x1 = self.rect.right + dx
  local y1 = self.rect.bottom + dy
  local resRulerOffset = 0

  if self.state == 1 then
    local halfHeight = math.floor( self.height / 2 )
    local bgTop = y0 + self.scale.offset - halfHeight
    local bgBottom = y0 + self.scale.offset + halfHeight
    local topRulerOffset = -halfHeight
    local bottomRulerOffset = halfHeight
    local lastRulerOffset = self.rulerOffset
    self.rulerOffset = self.GetRulerOffset( self, dx, dy )
    resRulerOffset = self.scale.offset
    if self.rulerOffset ~= lastRulerOffset then
      local coeff = 1.0 - ( self.rulerOffset + halfHeight ) / self.height
      self.value = self.values.min + coeff * ( self.values.max - self.values.min )
      if self.OnChangeHandler ~= nil then
        self.OnChangeHandler( self )
      end
      GUI.tooltip:SetPosition( x0 + 20, y0 + self.rulerOffset + resRulerOffset - 6 )
      GUI.tooltip:SetText( string.format( self.textFormat, self.value ) )
      GUI.tooltip:CropByTextWidth()
    end
    Render( 'sprite', x0 + 3, bgTop - 2, -5, x1 - 7, bgBottom + 2, -5, 'data/temp/blank.png', 'ffffffff' )
    Render( 'rect', x0 + 3, bgTop - 3, -5, x1 - 6, bgBottom + 2, -5, '444444ff' )
    Render( 'sprite', x0 + 5, bgTop, -5, x1 - 9, bgBottom, -5, 'data/temp/blank.png', '888888aa' )
  else
    self.rulerOffset = 0
  end
  Render( 'sprite', x0, y0 + self.rulerOffset + resRulerOffset - 4, -5, x1, y1 + self.rulerOffset + resRulerOffset + 4, -5, 'data/textures/gui/slider-vertical.tga', self.isHover and 'aa4444ff' or 'ffffffff' )
  if self.isHover then
    local halfWidth = math.floor( Render( 'getTextWidth', self.title ) / 2 )
    Render( 'text', x0 + 8 - halfWidth, y0 + 15, -1, self.title, '000000ff' )
  end

  -- childs
  for key, item in pairs( self.childs ) do
    item:Render( dx + self.rect.left, dy + self.rect.top )
  end
end -- GUISliderVertical:Render

function GUISliderVertical:TestInRect( x, y, dx, dy )
  if dx == nil then
    dx = 0
    dy = 0
  end
  local result = false
  if self.state == 1 then
    result = true
    self.isHover = true
  else
    if x >= self.rect.left + dx and x <= self.rect.right + dx and
       y >= self.rect.top - 4 + dy and y <= self.rect.bottom + 4 + dy then
      result = true
      self.isHover = true
      self.hoverPos.x = x - ( self.rect.left + dx )
      self.hoverPos.y = y - ( self.rect.top + dy )
    else
      self.isHover = false
    end
  end

  -- childs
  for key, item in pairs( self.childs ) do
    if item:TestInRect( mousePos.x, mousePos.y, dx + self.rect.left, dy + self.rect.top ) then
      if GUIRenderer.activeItem == nil then
        GUIRenderer.activeItem = item
      end
      do return true end
    end
  end
  if result and GUIRenderer.activeItem == nil then
    GUIRenderer.activeItem = self
  end
  return result
end -- GUISliderVertical:TestInRect

function GUISliderVertical:GetRulerOffset( dx, dy )
  local halfHeight = math.floor( self.height / 2 )
  return math.min( math.max( mousePos.y - self.scale.offset - ( dy + self.rect.top ), -halfHeight ), halfHeight )
end

function GUISliderVertical:OnClick( id, isPressed )
  if isPressed then --(
    if id == 2 then
      local oldValue = self.value
      self.value = self.valueDefault
      if self.value ~= oldValue and self.OnChangeHandler ~= nil then
        self.OnChangeHandler( self )
      end
    else
      if self.isHover then --(
        self.state = 1
        GUIRenderer.focusedItem = self
        local coeff = ( self.value - self.values.min ) / ( self.values.max - self.values.min ) - 0.5
        self.scale.offset = self.height * coeff
      else --) (
      end
    end
  else  --) (
    GUIRenderer.focusedItem = nil
    self.state = 0
    self.isHover = false
    GUI.tooltip:SetPosition( 0, settings.windowSize.y )
  end --)
end -- GUISliderVertical:OnClick
