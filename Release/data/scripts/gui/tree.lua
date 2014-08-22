GUITree = {}


--[[ GUITree:GetType ]]
function GUITree:GetType()
  return 'Tree'
end -- GUITree:GetType


--[[ GUITree:SetText ]]
function GUITree:SetText( setText )
  self.text = setText
end -- GUITree:SetText


--[[ GUITree:GetText ]]
function GUITree:GetText()
  return self.text
end -- GUITree:GetText


--[[ GUITree:SetEnabled ]]
function GUITree:SetEnabled( setIsEnabled )
  self.isEnabled = setIsEnabled
end -- GUITree:SetEnabled


function GUITree:GetOffset()
  local dx, dy = 0, 0
  if self.parent ~= nil then
    dx, dy = self.parent:GetOffset()
  end
  return self.rect.left + dx, self.rect.top + dy
end


--[[ GUITree:Create ]]
function GUITree:Create( x0, y0, setWidth, setHeight, setText, setElementsTable, parent, setOnRowMouseOverHandler, setOnRowClickHandler )
  local obj = {
    childs = {},
    parent = parent,
    rect = {
      left = x0,
      top = y0,
      right = x0 + setWidth,
      bottom = y0 + setHeight,
      height = setHeight,
    },
    scrollPercent = 0,
    scrollSize = 0,
    state = 0,  -- 0:free, 1:hover, 2:row-hover, 3:row-selected
    isHover = false,
    hoverPos = { x = 0, y = 0 },
    OnRowMouseOverHandler = setOnRowMouseOverHandler,
    OnRowClickHandler = setOnRowClickHandler,
    text = setText,
    colors = {
    },
    isEnabled = setIsEnabled == nil and true or setIsEnabled,
    elementsTable = setElementsTable == nil and {} or setElementsTable,
    lineHeight = 17,
  }
  self.__index = self
  local res = setmetatable( obj, self )
  if parent == nil then
    GUIRenderer.GUIElements[ #GUIRenderer.GUIElements + 1 ] = obj
  else
    parent.childs[ #parent.childs + 1 ] = obj
  end

  offset = 5
  for k,obj in pairs( setElementsTable ) do
    GUIButton:Create( 5, offset, 300, 16, '- '..obj.name, obj.OnClick == nil and nil or function( button ) obj.OnClick( button, obj ) end, res, true )
    offset = offset + res.lineHeight
  end
  res:RecalculateScroll()

  return res
end -- GUITree:Create


function GUITree:Render( dx, dy )
  if dx == nil then
    dx = 0
    dy = 0
  end
  local x0 = self.rect.left + dx
  local y0 = self.rect.top + dy
  local x1 = self.rect.right + dx
  local y1 = self.rect.bottom + dy

  Debug.Render( 'sprite', x0, y0, 0, x1, y1, 0, 'data/temp/blank.png', 'ffffffff' )
  -- childs
  Debug.Render( 'scissorEnable', x0, settings.windowSize.y - ( y1 - 2 ), x1 - 20, settings.windowSize.y - ( y0 - 2 ) )
  for key, item in pairs( self.childs ) do
    item:Render( dx + self.rect.left, dy + self.rect.top - self.scrollSize * self.scrollPercent / 100 )
  end
  Debug.Render( 'scissorDisable' )
end -- GUITree:Render


function GUITree:RecalculateScroll() --(
  local maxHeight = 0
  for k,obj in pairs( self.elementsTable ) do
    maxHeight = maxHeight + self.lineHeight
  end
  self.scrollSize = ( maxHeight > self.rect.height - 10 ) and ( maxHeight - ( self.rect.height - 10 ) ) or 0
end --) GUITree:RecalculateScroll


function GUITree:TestInRect( x, y, dx, dy )
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
  local num = 0
  for key, item in pairs( self.childs ) do
    if item:TestInRect( mousePos.x, mousePos.y, dx + self.rect.left, dy + self.rect.top ) then
      if GUIRenderer.activeItem == nil then
        GUIRenderer.activeItem = item
      end
      do return true end
    end
    num = num + 1
  end
  if result and GUIRenderer.activeItem == nil then
    GUIRenderer.activeItem = self
  end
  return result
end -- GUITree:TestInRect

function GUITree:OnClick( id, isPressed )
  if self.isHover then
    if isPressed then
      --[[
      GUIRenderer.focusedItem = self
      if self.state == 0 then
        self.state = 1
      end
      ]]
    else
      --[[
      GUIRenderer.focusedItem = nil
      if self.state == 1 then
        self.state = 0
        if self.OnClickHandler ~= nil then
          self.OnClickHandler()
        end
      end
      ]]
    end
  else
    GUIRenderer.focusedItem = nil
    self.state = 0
  end
end -- GUITree:OnClick
