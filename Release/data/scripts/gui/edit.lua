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
function GUIEdit:Create( x0, y0, width, setText, setOnChangeHandler, parent )
  local obj = {
    childs = {},
    rect = {
      left = x0,
      top = y0,
      right = x0 + width,
      bottom = y0 + 16,
    },
    isHover = false,
    OnChangeHandler = setOnChangeHandler,
    text = setText,
    oldText = setText,
    colors = {
      border = 'aaaaaaff',
      inner = 'eeeeeeff',
      cursor = '060606ff',
      selection = '0000ffff',
    },
    keyModificators = {
      shift = false,
      alt = false,
      ctrl = false,
    },
    cursorPos = #setText,
    cursorPosPixel = 0,
    state = 0, -- 0:free, 1:edit
    selection = 0,
    selectionWidth = 0,
    charTranslate = {
      [32]  = ' ',
      [186] = ';',
      [187] = '=',
      [188] = ',',
      [189] = '-',
      [190] = '.',
      [191] = '/',
      [192] = '`',
      [219] = '[',
      [221] = ']',
      [222] = '\'',
    },
    charTranslateShift = {
      [48] = ')',
      [49] = '!',
      [50] = '@',
      [51] = '#',
      [52] = '$',
      [53] = '%',
      [54] = '^',
      [55] = '&',
      [56] = '*',
      [57] = '(',
      [186] = ':',
      [187] = '+',
      [188] = '<',
      [189] = '_',
      [190] = '>',
      [191] = '?',
      [192] = '~',
      [219] = '{',
      [221] = '}',
      [222] = '"',
    },
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
  local cursorColor = ( math.floor( GetTime() * 2 ) % 2 == 1 and '00000000' or self.colors.cursor );
  Render( 'sprite', x0, y0, 0, x1, y1, 0, 'data/temp/blank.png', self.colors.inner )
  Render( 'rect', x0, y0, 0, x1, y1, 0, self.colors.border )
  if self.selection ~= 0 then
    Render( 'sprite', x0 + self.cursorPosPixel + 4, y0 + 1, 0, x0 + self.cursorPosPixel + 4 + ( self.selection > 0 and self.selectionWidth or -self.selectionWidth ), y1 - 1, 0, 'data/temp/blank.png', self.colors.selection )
  end
  Render( 'text', x0 + 4,y0, 0, self.text, '000000ff' )
  if self.state == 1 then -- cursor
    Render( 'line', x0 + self.cursorPosPixel + 4, y0 + 1, 0, x0 + self.cursorPosPixel + 4, y1 - 1, 0, cursorColor )
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
        self.oldText = self.text
      end
    end
  else
    if isPressed then
      GUIRenderer.focusedItem = nil
      self.state = 0
    end
  end
end -- GUIEdit:OnClick

function GUIEdit:OnKeyboard( id, isPressed )
  if id == 0x1B then  -- Escape
    GUIRenderer.focusedItem = nil
    self.state = 0
    do return true end
  end

  if id == 0x0D then  -- Enter
    GUIRenderer.focusedItem = nil
    self.state = 0
    if self.text ~= self.oldText and self.OnChangeHandler ~= nil then
      self.OnChangeHandler( self )
    end
    do return true end
  end

  if id == 0x10 then  -- Shift
    self.keyModificators.shift = isPressed
  elseif id == 0x11 then  -- Control
    self.keyModificators.ctrl = isPressed
  elseif id == 0x12 then  -- Alt
    self.keyModificators.alt = isPressed
  elseif isPressed then
    if id == 0x08 then  -- Backspace
      if self.cursorPos > 0 then
        self.text = self.text:sub( 1, self.cursorPos - 1 )..self.text:sub( self.cursorPos + 1 )
        self.cursorPos = self.cursorPos - 1
        self:UpdateCursor()
      end
    elseif id == 0x2E then  -- Delete
      if self.selection == 0 then
        self.text = self.text:sub( 1, self.cursorPos )..self.text:sub( self.cursorPos + 2 )
      else
        self.text = ( self.selection > 0 and self.text:sub( 1, self.cursorPos )..self.text:sub( self.cursorPos + self.selection + 1 ) or self.text:sub( 1, self.cursorPos + self.selection )..self.text:sub( self.cursorPos + 1 ) )
        if self.selection < 0 then
          self.cursorPos = self.cursorPos + self.selection
          self.selection = 0
          self:UpdateCursor()
        else
          self.selection = 0
          self:UpdateCursor()
        end
      end
    elseif id == 0x25 then  -- Left
      if self.cursorPos > 0 then
        self.cursorPos = self.cursorPos - 1
        if self.keyModificators.shift then
          self.selection = self.selection + 1
        else
          self.selection = 0
        end
        self:UpdateCursor()
      end
    elseif id == 0x27 then  -- Right
      if self.cursorPos < #self.text then
        self.cursorPos = self.cursorPos + 1
        if self.keyModificators.shift then
          self.selection = self.selection - 1
        else
          self.selection = 0
        end
        self:UpdateCursor()
      end
    elseif id == 0x24 then  -- Home
      if self.cursorPos > 0 then
        if self.keyModificators.shift then
          self.selection = self.cursorPos + self.selection
        else
          self.selection = 0
        end
        self.cursorPos = 0
      else
        self.selection = 0
      end
      self:UpdateCursor()
    elseif id == 0x23 then  -- End
      if self.cursorPos < #self.text then
        if self.keyModificators.shift then
          self.selection = self.cursorPos - #self.text + self.selection
        else
          self.selection = 0
        end
        self.cursorPos = #self.text
      else
        self.selection = 0
      end
      self:UpdateCursor()
    else  -- other keys
      -- self.text = self.text..string.format( "%c", id )
      local char = ''
      if id >= 65 and id <= 90 then -- A-Z
        char = string.format( "%c", id + ( self.keyModificators.shift and 0 or 32 ) )
      elseif id >= 48 and id <= 57 then -- A-Z
        if self.keyModificators.shift and self.charTranslateShift[ id ] ~= nil then
          char = self.charTranslateShift[ id ]
        else
          char = string.format( "%c", id )
        end
      else
        if self.keyModificators.shift and self.charTranslateShift[ id ] ~= nil then
          char = self.charTranslateShift[ id ]
        elseif self.charTranslate[ id ] ~= nil then
          char = self.charTranslate[ id ]
        else
          -- char = string.format( "%c", id )
          -- Alert(id)
        end
      end
      self.text = self.text:sub( 1, self.cursorPos )..char..self.text:sub( self.cursorPos + 1 )
      self.cursorPos = self.cursorPos + #char
      self:UpdateCursor()
    end
    do return true end
  end
  return false
end -- OnKeyboard

function GUIEdit:UpdateCursor()
  self.cursorPosPixel = Render( 'getTextWidth', self.text:sub( 1, self.cursorPos ) )
  -- Alert(self.selection)
  if self.selection ~= 0 then
    self.selectionWidth = self.selection > 0 and Render( 'getTextWidth', self.text:sub( self.cursorPos + 1, self.cursorPos + self.selection ) ) or Render( 'getTextWidth', self.text:sub( self.cursorPos + self.selection + 1, self.cursorPos ) )
  else
    self.selectionWidth = 0
  end
  -- Alert(self.selectionWidth)
end -- UpdateCursor
