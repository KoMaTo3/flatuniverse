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
}
mousePos = {    -- экранная позиция курсора
    x = 0,
    y = 0,
}

-- Инициализация
function Init()
  math.randomseed( GetRandomSeed() )

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

  -- показываем GUI
  settings.guiVisibility = true
  SetGuiVisibility( settings.guiVisibility )
  -- SetTimer( 0.01, 'UpdateDebug' )

  ObjectAttr( 'wall', { trigger = true, triggerSize = 26 } )
  ObjectAddTrigger( 'wall', 'testFun1' );

  -- ObjectAttr( 'wall', { 'textureName' } )
end
Init()

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
            GuiSetText( 'editor/object.object_name', object )
            SelectObject( object )
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
          GuiSetText( 'editor/object.object_name', object )
          SelectObject( object )
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
          GuiSetText( 'editor/object.object_name', object )
          SelectObject( object )
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
