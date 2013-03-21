GUILabel = {}


--[[ GUILabel:GetType ]]
function GUILabel:GetType()
  return 'Label'
end -- GUILabel:GetType


--[[ GUILabel:SetText ]]
function GUILabel:SetText( setText )
  self.text = setText
end -- GUILabel:SetText


--[[ GUILabel:GetText ]]
function GUILabel:GetText()
  return self.text
end -- GUILabel:GetText


--[[ GUILabel:Create ]]
function GUILabel:Create( x0, y0, width, height, setText, parent )
  local obj = {
    childs = {},
    rect = {
      left = x0,
      top = y0,
      right = x0 + width,
      bottom = y0 + height,
    },
    isHover = false,
    text = setText,
    colors = {
      border = 'aaaaaaff',
      inner = 'eeeeeeff',
    },
  }
  self.__index = self
  local res = setmetatable( obj, self )
  if parent == nil then
    GUIRenderer.GUIElements[ #GUIRenderer.GUIElements + 1 ] = obj
  else
    parent.childs[ #parent.childs + 1 ] = obj
  end
  return res
end -- GUILabel:Create

function GUILabel:Render( dx, dy )
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
  Render( 'text', x0 + 2,y0 + 2, 0, self.text, '000000ff' )
  -- childs
  for key, item in pairs( self.childs ) do
    item:Render( dx + self.rect.left, dy + self.rect.top )
  end
end -- GUILabel:Render

function GUILabel:TestInRect( x, y, dx, dy )
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
end -- GUILabel:TestInRect
