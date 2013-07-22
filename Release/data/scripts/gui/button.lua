GUIButton = {}


--[[ GUIButton:GetType ]]
function GUIButton:GetType()
  return 'Button'
end -- GUIButton:GetType


--[[ GUIButton:SetText ]]
function GUIButton:SetText( setText )
  self.text = setText
end -- GUIButton:SetText


--[[ GUIButton:GetText ]]
function GUIButton:GetText()
  return self.text
end -- GUIButton:GetText


--[[ GUIButton:SetEnabled ]]
function GUIButton:SetEnabled( setIsEnabled )
  self.isEnabled = setIsEnabled
end -- GUIButton:SetEnabled


--[[ GUIButton:Create ]]
function GUIButton:Create( x0, y0, width, height, setText, setOnClickHandler, parent, setIsEnabled )
  local obj = {
    childs = {},
    rect = {
      left = x0,
      top = y0,
      right = x0 + width,
      bottom = y0 + height,
    },
    state = 0,  -- 0:free, 1:press, 2:hover, 3:press+out
    isHover = false,
    hoverPos = { x = 0, y = 0 },
    OnClickHandler = setOnClickHandler,
    text = setText,
    colors = {
      [0] = {
        light = 'ffffffff',
        middle = 'ddddddff',
        dark = '888888ff',
      },
      [1] = {
        light = '888888ff',
        middle = 'ccccccff',
        dark = 'eeeeeeff',
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
end -- GUIButton:Create

function GUIButton:Render( dx, dy )
  if dx == nil then
    dx = 0
    dy = 0
  end
  local x0 = self.rect.left + dx
  local y0 = self.rect.top + dy
  local x1 = self.rect.right + dx
  local y1 = self.rect.bottom + dy
  local isPushed = ( self.state == 1 and self.isHover ) and true or false
  local color = self.colors[ isPushed and 1 or 0 ]
  if isPushed then
    dx = dx + 1
    dy = dy + 1
  end
  local offset = ( self.state == 1 and self.isHover ) and 1 or 0
  Render( 'sprite', x0 + offset, y0 + offset, 0, x1 + offset, y1 + offset, 0, 'data/temp/blank.png', color.middle )
  Render( 'line', x0 + offset, y0 + offset, 0, x1 + offset, y0 + offset, 0, color.light )
  Render( 'line', x0 + offset, y0 + offset, 0, x0 + offset, y1 + offset, 0, color.light )
  Render( 'line', x0 + offset, y1 + offset, 0, x1 + offset, y1 + offset, 0, color.dark )
  Render( 'line', x1 + offset, y0 + offset, 0, x1 + offset, y1 + offset, 0, color.dark )
  Render( 'text', x0 + 2 + offset,y0 + 2 + offset, 0, self.text, '000000ff' )
  -- childs
  for key, item in pairs( self.childs ) do
    item:Render( dx + self.rect.left, dy + self.rect.top )
  end
end -- GUIButton:Render

function GUIButton:TestInRect( x, y, dx, dy )
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
end -- GUIButton:TestInRect

function GUIButton:OnClick( id, isPressed )
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
        if self.OnClickHandler ~= nil then
          self.OnClickHandler()
        end
      end
    end
  else
    GUIRenderer.focusedItem = nil
    self.state = 0
  end
end -- GUIButton:OnClick
