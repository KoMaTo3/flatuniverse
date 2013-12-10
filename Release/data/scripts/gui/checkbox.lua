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


--[[ GUICheckbox:SetEnabled ]]
function GUICheckbox:SetEnabled( setIsEnabled )
  self.isEnabled = setIsEnabled
end -- GUICheckbox:SetEnabled


--[[ GUICheckbox:SetIsChecked ]]
function GUICheckbox:SetIsChecked( setIsChecked )
  self.checked = setIsChecked
end -- GUICheckbox:SetIsChecked


--[[ GUICheckbox:GetIsChecked ]]
function GUICheckbox:GetIsChecked()
  return self.checked
end -- GUICheckbox:GetIsChecked


function GUICheckbox:GetOffset()
  local dx, dy = 0, 0
  if self.parent ~= nil then
    dx, dy = self.parent:GetOffset()
  end
  return self.rect.left + dx, self.rect.top + dy
end



--[[ GUICheckbox:Create ]]
function GUICheckbox:Create( x0, y0, setText, setChecked, setOnChangeHandler, parent, setIsEnabled )
  local obj = {
    childs = {},
    parent = parent,
    rect = {
      left = x0,
      top = y0,
      right = x0 + 12,
      bottom = y0 + 12,
    },
    state = 0,  -- 0:free, 1:press, 2:hover, 3:press+out
    checked = setChecked,
    isHover = false,
    hoverPos = { x = 0, y = 0 },
    text = setText,
    isEnabled = setIsEnabled == nil and true or setIsEnabled,
    OnChange = setOnChangeHandler,
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
  Render(
    'sprite',
    self.rect.left + dx, self.rect.top + dy, 0,
    self.rect.right + dx, self.rect.bottom + dy, 0,
    self.checked and 'data/textures/gui/checkbox-checked.png' or 'data/textures/gui/checkbox.png',
    self.isEnabled and 'ffffffff' or 'ffffff44'
    )
  if self.state == 1 and self.isHover then
    Render( 'rect', self.rect.left + dx, self.rect.top + dy + 1, 0, self.rect.right + dx - 1, self.rect.bottom + dy, 0, '444466ff' )
  end
  local textColor = self.isEnabled and '000000ff' or '00000044'
  Render( 'text', self.rect.right + dx + 2, self.rect.top + dy, 0, self.text, textColor )
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
end -- GUICheckbox:TestInRect

function GUICheckbox:OnClick( id, isPressed )
  if not self.isEnabled then
    do return end
  end
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
        if self.OnChange ~= nil then
          self.OnChange( self )
        end
      end
    end
  else
    GUIRenderer.focusedItem = nil
    self.state = 0
  end
end -- GUICheckbox:OnClick
