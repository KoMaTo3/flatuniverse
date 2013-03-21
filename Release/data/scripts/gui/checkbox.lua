GUICheckbox = {}


--[[ GUICheckbox:GetType ]]
function GUICheckbox:GetType()
  return 'Checkbox'
end -- GUICheckbox:GetType


--[[ GUICheckbox:SetText ]]
function GUICheckbox:SetText( setText )
  self.text = setText
end -- GUICheckbox:SetText


--[[ GUICheckbox:GetText ]]
function GUICheckbox:GetText()
  return self.text
end -- GUICheckbox:GetText



--[[ GUICheckbox:Create ]]
function GUICheckbox:Create( x0, y0, setText, setChecked, parent )
  local obj = {
    childs = {},
    rect = {
      left = x0,
      top = y0,
      right = x0 + 12,
      bottom = y0 + 12,
    },
    state = 0,  -- 0:free, 1:press, 2:hover, 3:press+out
    color = setColor,
    checked = setChecked,
    isHover = false,
    text = setText,
  }
  self.__index = self
  local res = setmetatable( obj, self )
  if parent == nil then
    GUIRenderer.GUIElements[ #GUIRenderer.GUIElements + 1 ] = obj
  else
    parent.childs[ #parent.childs + 1 ] = obj
  end
  return res
end -- GUICheckbox:Create

function GUICheckbox:Render( dx, dy )
  if dx == nil then
    dx = 0
    dy = 0
  end
  Render( 'sprite', self.rect.left + dx, self.rect.top + dy, 0, self.rect.right + dx, self.rect.bottom + dy, 0, self.checked and 'data/textures/gui/checkbox-checked.png' or 'data/textures/gui/checkbox.png', 'ffffffff' )
  if self.state == 1 and self.isHover then
    Render( 'rect', self.rect.left + dx + 1, self.rect.top + dy, 0, self.rect.right + dx, self.rect.bottom + dy, 0, '444466ff' )
  end
  Render( 'text', self.rect.right + dx + 2, self.rect.top + dy, 0, self.text, '000000ff' )
end -- GUICheckbox:Render

function GUICheckbox:TestInRect( x, y, dx, dy )
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
  if result and GUIRenderer.activeItem == nil then
    GUIRenderer.activeItem = self
  end
  return result
end -- GUICheckbox:TestInRect

function GUICheckbox:OnClick( id, isPressed )
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
        self.checked = not self.checked
      end
    end
  else
    GUIRenderer.focusedItem = nil
    self.state = 0
  end
end -- GUICheckbox:OnClick
