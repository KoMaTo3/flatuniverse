GUIButton = {}
GUICheckbox = {}
GUIWindow = {}

GUIRenderer = {
  GUIElements = {},
  activeItem = nil,
  focusedItem = nil,
  OnClickDefault = nil,
  OnMouseMoveDefault = nil,

  RenderEnable = function()
    SetTimer( 0.1, 'GUIRendererRender' )
  end,
}



--
-- Button
--
--[[ GUIButton:Create ]]
function GUIButton:Create( x0, y0, x1, y1, setText, setOnClickHandler, parent )
  local obj = {
    childs = {},
    rect = {
      left = x0,
      top = y0,
      right = x1,
      bottom = y1,
    },
    state = 0,  -- 0:free, 1:press, 2:hover, 3:press+out
    color = setColor,
    isHover = false,
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
        dark = 'ddddddff',
      },
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
  Render( 'sprite', x0, y0, 0, x1, y1, 0, 'data/temp/blank.png', color.middle )
  Render( 'line', x0, y0, 0, x1, y0, 0, color.light )
  Render( 'line', x0, y0, 0, x0, y1, 0, color.light )
  Render( 'line', x0, y1, 0, x1, y1, 0, color.dark )
  Render( 'line', x1, y0, 0, x1, y1, 0, color.dark )
  Render( 'text', x0+2 + ( ( self.state == 1 and self.isHover ) and 1 or 0 ),y0+2 + ( ( self.state == 1 and self.isHover ) and 1 or 0 ),0, self.text, '000000ff' )
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
  else
    self.isHover = false
  end
  -- childs
  for key, item in pairs( self.childs ) do
    if item:TestInRect( mousePos.x, mousePos.y, dx + self.rect.left, dy + self.rect.top ) then
      GUIRenderer.activeItem = item
      do return true end
    end
  end
  if result then
    GUIRenderer.activeItem = self
    do return true end
  end
  return false
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



--
-- Checkbox
--
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
    GUIRenderer.activeItem = self
    self.isHover = true
    do return true end
  end
  self.isHover = false
  return false
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



--
-- Other
--
function GUIInit()
  ListenMouseKey( 'GUIMouseKey' )
  ListenMouseMove( 'GUIMouseMove' )
end

--[[ GUIRendererRender ]]
function GUIRendererRender()
  for id, item in pairs( GUIRenderer.GUIElements ) do
    --Alert( item.rect.right )
    item:Render()
  end
  --SetTimer( 0.1, 'GUIRendererRender' )
end -- GUIRendererRender

local but0 = GUIButton:Create( 20, 0, 67, 62, 'Butt0', function() Alert(0) end )
local but1 = GUIButton:Create( 110, 20, 155, 35, 'Butt1', function() Alert(1) end )
local cb0 = GUICheckbox:Create( 5, 15, 'cb0', false, but0 )
local cb1 = GUICheckbox:Create( 5, 30, 'cb1', false, but0 )
local cb2 = GUICheckbox:Create( 5, 45, 'cb2', true, but0 )

--[[ GUIMouseKey ]]
function GUIMouseKey( id, isPressed )
  if GUIRenderer.focusedItem ~= nil then --(
    GUIRenderer.focusedItem:OnClick( id, isPress )
  else --) (
    local inRect = false
    GUIRenderer.activeItem = nil
    for id, item in pairs( GUIRenderer.GUIElements ) do --(
      if item:TestInRect( mousePos.x, mousePos.y ) then --(
        inRect = true
        if GUIRenderer.activeItem ~= nil then
          GUIRenderer.activeItem:OnClick( id, isPressed )
          -- Alert( GUIRenderer.activeItem.rect.left )
        end
        do break end
      end --) if
    end --) for id,item
    if not inRect and GUIRenderer.OnClickDefault ~= nil then
      GUIRenderer.OnClickDefault( id, isPressed )
    end
  end --) ~GUIRenderer.focusedItem
end -- GUIMouseKey

--[[ GUIMouseMove ]]
function GUIMouseMove( x, y )
  mousePos.x = x
  mousePos.y = y
  local inRect = false
  for id, item in pairs( GUIRenderer.GUIElements ) do --(
    if item:TestInRect( x, y ) then --(
      inRect = true
      do break end
    end --) if
  end --) for id,item
  if not inRect and GUIRenderer.OnMouseMoveDefault ~= nil then
    GUIRenderer.OnMouseMoveDefault( x, y )
  end
end -- GUIMouseMove

GUIInit()
