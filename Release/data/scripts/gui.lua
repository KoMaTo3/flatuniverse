GUIButton = {}
GUICheckbox = {}
GUIWindow = {}

GUIRenderer = {
  GUIElements = {},
  activeItem = nil,

  RenderEnable = function()
    SetTimer( 0.1, 'GUIRendererRender' )
  end,
}



--
-- Button
--
--[[ GUIButton:Create ]]
function GUIButton:Create( x0, y0, x1, y1, setColor, parent )
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
    colors = {
      [0] = {
        light = 'ffffffff',
        middle = 'ddddddff',
        dark = '888888ff',
      },
      [1] = {
        light = '888888ff',
        middle = 'aaaaaaff',
        dark = 'ffffffff',
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
  Render( 'sprite', x0, y0 + 1, 0, x1, y1, 0, 'data/temp/blank.png', self.colors[ self.state ].middle )
  Render( 'line', x0, y0, 0, x1, y0, 0, self.colors[ self.state ].light )
  Render( 'line', x0, y0, 0, x0, y1, 0, self.colors[ self.state ].light )
  Render( 'line', x0, y1, 0, x1, y1, 0, self.colors[ self.state ].dark )
  Render( 'line', x1, y0, 0, x1, y1, 0, self.colors[ self.state ].dark )
  -- Render( 'sprite', self.rect.left + dx, self.rect.top + dy, 0, self.rect.right + dx, self.rect.bottom + dy, 0, 'data/temp/blank.png', self.color )
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
  if isPressed then
    if self.state == 0 then
      self.state = 1
    end
  else
    if self.state == 1 then
      self.state = 0
    end
    -- при отпускании 
    for id, item in pairs( GUIRenderer.GUIElements ) do
      item:Reset()
    end
  end
end -- GUIButton:OnClick

function GUIButton:Reset()
  self.state = 0
  for key, item in pairs( self.childs ) do
    item:Reset()
  end
end -- GUIButton:Reset



--
-- Checkbox
--
--[[ GUICheckbox:Create ]]
function GUICheckbox:Create( x0, y0, setChecked, parent )
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
    do return true end
  end
  return false
end -- GUICheckbox:TestInRect

function GUICheckbox:OnClick( id, isPressed )
  if isPressed then
    if self.state == 0 then
      self.state = 1
    end
  else
    if self.state == 1 then
      self.state = 0
      self.checked = not self.checked
      for id, item in pairs( GUIRenderer.GUIElements ) do
        item:Reset()
      end
    end
  end
end -- GUICheckbox:OnClick

function GUICheckbox:Reset()
  self.state = 0
  for key, item in pairs( self.childs ) do
    item:Reset()
  end
end -- GUICheckbox:Reset



--
-- Other
--
function GUIInit()
  ListenMouseKey( 'GUIMouseKey' )
end

--[[ GUIRendererRender ]]
function GUIRendererRender()
  for id, item in pairs( GUIRenderer.GUIElements ) do
    --Alert( item.rect.right )
    item:Render()
  end
  --SetTimer( 0.1, 'GUIRendererRender' )
end -- GUIRendererRender

local but0 = GUIButton:Create( 20, 0, 70, 30, '444488ff' )
-- local but1 = GUIButton:Create( 5, 5, 65, 35, '884444ff', but0 )
local cb0 = GUICheckbox:Create( 5, 5, false, but0 )
local cb1 = GUICheckbox:Create( 20, 5, false, but0 )
local cb2 = GUICheckbox:Create( 35, 5, true, but0 )
-- GUIRenderer.RenderEnable()

--[[ GUIMouseKey ]]
function GUIMouseKey( id, isPressed )
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
end -- GUIMouseKey

GUIInit()
