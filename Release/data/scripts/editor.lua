--[[

Визуальный редактор

settings.editorMode:
-- 0: none, default
-- 1: нажал ЛКМ и смотрим, будет ли двигать мышью
-- 2: выбран объект
-- 3: перемещение выбранного объекта
-- 10: mouseover на GUI.templates
-- 11: перемещение GUI.templates
-- 12: скролл GUI.templates
-- 20: промежуточный режим вставки тайла: нажал мышь и ждём отпускания или движения
-- 21: вставка блока тайлов: рисование прямоугольной области
]]
settings = {
    guiVisibility     = false,  -- отображение GUI

    objectMode3Moved  = false,  -- bool если происходит перемещение объекта мышкой
    objectMode3StartPosition = { 0, 0 },  -- начальное положение обекта, до перемещения

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
    showGrid = false,
    editorType = 0, -- 0 - общий режим, 1 - рендерейблы, 2 - коллизии, 3 - триггеры
    keys = {
      isShift = false,
      isCtrl  = false,
      isAlt   = false,
    },
    buffer = {},  -- буфер действий для отмены
}
mousePos = {    -- экранная позиция курсора
    x = 0,
    y = 0,
}
GUI = {
  DOM = {
    x = 0,
    y = 0,
    width = 100,
    height = 100,
  },
  templates = {
    x = 5,
    y = 5,
    width = 90,
    height = 0,
    moving = {
      x = 0,
      y = 0,
    },
    scroll = 0,
    maxScroll = 350,
    itemSize = 32,
    items = {},
    currentItem = 0,
    Draw = function()
      GUI.templates.maxScroll = ( #GUI.templates.items * ( GUI.templates.itemSize + 5 ) + 10 ) - ( GUI.templates.height - 15 )
      if GUI.templates.maxScroll < 1 then
        GUI.templates.maxScroll = 1
      end
      local y
      local i
      -- bg
      Render( 'sprite', GUI.templates.x, GUI.templates.y, 0, GUI.templates.x + GUI.templates.width, GUI.templates.y + GUI.templates.height, 0, 'data/temp/blank.png', '333333FF' )
      -- scroll bg
      Render( 'sprite', GUI.templates.x + GUI.templates.width - 15, GUI.templates.y, 0, GUI.templates.x + GUI.templates.width, GUI.templates.y + GUI.templates.height, 0, 'data/temp/blank.png', '555555FF' )
      -- scroll bar
      y = GUI.templates.y + 15 + GUI.templates.scroll*(GUI.templates.height-29)
      Render( 'sprite', GUI.templates.x + GUI.templates.width - 14, y + 1, 0, GUI.templates.x + GUI.templates.width - 1, y + 13, 0, 'data/temp/blank.png', '888888FF' )
      -- header
      Render( 'sprite', GUI.templates.x, GUI.templates.y, 0, GUI.templates.x + GUI.templates.width, GUI.templates.y + 15, 0, 'data/temp/blank.png', '999999FF' )
      -- items
      -- Render( 'scissorEnable', GUI.templates.x, settings.windowSize.y - ( GUI.templates.y + GUI.templates.height - 0 ), GUI.templates.x + GUI.templates.width - 15, settings.windowSize.y - 16 )
      Render( 'scissorEnable', GUI.templates.x + 1, settings.windowSize.y - ( GUI.templates.y + GUI.templates.height - 1 ), GUI.templates.x + GUI.templates.width - 17, settings.windowSize.y - ( GUI.templates.y + 17 ) )
      i = 0
      -- for key,item in pairs( GUI.templates.items ) do
      table.foreach( GUI.templates.items, GUI.templates.DrawItem )
      if GUI.templates.currentItem > 0 and GUI.templates.currentItem <= #GUI.templates.items then
        y = GUI.templates.y - GUI.templates.scroll * GUI.templates.maxScroll + ( GUI.templates.currentItem - 1 ) * ( GUI.templates.itemSize + 5 ) + 20
        Render( 'rect', GUI.templates.x + 5, y, 0, GUI.templates.x + 5 + GUI.templates.itemSize, y + GUI.templates.itemSize, 0, '00ff00ff' )
      end
      Render( 'scissorDisable' )
    end,
    DrawItem = function( i, item )
      y = GUI.templates.y - GUI.templates.scroll * GUI.templates.maxScroll + ( i - 1 ) * ( GUI.templates.itemSize + 5 ) + 20
      Render( 'sprite', GUI.templates.x + 5, y, 0, GUI.templates.x + 5 + GUI.templates.itemSize, y + GUI.templates.itemSize, 0, item.icon, 'ffffffff' )
    end,
  }, -- templates
  grid = {
    Render = function()
      local tileSize = GetTileSize()
      local cameraX, cameraY = GetCameraPos()
      local sx, sy
      sx = ( math.floor( settings.windowSize.x * 0.5 - cameraX ) ) % tileSize + tileSize * 0.5
      sy = ( math.floor( settings.windowSize.y * 0.5 - cameraY ) ) % tileSize + tileSize * 0.5
      local halfTileSize = tileSize * 0.5
      for x = 0, settings.windowSize.x, tileSize do
        Render( 'line', sx + x, 0, 0, sx + x, settings.windowSize.y, 0, '00FF0044' )
      end
      for y = 0, settings.windowSize.y, tileSize do
        Render( 'line', 0, sy + y, 0, settings.windowSize.x, sy + y, 0, '00FF0044' )
      end
    end,
  }, -- grid
}


-- Инициализация
function EditorInit()
  LoadScript( 'data/scripts/math.lua' )
  LoadScript( 'data/scripts/objecttemplates.lua' )
  math.randomseed( GetRandomSeed() )
  local x,y = GetWindowSize()
  settings.windowSize.x = x
  settings.windowSize.y = y

  -- Настраиваем блок с шаблонами
  GUI.templates.height = settings.windowSize.y - 10
  GUI.templates.items = GetObjectsTemplates()
  GUI.templates.width = GUI.templates.itemSize + 25

  -- Ставим обработчики на всё: клаву, кнопки и движение мыши
  ListenKeyboard( 'OnEditorKey' )
  ListenMouseKey( 'OnEditorMouseKey' )
  ListenMouseMove( 'OnEditorMouseMove' )

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

  UpdateGuiBySelectedObject()

  EditorUpdateDebug()
  RenderGUI()
end --EditorInit


-- Обработка кнопок клавы
function OnEditorKey( id, isPressed )
  if isPressed then
      if id == 0x1B then    -- Esc
        GameExit()
      end
      if id == 0x5A then    -- Z
        if settings.keys.isCtrl then  -- Ctrl+Z
          PopFromBuffer()
        else
          settings.guiVisibility = not settings.guiVisibility
          SetGuiVisibility( settings.guiVisibility )
        end
      end
      if id == 0xC0 then    -- ~
        GuiSetText( 'editor/settings.layer', 'default' )
        DebugRender( 0 )
        settings.editorType = 0
        settings.editorMode = 0
        SelectObject( '' )
        UpdateGuiBySelectedObject()
      end
      if id == 0x31 then    -- 1
        GuiSetText( 'editor/settings.layer', 'renderable' )
        DebugRender( 1 )
        settings.editorType = 1
      end
      if id == 0x32 then    -- 2
        GuiSetText( 'editor/settings.layer', 'collision' )
        DebugRender( 2 )
        settings.editorType = 2
      end
      if id == 0x33 then    -- 3
        GuiSetText( 'editor/settings.layer', 'trigger' )
        DebugRender( 4 )
        settings.editorType = 3
      end
      if id == 0x2E then    -- Del
        local object = GuiGetText( 'editor/object.object_name' )
        if #object > 0 then
          ObjectRemove( object )
          SelectObject( '' )
          if settings.editorMode == 2 then
            settings.editorMode = 0
          end
        end
      end
      if id == 0x20 then    -- Space
        SelectObject( '' )
        UpdateGuiBySelectedObject()
        if settings.editorMode == 2 then
          settings.editorMode = 0
        end
      end
      if id == 0x51 then    -- Q: Prev template
        if GUI.templates.currentItem > 1 then
          GUI.templates.currentItem = GUI.templates.currentItem - 1
        end
      end
      if id == 0x45 then    -- E: Next template
        if GUI.templates.currentItem < #GUI.templates.items then
          GUI.templates.currentItem = GUI.templates.currentItem + 1
        end
      end
      if id == 0x10 then    -- Shift
        ToggleGrid()
        settings.keys.isShift = isPressed
      end
      if id == 0x11 then    -- Control
        settings.keys.isCtrl = isPressed
      end
      if id == 0x12 then    -- Alt
        settings.keys.isAlt = isPressed
      end
  end
end --OnEditorKey

-- Обработка кнопок мыши
function OnEditorMouseKey( id, isPressed )
  local mode = {
    [0] = function()
      if isPressed then
        if settings.editorType == 0 and GUI.templates.currentItem > 0 then
          settings.editorMode = 20
          settings.move.mouseStart.x = mousePos.x
          settings.move.mouseStart.y = mousePos.y
        else
          local object = GetObjectUnderCursorByMode()
          if #object > 0 then
              SelectObject( object )
              UpdateGuiBySelectedObject()
              settings.editorMode = 1
          else
              -- multiselect?
          end
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
          local x, y = ObjectGetPos( GetSelectedObject() )
          settings.objectMode3Moved = false
          if settings.objectMode3StartPosition[ 1 ] ~= x or settings.objectMode3StartPosition[ 2 ] ~= y then
            local name = GetSelectedObject()
            local oldX = settings.objectMode3StartPosition[ 1 ]
            local oldY = settings.objectMode3StartPosition[ 2 ]
            PushToBuffer( function()
              end, function()
                ObjectSetPos( name, oldX, oldY )
              end
            )
          end
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
    [10] = function()
      if isPressed then
        if mousePos.x >= GUI.templates.x and
           mousePos.x <= GUI.templates.x + GUI.templates.width and
           mousePos.y >= GUI.templates.y and
           mousePos.y <= GUI.templates.y + 15
           then
          settings.editorMode = 11
          GUI.templates.moving.x = mousePos.x
          GUI.templates.moving.y = mousePos.y
        elseif mousePos.x >= GUI.templates.x + GUI.templates.width - 15 and
          mousePos.x <= GUI.templates.x + GUI.templates.width and
          mousePos.y >= GUI.templates.y + 15 and
          mousePos.y <= GUI.templates.y + GUI.templates.height then
            settings.editorMode = 12
            TestMouseOnGUI( mousePos.x, mousePos.y )
        else
          local num = math.floor( ( mousePos.y - GUI.templates.y - 15 + GUI.templates.scroll * GUI.templates.maxScroll ) / ( GUI.templates.itemSize + 5 ) ) + 1
          if num > 0 and num <= #GUI.templates.items then
            GUI.templates.currentItem = num
            DebugRender( 0 )
            settings.editorType = 0
            SelectObject( '' )
            UpdateGuiBySelectedObject()
          end
        end
      else
      end
    end,
    [11] = function()
      if isPressed then
      else
        settings.editorMode = 10
      end
    end,
    [12] = function()
      if isPressed then
      else
        if settings.editorMode == 12 then
          settings.editorMode = 10
        end
      end
    end,
    [20] = function()
      settings.editorMode = 0
      local name = EditorInsertItemByTemplate( mousePos.x, mousePos.y )
      PushToBuffer( function()
        end, function()
          ObjectRemove( name )
        end
      )
    end,
    [21] = function()
      if isPressed then
      else
        settings.editorMode = 0
        local cx, cy = GetCameraPos()
        local x = settings.move.mouseStart.x - cx + settings.windowSize.x * 0.5
        local y = settings.move.mouseStart.y - cy + settings.windowSize.y * 0.5
        local newX = mousePos.x
        local newY = mousePos.y
        local left   = x < newX and x or newX
        local right  = x < newX and newX or x
        local top    = y < newY and y or newY
        local bottom = y < newY and newY or y
        local tileSize = GetTileSize()
        left, top = GetTilePosByPixel( left, top )
        right, bottom = GetTilePosByPixel( right, bottom )
        local texture = GUI.templates.items[ GUI.templates.currentItem ].icon
        local names = {}
        for tx = left, right do
        for ty = top, bottom do
          x, y = GetPixelByTile( tx, ty )
          names[ #names + 1 ] = EditorInsertItemByTemplate( x, y )
        end
        end
        PushToBuffer( function()
          end, function()
            for q = 1,#names do
              ObjectRemove( names[ q ] )
            end
          end
        )
      end
    end, -- 21
  }
  mode[ settings.editorMode ]()
end --OnEditorMouseKey

-- Обработка движения мыши
function OnEditorMouseMove( x, y )
  if TestMouseOnGUI( x, y ) then
  else
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
        newX = math.floor( newX / tileSize ) * tileSize
        newY = math.floor( newY / tileSize ) * tileSize
        if not settings.objectMode3Moved then
          settings.objectMode3StartPosition = { oldX, oldY }
        end
        ObjectSetPos( object, newX, newY )
        settings.objectMode3Moved = true
      end,
      [20] = function()
        settings.editorMode = 21
        local cx, cy = GetCameraPos()
        settings.move.mouseStart.x = mousePos.x - settings.windowSize.x * 0.5 + cx
        settings.move.mouseStart.y = mousePos.y - settings.windowSize.y * 0.5 + cy
      end,
      [21] = function()
      end,
    }
    mode[ settings.editorMode ]()
  end
  mousePos.x = x
  mousePos.y = y
end --OnEditorMouseMove


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
end --ToggleRenderable

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
end --ToggleCollision

-- Чекбокс "Collision:Static"
function ToggleStatic( guiName )
  local name = GetSelectedObject()
  if #name < 1 then
    return
  end
  local checked = GuiGetChecked( 'editor/object.is_static' )
  ObjectAttr( name, { collisionStatic = checked } )
end --ToggleStatic

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
end --ToggleTrigger

-- Возвращает текущий размер тайлов (из дропбокса)
function GetTileSize()
  return GuiGetText( 'editor/world.tile_size' )
end --GetTileSize

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
end --TileApply

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
end --ToggleLayer

--
-- EditorUpdateDebug
--
function EditorUpdateDebug()
  local cx, cy = GetCameraPos()
  GuiSetText( 'editor/debug', settings.editorMode..':buffer['..#settings.buffer..']' )
  SetTimer( 0.01, 'EditorUpdateDebug' )
end -- EditorUpdateDebug

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
end --UpdateGuiBySelectedObject

function ToggleRenderableSize()
  local object = GetSelectedObject()
  if #object < 1 then
  else
    local width   = GuiGetText( 'editor/renderable.width' )
    local height  = GuiGetText( 'editor/renderable.height' )
    ObjectAttr( object, { renderableSize = width..' '..height } )
  end
end --ToggleRenderableSize

function RenderGUI()
  settings.timer = settings.timer + 0.01
  Render( 'clrscr' )
  if settings.showGrid then
    GUI.grid.Render()
  end
  if settings.editorMode == 21 then -- multi-insert tiles
    local cx, cy = GetCameraPos()
    local x = settings.move.mouseStart.x - cx + settings.windowSize.x * 0.5
    local y = settings.move.mouseStart.y - cy + settings.windowSize.y * 0.5
    local newX = mousePos.x
    local newY = mousePos.y
    Render( 'rect', x, y, 0, newX, newY, 0, '00ff00ff' )
    local left   = x < newX and x or newX
    local right  = x < newX and newX or x
    local top    = y < newY and y or newY
    local bottom = y < newY and newY or y
    local tileSize = GetTileSize()
    left, top = GetTilePosByPixel( left, top )
    right, bottom = GetTilePosByPixel( right, bottom )
    local texture = GUI.templates.items[ GUI.templates.currentItem ].icon
    for tx = left, right do
    for ty = top, bottom do
      x, y = GetPixelByTile( tx, ty )
      Render( 'sprite', x, y, 0, x + tileSize, y + tileSize, 0, texture, 'ffffff44' )
    end
    end
  end
  GUI.templates.Draw()
  SetTimer( 0.01, 'RenderGUI' )
end --RenderGUI

function TestMouseOnGUI( x, y )
  if settings.editorMode == 12 then
    GUI.templates.scroll = ( y - GUI.templates.y - 8 - 15 ) / ( GUI.templates.height - 32 )
    if GUI.templates.scroll < 0 then
      GUI.templates.scroll = 0
    elseif GUI.templates.scroll > 1 then
      GUI.templates.scroll = 1
    end
    return true
  elseif settings.editorMode == 11 then
    local dx = mousePos.x - GUI.templates.moving.x
    local dy = mousePos.y - GUI.templates.moving.y
    GUI.templates.moving.x = mousePos.x
    GUI.templates.moving.y = mousePos.y
    GUI.templates.x = GUI.templates.x + dx
    GUI.templates.y = GUI.templates.y + dy
    return true
  end
  if x >= GUI.templates.x and
     x <= GUI.templates.x + GUI.templates.width and
     y >= GUI.templates.y and
     y <= GUI.templates.y + GUI.templates.height
     then
    settings.editorMode = 10
    return true
  else
    if settings.editorMode == 10 then
      settings.editorMode = 0
      return false
    end
  end
  return false
end --TestMouseOnGUI

--[[ tableLength ]]
function tableLength( T )
  local count = 0
  for _ in pairs( T ) do count = count + 1 end
  return count
end --tableLength

--[[ EditorInsertItemByTemplate ]]
function EditorInsertItemByTemplate( px, py )
  if GUI.templates.currentItem <= 0 or GUI.templates.currentItem > #GUI.templates.items then
    return ''
  end

  local cameraX, cameraY = GetCameraPos()
  local tileSize = GetTileSize()
  local attrs = GUI.templates.items[ GUI.templates.currentItem ].attr
  local tags = GUI.templates.items[ GUI.templates.currentItem ].tags
  attrs.renderableSize = tileSize..' '..tileSize
  if attrs['collision'] ~= nil and attrs.collision then
    local size = tileSize
    if attrs['_collisionScale'] ~= nil then
      size = size * attrs['_collisionScale']
    end
    attrs['collisionSize'] = size..' '..size
  end
  --attrs.collisionSize = ( tileSize * _collisionScale )..' '..( tileSize * _collisionScale )
  --attrs.triggerSize = ( tileSize * _triggerScale )..' '..( tileSize * _triggerScale )

  local x, y = GetTilePosByPixel( px, py )
  -- local gridPos = GetGridByCoords( x * tileSize, y * tileSize )
  local name = 'wall.'..px..'.'..py..'.'..settings.timer

  --ObjectRemove( name )
  ObjectCreate( name, x * tileSize, y * tileSize, 0 )
  ObjectAttr( name, attrs )
  if GUI.templates.items[ GUI.templates.currentItem ].creationScript ~= nil then
    GUI.templates.items[ GUI.templates.currentItem ].creationScript( name )
  end

  if tags ~= nil then
    for q = 1,#tags do
      ObjectAddTag( name, tags[ q ] )
    end
  end

  return name
end --EditorInsertItemByTemplate

--[[ ToggleGrid ]]
function ToggleGrid()
  settings.showGrid = not settings.showGrid
end --ToggleGrid


--[[ BUFFER ]]
--[[ PushToBuffer ]]
function PushToBuffer( action, cancelAction )
  settings.buffer[ #settings.buffer + 1 ] = {
    DoAction = action,
    CancelAction = cancelAction,
  }
end -- PushToBuffer

--[[ PopFromBuffer ]]
function PopFromBuffer()
  if #settings.buffer > 0 then
    settings.buffer[ #settings.buffer ].CancelAction()
    settings.buffer[ #settings.buffer ] = nil
  end
end -- PopFromBuffer
