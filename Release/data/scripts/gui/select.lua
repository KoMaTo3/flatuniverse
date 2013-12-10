GUISelect = {}


--[[ GUISelect:GetType ]]
function GUISelect:GetType()
  return 'Select'
end -- GUISelect:GetType


--[[ GUISelect:SetText ]]
function GUISelect:SetText( setText )
  for value,name in pairs( self.values ) do
    if name == setText then
      self.value = value
      do break end
    end
  end
end -- GUISelect:SetText


--[[ GUISelect:GetText ]]
function GUISelect:GetText()
  return self.values[ self.value ]
end -- GUISelect:GetText

--[[ GUISelect:GetValue ]]
function GUISelect:GetValue()
  return self.value
end -- GUISelect:GetValue


--[[ GUISelect:SetEnabled ]]
function GUISelect:SetEnabled( setIsEnabled )
  self.isEnabled = setIsEnabled
end -- GUISelect:SetEnabled


function GUISelect:GetOffset()
  local dx, dy = 0, 0
  if self.parent ~= nil then
    dx, dy = self.parent:GetOffset()
  end
  return self.rect.left + dx, self.rect.top + dy
end


--[[ GUISelect:Create ]]
function GUISelect:Create( x0, y0, width, setValue, setValuesList, setOnChangeHandler, parent, setIsEnabled )
  local defaultHeight = 16
  local obj = {
    childs = {},
    parent = parent,
    rect = {
      left = x0,
      top = y0,
      right = x0 + width,
      bottom = y0 + defaultHeight,
      height = defaultHeight,
    },
    state = 0,  -- 0:free, 1:press, 2:hover, 3:press+out
    isHover = false,
    hoverPos = { x = 0, y = 0 },
    OnChangeHandler = setOnChangeHandler,
    value = setValue,
    values = setValuesList,
    valueHover = nil,
    colors = {
      light = 'ffffffff',
      middle0 = 'eeeeeeff',
      middle1 = 'd1d1d1ff',
      dark = '888888ff',
      default = {
        bg = 'eeeeeeff',
        text = '000000ff',
      },
      focus = {
        bg = 'aa4444ff',
        text = 'ffffffff',
      },
      current = {
        bg = '000077ff',
        text = 'ffffffff',
      },
    },
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
end -- GUISelect:Create

function GUISelect:Render( dx, dy )
  if dx == nil then
    dx = 0
    dy = 0
  end
  local x0 = self.rect.left + dx
  local y0 = self.rect.top + dy
  local x1 = self.rect.right + dx
  local y1 = self.rect.bottom + dy

  Render( 'sprite', x0, y0, 0, x1, ( y0 + y1 ) / 2, 0, 'data/temp/blank.png', self.colors.middle0 )
  Render( 'sprite', x0, ( y0 + y1 ) / 2, 0, x1, y1, 0, 'data/temp/blank.png', self.colors.middle1 )
  Render( 'line', x0, y0, 0, x1, y0, 0, self.colors.light )
  Render( 'line', x0, y0, 0, x0, y1, 0, self.colors.light )
  Render( 'line', x0, y1, 0, x1, y1, 0, self.colors.dark )
  Render( 'line', x1, y0, 0, x1, y1, 0, self.colors.dark )
  Render( 'text', x0 + 3,y0, 0, self.values[ self.value ], '000000ff' )
  Render( 'sprite', x1 - 10, y0 + 7, 0, x1 - 3, y0 + 11, 0, 'data/textures/gui/select.png', 'ffffffff' )

  if self.state == 1 then
    local y = y1 + 1, color
    for value,name in pairs( self.values ) do
      color = ( self.valueHover == value ) and self.colors.focus or ( ( self.value == value ) and self.colors.current or self.colors.default )
      Render( 'sprite', x0, y - 1, 0, x1, y + self.rect.height - 1, 0, 'data/temp/blank.png', color.bg )
      Render( 'line', x0, y + self.rect.height - 1, 0, x1, y + self.rect.height - 1, 0, self.colors.dark )
      Render( 'text', x0 + 5, y, 0, name, color.text )
      y = y + self.rect.height
    end
  end

  --[[
  local isPushed = ( self.state == 1 and self.isHover ) and true or false
  local color = self.colors[ isPushed and 1 or 0 ]
  if isPushed then
    dx = dx + 1
    dy = dy + 1
  end
  ]]
  -- childs
  for key, item in pairs( self.childs ) do
    item:Render( dx + self.rect.left, dy + self.rect.top )
  end
end -- GUISelect:Render

function GUISelect:TestInRect( x, y, dx, dy )
  if dx == nil then
    dx = 0
    dy = 0
  end
  local result = false
  if x >= self.rect.left + dx and x <= self.rect.right + dx and
     y >= self.rect.top + dy and y <= self.rect.bottom + dy then
    result = true
    self.isHover = true
    self.hoverPos.x = x - ( self.rect.left + dx )
    self.hoverPos.y = y - ( self.rect.top + dy )
  else
    self.isHover = false
  end
  -- test drop list
  if self.state == 1 then
    self.valueHover = math.floor( ( y - ( self.rect.bottom + dy - self.rect.height ) ) / self.rect.height )
    if self.values[ self.valueHover ] ~= nil and x >= self.rect.left + dx and x <= self.rect.right + dx then  -- водим по выпавшему списку элементу
      result = true
      self.isHover = true
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
end -- GUISelect:TestInRect

function GUISelect:OnClick( id, isPressed )
  -- labelDebug:SetText( labelDebug:GetText()..( isPressed and 1 or 0 ) )
  if self.isHover then
    if isPressed == true then
      if GUIRenderer.focusedItem == self then
        if self.values[ self.valueHover ] == nil then
          GUIRenderer.focusedItem = nil
          self.state = 0
        else
          local oldValue = self.value
          self.value = self.valueHover
          GUIRenderer.focusedItem = nil
          self.state = 0
          if self.value ~= oldValue and self.OnChangeHandler ~= nil then
            self.OnChangeHandler( self )
          end
        end
      else
        GUIRenderer.focusedItem = self
        self.valueHover = 0
        if self.state == 0 then
          self.state = 1
        end
      end
    else
      if self.values[ self.valueHover ] ~= nil then
        local oldValue = self.value
        self.value = self.valueHover
        GUIRenderer.focusedItem = nil
        self.state = 0
        if self.value ~= oldValue and self.OnChangeHandler ~= nil then
          self.OnChangeHandler( self )
        end
      else
        if self.valueHover == 0 then  -- клик по самой плашке селектора => оставляем селектор открытым
        else
          GUIRenderer.focusedItem = nil
          self.state = 0
        end
      end
    end
  else  -- увели курсор вникуда => схлопываем и оставляем как есть
    GUIRenderer.focusedItem = nil
    self.state = 0
  end
  --[[
  if self.isHover then
    if isPressed then
      GUIRenderer.focusedItem = self
      if self.state == 0 then
        self.state = 1
      end
    else
      GUIRenderer.focusedItem = nil
      if self.state == 1 then
        self.state = 0
        if self.OnChangeHandler ~= nil then
          self.OnChangeHandler()
        end
      end
    end
  else
    GUIRenderer.focusedItem = nil
    self.state = 0
  end
  ]]
end -- GUISelect:OnClick

function GUISelect:OnKeyboard( id, isPressed )
  if isPressed then
    if id == 0x1B then  -- Escape
      GUIRenderer.focusedItem = nil
      self.state = 0
      return true
    elseif id == 0x28 then  -- Down
      if self.valueHover < #self.values then
        self.valueHover = self.valueHover + 1
      end
    elseif id == 0x26 then  -- Up
      if self.valueHover > 1 then
        self.valueHover = self.valueHover - 1
      end
    elseif id == 0x24 then  -- Home
      self.valueHover = 1
    elseif id == 0x23 then  -- End
      self.valueHover = #self.values
    elseif id == 0x21 then  -- Page Up
      self.valueHover = math.max( self.valueHover - 5, 1 )
    elseif id == 0x22 then  -- Page Down
      self.valueHover = math.min( self.valueHover + 5, #self.values )
    elseif id == 0x0d then  -- Enter
      if self.values[ self.valueHover ] ~= nil then
        local oldValue = self.value
        self.value = self.valueHover
        GUIRenderer.focusedItem = nil
        self.state = 0
        if self.value ~= oldValue and self.OnChangeHandler ~= nil then
          self.OnChangeHandler( self )
        end
      else
        GUIRenderer.focusedItem = nil
        self.state = 0
      end
    end
  end
  return false
end -- OnKeyboard
