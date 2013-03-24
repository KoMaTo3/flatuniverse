GUIEdit = {}


--[[ GUIEdit:GetType ]]
function GUIEdit:GetType()
  return 'Edit'
end -- GUIEdit:GetType


--[[ GUIEdit:SetText ]]
function GUIEdit:SetText( setText )
  self.text = setText
end -- GUIEdit:SetText


--[[ GUIEdit:GetText ]]
function GUIEdit:GetText()
  return self.text
end -- GUIEdit:GetText


--[[ GUIEdit:Create ]]
function GUIEdit:Create( x0, y0, width, setText, parent )
  local obj = {
    childs = {},
    rect = {
      left = x0,
      top = y0,
      right = x0 + width,
      bottom = y0 + 16,
    },
    isHover = false,
    text = setText,
    colors = {
      border = 'aaaaaaff',
      inner = 'eeeeeeff',
      cursor = '060606ff',
    },
    cursorPos = #setText,
    cursorPosPixel = 0,
    state = 0, -- 0:free, 1:edit
  }
  self.__index = self
  local res = setmetatable( obj, self )
  if parent == nil then
    GUIRenderer.GUIElements[ #GUIRenderer.GUIElements + 1 ] = obj
  else
    parent.childs[ #parent.childs + 1 ] = obj
  end
  res.cursorPosPixel = Render( 'getTextWidth', setText )
  return res
end -- GUIEdit:Create

function GUIEdit:Render( dx, dy )
  if dx == nil then
    dx = 0
    dy = 0
  end
  local x0 = self.rect.left + dx
  local y0 = self.rect.top + dy
  local x1 = self.rect.right + dx
  local y1 = self.rect.bottom + dy
  Render( 'sprite', x0, y0, 0, x1, y1, 0, 'data/temp/blank.png', self.colors.inner )
  Render( 'rect', x0, y0, 0, x1, y1, 0, self.colors.border )
  Render( 'text', x0 + 4,y0, 0, self.text, '000000ff' )
  if self.state == 1 then -- cursor
    Render( 'line', x0 + self.cursorPosPixel + 4, y0 + 1, 0, x0 + self.cursorPosPixel + 4, y1 - 1, 0, self.colors.cursor )
  end
  -- childs
  for key, item in pairs( self.childs ) do
    item:Render( dx + self.rect.left, dy + self.rect.top )
  end
end -- GUIEdit:Render

function GUIEdit:TestInRect( x, y, dx, dy )
  if dx == nil then
    dx = 0
    dy = 0
  end
  local result = false
  if x >= self.rect.left + dx and x <= self.rect.right + dx and
     y >= self.rect.top + dy and y <= self.rect.bottom + dy then
    result = true
    self.isHover = true
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
  if result then
    GUIRenderer.activeItem = self
    do return true end
  end
  return false
end -- GUIEdit:TestInRect

function GUIEdit:OnClick( id, isPressed )
  if self.isHover then
    if isPressed then
      GUIRenderer.focusedItem = self
      if self.state == 0 then
        self.state = 1
      end
    end
  else
    if isPressed then
      GUIRenderer.focusedItem = nil
      self.state = 0
    end
  end
end -- GUIEdit:OnClick
