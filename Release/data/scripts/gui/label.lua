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


--[[ GUILabel:SetPosition ]]
function GUILabel:SetPosition( x, y )
  local width = self.rect.right - self.rect.left
  local height = self.rect.bottom - self.rect.top
  self.rect.left = x
  self.rect.top = y
  self.rect.right = x + width
  self.rect.bottom = y + height
end -- GUILabel:SetPosition


--[[ GUILabel:CropByTextWidth ]]
function GUILabel:CropByTextWidth()
  local width = Render( 'getTextWidth', self.text )
  self.rect.right = self.rect.left + width + 8
end -- GUILabel:CropByTextWidth


function GUILabel:GetOffset()
  local dx, dy = 0, 0
  if self.parent ~= nil then
    dx, dy = self.parent:GetOffset()
  end
  return self.rect.left + dx, self.rect.top + dy
end


--[[ GUILabel:Create ]]
function GUILabel:Create( x0, y0, width, height, setText, parent, setOnMouseMove, zIndex )
  local obj = {
    childs = {},
    parent = parent,
    rect = {
      left = x0,
      top = y0,
      right = x0 + width,
      bottom = y0 + height,
    },
    z = ( zIndex == nil and 0 or zIndex ),
    isHover = false,
    hoverPos = { x = 0, y = 0 },
    text = setText,
    colors = {
      border = 'aaaaaaff',
      inner = 'eeeeeeff',
    },
    OnMouseMove = setOnMouseMove,
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
  Render( 'sprite', x0, y0, self.z, x1, y1, self.z, 'data/temp/blank.png', self.colors.inner )
  Render( 'rect', x0, y0, self.z, x1, y1, self.z, self.colors.border )
  Render( 'text', x0 + 4,y0, self.z, self.text, '000000ff' )
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
  if result then
    GUIRenderer.activeItem = self
    if self.OnMouseMove ~= nil then
      self.OnMouseMove( self )
    end
    do return true end
  end
  return false
end -- GUILabel:TestInRect
