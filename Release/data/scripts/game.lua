--[[ editorMode
-- 0: none, default
-- 1: нажал ЛКМ и смотрим, будет ли двигать мышью
-- 2: выбран объект
-- 3: перемещение выбранного объекта
]]
settings = {
    guiVisibility     = false,  -- отображение GUI
    objectMode3Moved  = false,  -- bool если происходим перемещение объекта мышкой
    editorMode        = 0,      -- текущий режим редактора
    move              = {
      objectStart = {
        x = 0,
        y = 0,
      },
      mouseStart = {
        x = 0,
        y = 0,
      },
    },
    timer = 0.0,
    windowSize = { x = 0, y = 0 },
}
mousePos = {    -- экранная позиция курсора
    x = 0,
    y = 0,
}

-- Инициализация
function Init()
  math.randomseed( GetRandomSeed() )
  local x,y = GetWindowSize()
  settings.windowSize.x = x
  settings.windowSize.y = y

  -- Ставим обработчики на всё: клаву, кнопки и движение мыши
  ListenKeyboard( 'onKey' )
  ListenMouseKey( 'onMouseKey' )
  ListenMouseMove( 'onMouseMove' )

  -- Ставим обработчики чекбоксов и кнопок редактора
  GuiAddTrigger( 'editor/settings.layer', 'ToggleLayer' )
  GuiAddTrigger( 'editor/object.is_renderable', 'ToggleRenderable' )
  GuiAddTrigger( 'editor/object.is_collision', 'ToggleCollision' )
  GuiAddTrigger( 'editor/object.is_trigger', 'ToggleTrigger' )
  GuiAddTrigger( 'editor/object.is_static', 'ToggleStatic' )
  GuiAddTrigger( 'editor/object.tile_apply', 'TileApply' )
  GuiAddTrigger( 'editor/renderable.width', 'ToggleRenderableSize' )
  GuiAddTrigger( 'editor/renderable.height', 'ToggleRenderableSize' )

  -- показываем GUI
  settings.guiVisibility = true
  SetGuiVisibility( settings.guiVisibility )
  -- SetTimer( 0.01, 'UpdateDebug' )

  --[[
  ObjectAttr( 'wall', { trigger = true, triggerSize = 26 } )
  ObjectAddTrigger( 'wall', 'testFun1' );
  ]]
  UpdateGuiBySelectedObject()

  -- create temp object
  --[[
  local cameraX, cameraY = GetCameraPos()
  local name = 'wall-new'
  local tileSize = GetTileSize()
  ObjectCreate( name, cameraX, cameraY, -1 )
  ObjectAttr( name, { renderable = true, textureName = 'temp/enemy0.png', renderableSize = tileSize..' '..tileSize } )
  ObjectAttr( name, { collision = true, collisionSize = tileSize..' '..tileSize, collisionStatic = isStatic } )
  ObjectAttr( name, { trigger = true, triggerSize = ( tileSize + 2 )..' '..( tileSize + 2 ) } )
  --]]

  -- ObjectAttr( 'wall', { 'textureName' } )
  -- ObjectAttr( 'wall', { 'textureName' } )
  -- local r,g,b,a = ObjectAttr( 'wall', { 'color' } )
  -- Alert( '', r..':'..g..':'..b..':'..a )

  RenderGUI()
end

function testFun1( obj )
  settings.guiVisibility = not settings.guiVisibility
  SetGuiVisibility( settings.guiVisibility )
end

-- Обработка кнопок клавы
function onKey( id, isPressed )
  if isPressed then
      if id == 0x1B then    -- Esc
        GameExit()
      end
      if id == 0x5A then    -- Z
        settings.guiVisibility = not settings.guiVisibility
        SetGuiVisibility( settings.guiVisibility )
      end
      if id == 0xC0 then    -- ~
        GuiSetText( 'editor/settings.layer', 'default' )
        DebugRender( 0 )
      end
      if id == 0x31 then    -- 1
        GuiSetText( 'editor/settings.layer', 'renderable' )
        DebugRender( 1 )
      end
      if id == 0x32 then    -- 2
        GuiSetText( 'editor/settings.layer', 'collision' )
        DebugRender( 2 )
      end
      if id == 0x33 then    -- 3
        GuiSetText( 'editor/settings.layer', 'trigger' )
        DebugRender( 4 )
      end
      if id == 0x2E then    -- Del
        local object = GuiGetText( 'editor/object.object_name' )
        if #object > 0 then
          ObjectRemove( object )
        end
      end
      if id == 0x20 then    -- Space
        SelectObject( '' )
        UpdateGuiBySelectedObject()
      end
  end
  if id == 0x4C then
    LoadScript( 'data/scripts/clrscr.lua' )
  end
end

-- Обработка кнопок мыши
function onMouseKey( id, isPressed )
  local mode = {
    [0] = function()
      if isPressed then
        local object = GetObjectUnderCursorByMode()
        if #object > 0 then
            SelectObject( object )
            UpdateGuiBySelectedObject()
            settings.editorMode = 1
        else
            -- multiselect?
        end
      else
      end
    end,
    [1] = function()
      if isPressed then
      else
        local object = GetSelectedObject()
        if #object > 0 then
          settings.editorMode = 2
        else
          settings.editorMode = 0
      end
      end
    end,
    [2] = function()
      if isPressed then
        settings.editorMode = 3
        settings.objectMode3Moved = false
      else
        local object = GetObjectUnderCursorByMode()
        if #object > 0 then
          SelectObject( object )
          UpdateGuiBySelectedObject()
        end
      end
    end,
    [3] = function()
      if isPressed then
      else
        if settings.objectMode3Moved then
          settings.editorMode = 2
        else
          local object = GetObjectUnderCursorByMode()
          SelectObject( object )
          UpdateGuiBySelectedObject()
          if #object > 0 then
            settings.editorMode = 2
          else
            settings.editorMode = 0
          end
        end
      end
    end,
  }
  mode[ settings.editorMode ]()
end

-- Обработка движения мыши
function onMouseMove( x, y )
  local mode = {
    [0] = function()
    end,
    [1] = function()
      settings.editorMode = 3
    end,
    [2] = function()
    end,
    [3] = function()
      local object = GetSelectedObject()
      local oldX, oldY = ObjectGetPos( object )
      if not settings.objectMode3Moved then
        settings.move.objectStart.x = oldX
        settings.move.objectStart.y = oldY
        settings.move.mouseStart.x = mousePos.x
        settings.move.mouseStart.y = mousePos.y
      end
      local newX = settings.move.objectStart.x + x - settings.move.mouseStart.x
      local newY = settings.move.objectStart.y + y - settings.move.mouseStart.y
      local tileSize = GetTileSize()
      newX = math.floor( newX / tileSize ) * tileSize + 0.5 * tileSize
      newY = math.floor( newY / tileSize ) * tileSize + 0.5 * tileSize
      ObjectSetPos( object, newX, newY )
      settings.objectMode3Moved = true
    end,
  }
  mode[ settings.editorMode ]()
  mousePos.x = x
  mousePos.y = y
end

-- Возвращает объект под курсором
-- Если уже выбран какой-то объект, то берётся следующий за ним
function GetObjectUnderCursorByMode()
    local mx, my = GetMousePos()
    local cx, cy = GetCameraPos()
    local width, height = GetWindowSize()
    local pos = {
      x = cx + mx - width * 0.5,
      y = cy + my - height * 0.5
    }
    local layer = GuiGetText( 'editor/settings.layer' )
    local name = GetSelectedObject()
    if layer == 'renderable' then
      name = GetObjectByPoint( 1, pos.x, pos.y, name )
    end
    if layer == 'collision' then
      name = GetObjectByPoint( 2, pos.x, pos.y, name )
    end
    if layer == 'trigger' then
      name = GetObjectByPoint( 3, pos.x, pos.y, name )
    end
    return name
end --GetObjectUnderCursorByMode

-- Чекбокс "Renderable"
function ToggleRenderable( guiName )
  local name = GetSelectedObject()
  if #name < 1 then
    return
  end
  local checked = GuiGetChecked( 'editor/object.is_renderable' )
  if checked then
    local tileSize = GetTileSize()
    local texture = GuiGetText( 'editor/renderable.texture_name' )
    ObjectAttr( name, { renderable = true, textureName = texture, renderableSize = tileSize..' '..tileSize } )
  else
    ObjectAttr( name, { renderable = false } )
  end
end

-- Чекбокс "Collision"
function ToggleCollision( guiName )
  local name = GetSelectedObject()
  if #name < 1 then
    return
  end
  local checked = GuiGetChecked( 'editor/object.is_collision' )
  local isStatic = GuiGetChecked( 'editor/object.is_static' )
  if checked then
    local tileSize = GetTileSize()
    ObjectAttr( name, { collision = true, collisionSize = tileSize..' '..tileSize, collisionStatic = isStatic } )
  else
    ObjectAttr( name, { collision = false } )
  end
end

-- Чекбокс "Collision:Static"
function ToggleStatic( guiName )
  local name = GetSelectedObject()
  if #name < 1 then
    return
  end
  local checked = GuiGetChecked( 'editor/object.is_static' )
  ObjectAttr( name, { collisionStatic = checked } )
end

-- Чекбокс "Trigger"
function ToggleTrigger( guiName )
  local name = GetSelectedObject()
  if #name < 1 then
    return
  end
  local checked = GuiGetChecked( 'editor/object.is_trigger' )
  if checked then
    local tileSize = GetTileSize()
    ObjectAttr( name, { trigger = true, triggerSize = tileSize..' '..tileSize } )
  else
    ObjectAttr( name, { trigger = false } )
  end
end

-- Возвращает текущий размер тайлов (из дропбокса)
function GetTileSize()
  return GuiGetText( 'editor/world.tile_size' )
end

-- Кнопка "Apply"
-- TODO: переделать
function TileApply( guiName )
  local name = GuiGetText( 'editor/object.object_name' )
  local cameraX, cameraY = GetCameraPos()
  ObjectCreate( name, cameraX, cameraY, 0 )
  SelectObject( name )
  ToggleRenderable( 'editor/object.is_renderable' )
  ToggleCollision( 'editor/object.is_collision' )
  ToggleTrigger( 'editor/object.is_trigger' )
  UpdateGuiBySelectedObject()
  settings.editorMode = 2
end

-- Дропбокс "Layer"
function ToggleLayer( guiName )
  local layer = GuiGetText( 'editor/settings.layer' )
  local flags = 0
  if layer == 'renderable' then
    flags = 1
  end
  if layer == 'collision' then
    flags = 2
  end
  if layer == 'trigger' then
    flags = 4
  end
  DebugRender( flags )
end

function UpdateDebug()
  GuiSetText( 'editor/debug', settings.editorMode..':'..( settings.objectMode3Moved and 'true' or 'false' ) )
  SetTimer( 0.01, 'UpdateDebug' )
end

function UpdateGuiBySelectedObject()
  local object = GetSelectedObject()
  elements = {
    'editor/object.object_name',
    'editor/object.is_renderable',
    'editor/renderable.texture_name',
    'editor/object.is_collision',
    'editor/object.is_static',
    'editor/object.is_trigger',
    'editor/renderable.width',
    'editor/renderable.height',
  }
  if #object < 1 then
    for i=1,#elements do
      GuiAttr( elements[ i ], 'enabled', false )
      GuiAttr( elements[ i ], 'checked', false )
    end
  else
    local isRenderable
        , isCollision
        , isTrigger
        , textureName
        , renderableSizeX
        , renderableSizeY
        = ObjectAttr( object, {
          'renderable',
          'collision',
          'trigger',
          'textureName',
          'renderableSize',
        } )
    GuiAttr( 'editor/object.is_renderable', 'enabled', true )
    GuiAttr( 'editor/object.is_collision', 'enabled', true )
    GuiAttr( 'editor/object.is_trigger', 'enabled', true )
    GuiAttr( 'editor/renderable.width', 'enabled', isRenderable )
    GuiAttr( 'editor/renderable.height', 'enabled', isRenderable )
    GuiAttr( 'editor/renderable.texture_name', 'enabled', isRenderable )
    GuiAttr( 'editor/object.is_renderable', 'checked', isRenderable )
    GuiAttr( 'editor/object.is_collision', 'checked', isCollision )
    GuiAttr( 'editor/object.is_trigger', 'checked', isTrigger )
    GuiSetText( 'editor/renderable.texture_name', textureName )
    GuiSetText( 'editor/renderable.width', renderableSizeX )
    GuiSetText( 'editor/renderable.height', renderableSizeY )
  end
  GuiSetText( 'editor/object.object_name', object )
end

function ToggleRenderableSize()
  local object = GetSelectedObject()
  if #object < 1 then
  else
    local width   = GuiGetText( 'editor/renderable.width' )
    local height  = GuiGetText( 'editor/renderable.height' )
    ObjectAttr( object, { renderableSize = width..' '..height } )
  end
end

function RenderGUI()
  settings.timer = settings.timer + 0.01
  local a = -settings.timer * 6.0 - 0.1
  local b = -settings.timer * 6.0 + 0.1
  local x = 25
  local y = 25
  local r = 50
  Render( 'clrscr' )
  Render( 'line', x,y,0, x + math.sin( a ) * r,y + math.cos( a ) * r,0, 1,0,0,1 )
  Render( 'line', x,y,0, x + math.sin( b ) * r,y + math.cos( b ) * r,0, 0,1,0,1 )
  Render( 'line', x + math.sin( a ) * r,y + math.cos( a ) * r,0, x + math.sin( b ) * r,y + math.cos( b ) * r,0, 0,0,1,1 )
  Render( 'sprite', 0,0,0, 50 + math.random( 0, 20 ),50 + math.random( 0, 20 ),0, 'data/temp/BLUE_STARA.png' )
  SetTimer( 0.01, 'RenderGUI' )
end

Init()

