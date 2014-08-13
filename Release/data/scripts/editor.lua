--[[

Визуальный редактор

]]
settings = {
    guiVisibility     = false,  -- отображение GUI

    objectMode3Moved  = false,  -- bool если происходит перемещение объекта мышкой
    objectMode3StartPosition = { 0, 0 },  -- начальное положение объекта, до перемещения

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
      lastOffset = {
        x = 0,
        y = 0,
      },
      blockAxis = {
        x = false,
        y = false,
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
    gridSize = Config.GetGridSize(),
    gamePaused = false,
    cameraDirection = 0,  -- перемещение камеры
    multiSelectStartPoint = { x = 0, y = 0 }, -- стартовая точка мультиселекта
    ambientLight = { R = 1.0, G = 1.0, B = 1.0, A = 0.2 },
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
  tooltip = nil,
  elements = {},
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
    zIndex = 9.0,
    Draw = function()
      GUI.templates.maxScroll = ( #GUI.templates.items * ( GUI.templates.itemSize + 5 ) + 10 ) - ( GUI.templates.height - 15 )
      if GUI.templates.maxScroll < 1 then
        GUI.templates.maxScroll = 1
      end
      local y
      local i
      -- bg
      Debug.Render( 'sprite', GUI.templates.x, GUI.templates.y, GUI.templates.zIndex, GUI.templates.x + GUI.templates.width, GUI.templates.y + GUI.templates.height, GUI.templates.zIndex, 'data/temp/blank.png', '333333FF' )
      -- scroll bg
      Debug.Render( 'sprite', GUI.templates.x + GUI.templates.width - 15, GUI.templates.y, GUI.templates.zIndex, GUI.templates.x + GUI.templates.width, GUI.templates.y + GUI.templates.height, GUI.templates.zIndex, 'data/temp/blank.png', '555555FF' )
      -- scroll bar
      y = GUI.templates.y + 15 + GUI.templates.scroll*(GUI.templates.height-29)
      Debug.Render( 'sprite', GUI.templates.x + GUI.templates.width - 14, y + 1, GUI.templates.zIndex, GUI.templates.x + GUI.templates.width - 1, y + 13, GUI.templates.zIndex, 'data/temp/blank.png', '888888FF' )
      -- header
      Debug.Render( 'sprite', GUI.templates.x, GUI.templates.y, GUI.templates.zIndex, GUI.templates.x + GUI.templates.width, GUI.templates.y + 15, GUI.templates.zIndex, 'data/temp/blank.png', '999999FF' )
      -- items
      -- Debug.Render( 'scissorEnable', GUI.templates.x, settings.windowSize.y - ( GUI.templates.y + GUI.templates.height - 0 ), GUI.templates.x + GUI.templates.width - 15, settings.windowSize.y - 16 )
      Debug.Render( 'scissorEnable', GUI.templates.x + 1, settings.windowSize.y - ( GUI.templates.y + GUI.templates.height - 1 ), GUI.templates.x + GUI.templates.width - 17, settings.windowSize.y - ( GUI.templates.y + 17 ) )
      i = 0
      for key, item in pairs( GUI.templates.items ) do
        GUI.templates.DrawItem( key, item )
      end
      if GUI.templates.currentItem > 0 and GUI.templates.currentItem <= #GUI.templates.items then
        y = GUI.templates.y - GUI.templates.scroll * GUI.templates.maxScroll + ( GUI.templates.currentItem - 1 ) * ( GUI.templates.itemSize + 5 ) + 20
        Debug.Render( 'rect', GUI.templates.x + 5, y, GUI.templates.zIndex, GUI.templates.x + 5 + GUI.templates.itemSize, y + GUI.templates.itemSize, GUI.templates.zIndex, '00ff00ff' )
      end
      Debug.Render( 'scissorDisable' )
    end,
    DrawItem = function( i, item )
      y = GUI.templates.y - GUI.templates.scroll * GUI.templates.maxScroll + ( i - 1 ) * ( GUI.templates.itemSize + 5 ) + 20
      Debug.Render( 'sprite', GUI.templates.x + 5, y, GUI.templates.zIndex, GUI.templates.x + 5 + GUI.templates.itemSize, y + GUI.templates.itemSize, GUI.templates.zIndex, item.icon, 'ffffffff' )
    end,
  }, -- templates

  tabbedTemplates = {
    tabsList = {
      {
        id = 'blocks',
        title = 'Блоки',
        items = {},
      },
      {
        id = 'grass',
        title = 'Трава',
        items = {},
      },
      {
        id = 'sky',
        title = 'Небо',
        items = {},
      },
      {
        id = 'light',
        title = 'Свет',
        items = {},
      },
      {
        id = 'go',
        title = 'Объекты',
        items = {},
      },
      {
        id = 'npc',
        title = 'NPC',
        items = {},
      },
      {
        id = 'other',
        title = 'Разное',
        items = {},
      },
    },
    position = {
      x = 0,
      y = 20,
      height = 250,
      width = nil,
    },
    tileSize = 24,
    tilesPerRow = 13,
    zIndex = 8.5,
    currentTab = 1,
    currentItem = 0,
    isHovered = false,
    isTitleHovered = false,
    isMinimized = false,
    Init = function()
      GUI.tabbedTemplates.position.width = GUI.tabbedTemplates.tilesPerRow * GUI.tabbedTemplates.tileSize + 4 + 10
      local offset = 0
      for num,tab in pairs( GUI.tabbedTemplates.tabsList ) do --(
        local width = Debug.Render( 'getTextWidth', tab.title ) + 4
        tab.offset = offset
        tab.width = width
        offset = offset + width
      end --)
      local tableData = GetObjectsTabbedTemplates()
      for num,tab in pairs( GUI.tabbedTemplates.tabsList ) do --(
        for tdId ,tdTab in pairs( tableData ) do --(
          if tdId == tab.id then  --(
            tab.items = tdTab
          end --)
        end --)
      end --)
    end,
    Draw = function() --(
      local x, y, z, width, height
        = GUI.tabbedTemplates.position.x
        , GUI.tabbedTemplates.position.y
        , GUI.tabbedTemplates.zIndex
        , GUI.tabbedTemplates.position.width + 2
        , GUI.tabbedTemplates.isMinimized and 11 or GUI.tabbedTemplates.position.height
      -- background
      Debug.Render( 'sprite', x, y, z, x + width, y + height, z, 'data/temp/blank.png', 'EDEDEDFF' )
      Debug.Render( 'rect', x, y, z, x + width, y + height, z, 'A9A9A9FF' )
      -- title
      Debug.Render( 'sprite', x + 1, y + 2, z, x + width - 2, y + 10, z, 'data/temp/blank.png', GUI.tabbedTemplates.isTitleHovered and 'ffaaaaff' or 'ffaaaa77' )
      if not GUI.tabbedTemplates.isMinimized then
        -- tabs
        for num,tab in pairs( GUI.tabbedTemplates.tabsList ) do --(
          GUI.tabbedTemplates.DrawTab( num, tab, GUI.tabbedTemplates.position.x, GUI.tabbedTemplates.position.y + 10 )
        end --)
        -- items
        x = x + 2
        y = y + 29
        for num,item in pairs( GUI.tabbedTemplates.tabsList[ GUI.tabbedTemplates.currentTab ].items ) do --(
          local tx, ty, tz
            = x + ( ( num - 1 ) % GUI.tabbedTemplates.tilesPerRow ) * GUI.tabbedTemplates.tileSize
            , y + math.floor( ( num - 1 ) / GUI.tabbedTemplates.tilesPerRow ) * GUI.tabbedTemplates.tileSize
            , ( ( item.isHovered or num == GUI.tabbedTemplates.currentItem ) and GUI.tabbedTemplates.zIndex - 0.1 or GUI.tabbedTemplates.zIndex )
          local borderColor = ( num == GUI.tabbedTemplates.currentItem and 'ff0000ff' or ( item.isHovered and 'ff6666ff' or 'ffffffff' ) )
          Debug.Render( 'sprite', tx, ty, tz, tx + GUI.tabbedTemplates.tileSize - 1, ty + GUI.tabbedTemplates.tileSize, tz, 'data/temp/blank.png', '000000ff' )
          Debug.Render( 'sprite', tx, ty, tz, tx + GUI.tabbedTemplates.tileSize, ty + GUI.tabbedTemplates.tileSize, tz, item.icon, 'ffffffff' )
          Debug.Render( 'rect', tx, ty, tz, tx + GUI.tabbedTemplates.tileSize, ty + GUI.tabbedTemplates.tileSize, tz, borderColor )
        end
      end
    end,  --) Draw
    DrawTab = function( id, tab, dx, dy ) --(
      local x, y, z = dx + tab.offset + 2, dy + 2, GUI.tabbedTemplates.zIndex
      local isCurrent = GUI.tabbedTemplates.currentTab == id
      local
        colorBG, colorBGbottom, colorBorder =
        ( isCurrent and 'ffaaaaff' or ( tab.isHovered and 'ffeeffff' or 'ffffffff' ) ), ( isCurrent and 'ff8888ff' or ( tab.isHovered and 'ffccccff' or 'ffffffff' ) ), 'A9A9A9FF'
      Debug.Render( 'sprite', x, y, z, x + tab.width, y + 8, z, 'data/temp/blank.png', colorBG )
      Debug.Render( 'sprite', x, y + 8, z, x + tab.width, y + 15, z, 'data/temp/blank.png', colorBGbottom )
      Debug.Render( 'rect', x, y, z, x + tab.width, y + 15, z, colorBorder )
      Debug.Render( 'text', x + 2, y + 2, z, tab.title, '000000ff' )
    end,  --) DrawTab
    OnMouseMove = function( x, y )  --(
      if not settings.guiVisibility then
        GUI.tabbedTemplates.isHovered = false
        return
      end
      if settings.editorMode == 31 then
        GUI.tabbedTemplates.position.x = GUI.tabbedTemplates.moving.position.x + x - GUI.tabbedTemplates.moving.mousePos.x
        GUI.tabbedTemplates.position.y = GUI.tabbedTemplates.moving.position.y + y - GUI.tabbedTemplates.moving.mousePos.y
      end
      GUI.tabbedTemplates.isHovered =
        x >= GUI.tabbedTemplates.position.x
        and x <= GUI.tabbedTemplates.position.x + GUI.tabbedTemplates.position.width
        and y >= GUI.tabbedTemplates.position.y
        and y <= GUI.tabbedTemplates.position.y + ( GUI.tabbedTemplates.isMinimized and 11 or GUI.tabbedTemplates.position.height )
      GUI.tabbedTemplates.isTitleHovered = 
        mousePos.x >= GUI.tabbedTemplates.position.x and
        mousePos.x <= GUI.tabbedTemplates.position.x + GUI.tabbedTemplates.position.width and
        mousePos.y >= GUI.tabbedTemplates.position.y and
        mousePos.y <= GUI.tabbedTemplates.position.y + 10
      if not GUI.tabbedTemplates.isMinimized then --(
        if GUI.tabbedTemplates.isHovered then --(
          if settings.editorMode == 0 then
            settings.editorMode = 30
          end
        else  --)(
          if settings.editorMode == 30 then
            settings.editorMode = 0
          end
        end --)
        if GUI.tabbedTemplates.isHovered then --(
          local dx, dy, dz, width, height = GUI.tabbedTemplates.position.x, GUI.tabbedTemplates.position.y, GUI.tabbedTemplates.zIndex, GUI.tabbedTemplates.position.width + 2, GUI.tabbedTemplates.position.height + 30
          dx = dx + 2
          dy = dy + 29
          local hovered = 0
          for num,item in pairs( GUI.tabbedTemplates.tabsList[ GUI.tabbedTemplates.currentTab ].items ) do --(
            local tx, ty = dx + ( ( num - 1 ) % GUI.tabbedTemplates.tilesPerRow ) * GUI.tabbedTemplates.tileSize, dy + math.floor( ( num - 1 ) / GUI.tabbedTemplates.tilesPerRow ) * GUI.tabbedTemplates.tileSize
            item.isHovered = x >= tx and x <= tx + GUI.tabbedTemplates.tileSize and y >= ty and y <= ty + GUI.tabbedTemplates.tileSize
            if item.isHovered then
              hovered = num
            end
          end --)
          if hovered > 0 then
            GUI.tooltip:SetPosition( x + 1, y - 25 )
            GUI.tooltip:SetText( GUI.tabbedTemplates.tabsList[ GUI.tabbedTemplates.currentTab ].items[ hovered ].name )
            GUI.tooltip:CropByTextWidth()
          else
            GUI.tooltip:SetPosition( 0, settings.windowSize.y )
          end
        end --)
        for num,tab in pairs( GUI.tabbedTemplates.tabsList ) do --(
          tab.isHovered = 
            x >= GUI.tabbedTemplates.position.x + tab.offset
            and x <= GUI.tabbedTemplates.position.x + tab.offset + tab.width
            and y >= GUI.tabbedTemplates.position.y + 10
            and y <= GUI.tabbedTemplates.position.y + 28
        end --)
      end --)
    end,  --) OnMouseMove
    OnMouseClick = function( id, isPressed ) --(
      if isPressed then
        if GUI.tabbedTemplates.isTitleHovered then
          -- move gui
          settings.editorMode = 31
          GUI.tabbedTemplates.moving = {
            mousePos = {
              x = mousePos.x,
              y = mousePos.y,
            },
            position = {
              x = GUI.tabbedTemplates.position.x,
              y = GUI.tabbedTemplates.position.y,
            }
          }
        elseif mousePos.x >= GUI.tabbedTemplates.position.x + GUI.tabbedTemplates.position.width - 10 and
          mousePos.x <= GUI.tabbedTemplates.position.x + GUI.tabbedTemplates.position.width and
          mousePos.y >= GUI.tabbedTemplates.position.y + 10 and
          mousePos.y <= GUI.tabbedTemplates.position.y + GUI.tabbedTemplates.position.height then
            -- scroll
            settings.editorMode = 32
        else
          -- select
          for num,tab in pairs( GUI.tabbedTemplates.tabsList ) do --(
            if tab.isHovered then
              GUI.tabbedTemplates.currentTab = num
              GUI.tabbedTemplates.currentItem = 0
              break
            end
          end --)
          for num,item in pairs( GUI.tabbedTemplates.tabsList[ GUI.tabbedTemplates.currentTab ].items ) do --(
            if item.isHovered then
              GUI.tabbedTemplates.currentItem = num
              DoPause( true )
              settings.editorType = 0
              break
            end
          end --)
        end
      else
        if settings.editorMode == 31 and GUI.tabbedTemplates.moving.mousePos.x == mousePos.x and GUI.tabbedTemplates.moving.mousePos.y == mousePos.y then
          GUI.tabbedTemplates.isMinimized = not GUI.tabbedTemplates.isMinimized
        end
        settings.editorMode = 30
      end
    end,  --) OnMouseClick
  },  -- tabbedTemplates

  grid = {
    zIndex = 8.5,
    Render = function()
      local tileSize = GetTileSize()
      local offsetX, offsetY = GetTileOffset()
      local cameraX, cameraY = GetCameraPos()
      local sx, sy
      sx = ( math.floor( settings.windowSize.x * 0.5 - cameraX ) ) % tileSize + tileSize * 0.5 + offsetX
      sy = ( math.floor( settings.windowSize.y * 0.5 - cameraY ) ) % tileSize + tileSize * 0.5 + offsetY
      local halfTileSize = tileSize * 0.5
      for x = 0, settings.windowSize.x, tileSize do
        Debug.Render( 'line', sx + x, 0, GUI.grid.zIndex, sx + x, settings.windowSize.y, GUI.grid.zIndex, '00FF0044' )
      end
      for y = 0, settings.windowSize.y, tileSize do
        Debug.Render( 'line', 0, sy + y, GUI.grid.zIndex, settings.windowSize.x, sy + y, GUI.grid.zIndex, '00FF0044' )
      end
      for gx = 0,1 do
        local x = ( math.floor( cameraX / settings.gridSize ) + gx ) * settings.gridSize - cameraX + settings.windowSize.x * 0.5
        Debug.Render( 'line', x, 0, GUI.grid.zIndex, x, settings.windowSize.y, GUI.grid.zIndex, 'FF0000FF' )
      end
      for gy = 0,1 do
        local y = ( math.floor( cameraY / settings.gridSize ) + gy ) * settings.gridSize - cameraY + settings.windowSize.y * 0.5
        Debug.Render( 'line', 0, y, GUI.grid.zIndex, settings.windowSize.x, y, GUI.grid.zIndex, 'FF0000FF' )
      end
    end,
  }, -- grid
}


-- Инициализация
function EditorInit()
  LoadScript( 'data/scripts/tools.lua' )
  LoadScript( 'data/scripts/objecttemplates.lua' )
  math.randomseed( GetRandomSeed() )
  local x,y = GetWindowSize()
  settings.windowSize.x = x
  settings.windowSize.y = y

  -- Настраиваем блок с шаблонами
  -- GUI.templates.height = settings.windowSize.y - 10
  -- GUI.templates.items = GetObjectsTemplates()
  -- GUI.templates.width = GUI.templates.itemSize + 25
  GUI.tabbedTemplates.Init()

  -- Ставим обработчики на всё: клаву, кнопки и движение мыши
  -- Keyboard.Listen( 'OnEditorKey' )
  -- Mouse.ListenKey( 'OnEditorMouseKey' )
  -- ListenMouseMove( 'OnEditorMouseMove' )
  -- GUIRenderer.OnClickDefault = OnEditorMouseKey
  -- GUIRenderer.OnMouseMoveDefault = OnEditorMouseMove
  -- GUIRenderer.OnKeyboardDefault = OnEditorKey

  local editor = Object.New( 'editor', '', false )
  editor.api:SetScript( 'data/scripts/objects/editor.lua' )

  -- GUI
  GUI.tooltip = GUILabel:Create( 0, settings.windowSize.y, 200, 13, '', nil, function( self ) self.SetPosition( self, 0, settings.windowSize.y ) end, -9 )
  GUI.elements.labelDebug = GUILabel:Create( 0, 0, settings.windowSize.x, 20, 'debug' )

  -- settings block
  GUI.elements.windowSettings = GUILabel:Create( settings.windowSize.x - 220, 0, 220, 200, 'Settings' )

  -- layer
  GUILabel:Create( 0, 20, 0, 0, 'Layer:', GUI.elements.windowSettings )
  local list = {}
  table.insert( list, 'default' )
  table.insert( list, 'renderable' )
  table.insert( list, 'collision' )
  table.insert( list, 'trigger' )
  GUI.elements.layer = GUISelect:Create( 55, 20, 90, 1, list, OnChangeLayer, GUI.elements.windowSettings )

  --grid size
  GUILabel:Create( 0, 40, 0, 0, 'Grid:', GUI.elements.windowSettings )
  GUI.elements.showGrid = GUICheckbox:Create( 40, 42, '', false, ToggleGrid, GUI.elements.windowSettings, true )
  local list = {}
  table.insert( list, '8' )
  table.insert( list, '16' )
  table.insert( list, '24' )
  table.insert( list, '32' )
  table.insert( list, '48' )
  table.insert( list, '64' )
  table.insert( list, '80' )
  table.insert( list, '96' )
  table.insert( list, '112' )
  table.insert( list, '128' )
  table.insert( list, '144' )
  table.insert( list, '160' )
  table.insert( list, '176' )
  table.insert( list, '192' )
  table.insert( list, '208' )
  table.insert( list, '224' )
  table.insert( list, '240' )
  table.insert( list, '256' )
  table.insert( list, '512' )
  table.insert( list, '1024' )
  GUI.elements.gridSize = GUISelect:Create( 55, 40, 40, 3, list, OnChangeTileSize, GUI.elements.windowSettings )

  --grid offset
  GUILabel:Create( 95, 40, 0, 0, 'offset', GUI.elements.windowSettings )
  local list = {}
  for q = 0,32,2 do
    table.insert( list, q )
  end
  GUI.elements.gridOffsetX = GUISelect:Create( 150, 40, 30, 1, list, nil, GUI.elements.windowSettings )
  GUI.elements.gridOffsetY = GUISelect:Create( 180, 40, 30, 1, list, nil, GUI.elements.windowSettings )

  -- Ambient Light
  GUILabel:Create( 0, 60, 0, 0, 'Ambient light:', GUI.elements.windowSettings )
  -- GUI.elements.ambientLightR = GUIEdit:Create( 110, 60, 25, string.format( '%.1f', settings.ambientLight.R ), UpdateAmbientByGUI, GUI.elements.windowSettings )
  -- GUI.elements.ambientLightG = GUIEdit:Create( 110 + 27*1, 60, 25, string.format( '%.1f', settings.ambientLight.G ), UpdateAmbientByGUI, GUI.elements.windowSettings )
  -- GUI.elements.ambientLightB = GUIEdit:Create( 110 + 27*2, 60, 25, string.format( '%.1f', settings.ambientLight.B ), UpdateAmbientByGUI, GUI.elements.windowSettings )
  -- GUI.elements.ambientLightA = GUIEdit:Create( 110 + 27*3, 60, 25, string.format( '%.1f', settings.ambientLight.A ), UpdateAmbientByGUI, GUI.elements.windowSettings )
  GUI.elements.ambientLightR = GUISliderVertical:Create( 110, 67, 40, -1, 1, settings.ambientLight.R, 'R', UpdateAmbientByGUI, GUI.elements.windowSettings, true, '%1.2f' )
  GUI.elements.ambientLightG = GUISliderVertical:Create( 110 + 20*1, 67, 40, -1, 1, settings.ambientLight.G, 'G', UpdateAmbientByGUI, GUI.elements.windowSettings, true, '%1.2f' )
  GUI.elements.ambientLightB = GUISliderVertical:Create( 110 + 20*2, 67, 40, -1, 1, settings.ambientLight.B, 'B', UpdateAmbientByGUI, GUI.elements.windowSettings, true, '%1.2f' )
  GUI.elements.ambientLightA = GUISliderVertical:Create( 110 + 20*3, 67, 40, -1, 1, settings.ambientLight.A, 'A', UpdateAmbientByGUI, GUI.elements.windowSettings, true, '%1.2f' )

  -- object block
  GUI.elements.windowObject = GUILabel:Create( settings.windowSize.x - 220, 205, 220, 130, 'Object' )
  local y = 18
  GUILabel:Create( 1, y, 0, 0, 'Name:', GUI.elements.windowObject )
  GUI.elements.objectName   = GUIEdit:Create( 50, y, 165, '', function( obj ) Debug.Alert( obj:GetText() ) end, GUI.elements.windowObject )
  y = y + 20
  GUILabel:Create( 1, y, 0, 0, 'Z:', GUI.elements.windowObject )
  GUI.elements.objectZ = GUIEdit:Create( 20, y, 40, '', function( obj ) OnChangeZ( obj:GetText() ) end, GUI.elements.windowObject )
  GUI.elements.objectZslider = GUISliderVertical:Create( 62, y + 8, 150, -100, 10, 0, 'Z', function( obj ) OnChangeZ( obj:GetText() ) end, GUI.elements.windowObject, false, '%.1f' )
  y = y + 17
  GUI.elements.isRenderable = GUICheckbox:Create( 5, y, 'Renderable', false, OnChangeIsRenderable, GUI.elements.windowObject, false )
  y = y + 15
  GUI.elements.isCollision  = GUICheckbox:Create( 5, y, 'Collision', false, OnChangeIsCollision, GUI.elements.windowObject, false )
  y = y + 15
  GUI.elements.isTrigger    = GUICheckbox:Create( 5, y, 'Trigger', false, OnChangeIsTrigger, GUI.elements.windowObject, false )
  y = y + 15
  GUI.elements.isLightBlockByCollision = GUICheckbox:Create( 5, y, 'Light block by collision', false, OnChangeIsLightBlockByCollision, GUI.elements.windowObject, false )
  y = y + 15
  GUI.elements.isLightPoint = GUICheckbox:Create( 5, y, 'Light point', false, OnChangeIsLightPoint, GUI.elements.windowObject, false )

  -- GUIEdit:Create( 200, 50, 100, 'test123', function() Debug.Alert(2) end )

  -- Ставим обработчики чекбоксов и кнопок редактора
  --[[
  GuiAddTrigger( 'editor/settings.layer', 'ToggleLayer' )
  GuiAddTrigger( 'editor/object.is_renderable', 'ToggleRenderable' )
  GuiAddTrigger( 'editor/object.is_collision', 'ToggleCollision' )
  GuiAddTrigger( 'editor/object.is_trigger', 'ToggleTrigger' )
  GuiAddTrigger( 'editor/object.is_static', 'ToggleStatic' )
  GuiAddTrigger( 'editor/object.tile_apply', 'TileApply' )
  GuiAddTrigger( 'editor/renderable.width', 'ToggleRenderableSize' )
  GuiAddTrigger( 'editor/renderable.height', 'ToggleRenderableSize' )
  ]]

  -- показываем GUI
  settings.guiVisibility = true
  --SetGuiVisibility( settings.guiVisibility )

  Object.Get( 'editor' ):UpdateGuiBySelectedObject()

  EditorUpdateDebug( 0 )
  -- RenderGUI( 0 )
  UpdateEditorCamera( 0 )
end --EditorInit


-- UpdateEditorCamera
function UpdateEditorCamera( timerId )
  if settings.gamePaused then
    SetCamera( 'defaults/camera' )
  end
  local x, y = ObjectAttr( 'defaults/camera', { 'position' } )
  if bit32.band( settings.cameraDirection, 8 ) == 8 then
    y = y - 10
  end
  if bit32.band( settings.cameraDirection, 2 ) == 2 then
    y = y + 10
  end
  if bit32.band( settings.cameraDirection, 4 ) == 4 then
    x = x - 10
  end
  if bit32.band( settings.cameraDirection, 1 ) == 1 then
    x = x + 10
  end
  ObjectAttr( 'defaults/camera', { position = x..' '..y  } )
  Core.SetTimer( 0.01, UpdateEditorCamera, true )
end -- UpdateEditorCamera


--( Переключение режима паузы(редактора) и игры
function DoPause( setPause )
  settings.gamePaused = setPause
  SetPause( not settings.gamePaused )
  if settings.gamePaused then
    if GetCamera() ~= 'defaults/camera' then
      local x, y = ObjectAttr( 'player', { 'position' } )
      ObjectAttr( 'defaults/camera', { position = x..' '..y } )
    end
    SetCamera( 'defaults/camera' )
  else
    SetCamera( 'player' )
    Debug.RenderState( 0 )
    settings.editorType = 0
    GUI.elements.layer:SetText( 'default' )
    -- OnChangeLayer( GUI.elements.layer )
  end
end --)


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
    -- local layer = 'default' -- GuiGetText( 'editor/settings.layer' )
    local objects = Object.GetSelected()
    local name = ''
    for _,object in pairs( objects ) do
      name = object.api:GetNameFull()
      break
    end
    local objectByPoint = {}
    if settings.editorType == 1 then
      -- name = GetObjectByPoint( 1, pos.x, pos.y, name )
      objectByPoint = { Object.GetByPoint( 1, pos.x, pos.y, name ) }
      local tmp = nil
      for _,v in pairs( objectByPoint ) do
        tmp = v
        break
      end
    end
    if settings.editorType == 2 then
      -- name = GetObjectByPoint( 2, pos.x, pos.y, name )
      objectByPoint = { Object.GetByPoint( 2, pos.x, pos.y, name ) }
    end
    if settings.editorType == 3 then
      -- name = GetObjectByPoint( 3, pos.x, pos.y, name )
      objectByPoint = { Object.GetByPoint( 3, pos.x, pos.y, name ) }
    end
    return objectByPoint
end --GetObjectUnderCursorByMode

-- Чекбокс "Renderable"
--[[
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
]]

-- Возвращает текущий размер тайлов (из дропбокса)
function GetTileSize()
  return GUI.elements.gridSize:GetText()
end --GetTileSize

-- Возвращает текущее смещение сетки (из дропбокса)
function GetTileOffset()
  return GUI.elements.gridOffsetX:GetText(), GUI.elements.gridOffsetY:GetText()
end --GetTileSize

-- Кнопка "Apply"
-- TODO: переделать
--[[
function TileApply( guiName )
  local name = GuiGetText( 'editor/object.object_name' )
  local cameraX, cameraY = GetCameraPos()
  ObjectCreate( name, cameraX, cameraY, 0 )
  SelectObject( name )
  ToggleRenderable( 'editor/object.is_renderable' )
  ToggleCollision( 'editor/object.is_collision' )
  ToggleTrigger( 'editor/object.is_trigger' )
  Object.Get( 'editor' ):UpdateGuiBySelectedObject()
  settings.editorMode = 2
end --TileApply
]]

-- Дропбокс "Layer"
function ToggleLayer( guiName )
  -- local layer = GuiGetText( 'editor/settings.layer' )
  local flags = 0
  if settings.editorType == 1 then
    flags = 1
  end
  if settings.editorType == 2 then
    flags = 2
  end
  if settings.editorType == 3 then
    flags = 4
  end
  Debug.RenderState( flags )
end --ToggleLayer

--
-- EditorUpdateDebug
--
function EditorUpdateDebug( timerId )
  local cx, cy = GetCameraPos()
  local mx, my = GetMousePos()
  local width, height = GetWindowSize()
  local pos = {
    x = cx + mx - width * 0.5,
    y = cy + my - height * 0.5
  }
  GUI.elements.labelDebug:SetText( string.format( 'grid[%d; %d] pixel[%d; %d] mode[%d] test[%d]', math.floor( cx / settings.gridSize ), math.floor( cy / settings.gridSize ), math.floor( pos.x ), math.floor( pos.y ), settings.editorMode, ( GUI.tabbedTemplates.isHovered and '1' or '0' ) ) )
  -- GuiSetText( 'editor/debug', settings.editorMode..':buffer['..#settings.buffer..']' )
  Core.SetTimer( 0.01, EditorUpdateDebug, true )
end -- EditorUpdateDebug

--[[
function UpdateGuiBySelectedObject()
  -- local object = GetSelectedObject()
  -- local objectList = Explode( object, '//' )
  local objectList = Object.GetSelected()
  local doReset = false
  if( TableSize( objectList ) == 1 ) then --( one object
    if TableSize( objectList ) < 1 then
      doReset = true
    else
      local object = nil
      for _,obj in pairs( objectList ) do
        object = obj
        break
      end
      object = object.api:GetNameFull()
      local isRenderable
        , isCollision
        , isTrigger
        , isLightBlockByCollision
        , isLightPoint
        , textureName
        , renderableSizeX
        , renderableSizeY
        , objectZ
        = ObjectAttr( object, {
          'renderable',
          'collision',
          'trigger',
          'lightBlockByCollision',
          'lightPoint',
          'textureName',
          'renderableSize',
          'z',
        } )
      GUI.elements.objectName:SetEnabled( true )
      GUI.elements.objectName:SetText( object )
      GUI.elements.objectZ:SetEnabled( true )
      GUI.elements.objectZ:SetText( string.format( '%.3f', objectZ ) )
      GUI.elements.objectZslider:SetEnabled( true )
      GUI.elements.objectZslider:SetText( string.format( '%.3f', objectZ ) )
      GUI.elements.isRenderable:SetEnabled( true )
      GUI.elements.isRenderable:SetIsChecked( isRenderable )
      GUI.elements.isCollision:SetEnabled( true )
      GUI.elements.isCollision:SetIsChecked( isCollision )
      GUI.elements.isTrigger:SetEnabled( true )
      GUI.elements.isTrigger:SetIsChecked( isTrigger )
      GUI.elements.isLightBlockByCollision:SetEnabled( true )
      GUI.elements.isLightBlockByCollision:SetIsChecked( isLightBlockByCollision )
      GUI.elements.isLightPoint:SetEnabled( true )
      GUI.elements.isLightPoint:SetIsChecked( isLightPoint )
    end
  else --)( multi-object
    local isRenderable, isCollision, isTrigger, isLightBlockByCollision, isLightPoint, objectZ = false, false, false, false, false, 0.0
    for _,object in pairs( objectList ) do
      local name = object.api:GetNameFull()
      isRenderable = isRenderable or ObjectAttr( name, { 'renderable' } )
      isCollision = isCollision or ObjectAttr( name, { 'collision' } )
      isTrigger = isTrigger or ObjectAttr( name, { 'trigger' } )
      isLightBlockByCollision = isLightBlockByCollision or ObjectAttr( name, { 'lightBlockByCollision' } )
      isLightPoint = isLightPoint or ObjectAttr( name, { 'lightPoint' } )
      objectZ = ObjectAttr( name, { 'z' } )
    end
    GUI.elements.objectName:SetEnabled( false )
    GUI.elements.objectName:SetText( '' )
    GUI.elements.objectZ:SetEnabled( true )
    GUI.elements.objectZ:SetText( string.format( '%.3f', objectZ ) )
    GUI.elements.objectZslider:SetEnabled( true )
    GUI.elements.objectZslider:SetText( string.format( '%.3f', objectZ ) )
    GUI.elements.isRenderable:SetEnabled( true )
    GUI.elements.isRenderable:SetIsChecked( isRenderable )
    GUI.elements.isCollision:SetEnabled( true )
    GUI.elements.isCollision:SetIsChecked( isCollision )
    GUI.elements.isTrigger:SetEnabled( true )
    GUI.elements.isTrigger:SetIsChecked( isTrigger )
    GUI.elements.isLightBlockByCollision:SetEnabled( true )
    GUI.elements.isLightBlockByCollision:SetIsChecked( isLightBlockByCollision )
    GUI.elements.isLightPoint:SetEnabled( true )
    GUI.elements.isLightPoint:SetIsChecked( isLightPoint )
  end --)
  if doReset then
    GUI.elements.objectName:SetEnabled( false )
    GUI.elements.objectName:SetText( '' )
    GUI.elements.objectZ:SetEnabled( false )
    GUI.elements.objectZ:SetText( '' )
    GUI.elements.objectZslider:SetEnabled( false )
    GUI.elements.objectZslider:SetText( '' )
    GUI.elements.isRenderable:SetEnabled( false )
    GUI.elements.isRenderable:SetIsChecked( false )
    GUI.elements.isCollision:SetEnabled( false )
    GUI.elements.isCollision:SetIsChecked( false )
    GUI.elements.isTrigger:SetEnabled( false )
    GUI.elements.isTrigger:SetIsChecked( false )
    GUI.elements.isLightBlockByCollision:SetEnabled( false )
    GUI.elements.isLightBlockByCollision:SetIsChecked( false )
    GUI.elements.isLightPoint:SetEnabled( false )
    GUI.elements.isLightPoint:SetIsChecked( false )
  end
end --UpdateGuiBySelectedObject
]]

--[[
function ToggleRenderableSize()
  local object = GetSelectedObject()
  if #object < 1 then
  else
    local width   = GuiGetText( 'editor/renderable.width' )
    local height  = GuiGetText( 'editor/renderable.height' )
    ObjectAttr( object, { renderableSize = width..' '..height } )
  end
end --ToggleRenderableSize
]]
function RenderGUI( timerId )
  settings.timer = settings.timer + 0.01
  Debug.Render( 'clrscr' )
  if settings.showGrid then
    GUI.grid.Render()
  end
  --( вставка блока тайлов
  if settings.editorMode == 21 then
    local cx, cy = GetCameraPos()
    local x = settings.move.mouseStart.x - cx + settings.windowSize.x * 0.5
    local y = settings.move.mouseStart.y - cy + settings.windowSize.y * 0.5
    local newX = mousePos.x
    local newY = mousePos.y
    Debug.Render( 'rect', x, y, 8.0, newX, newY, 8.0, '00ff00ff' )
    local left   = x < newX and x or newX
    local right  = x < newX and newX or x
    local top    = y < newY and y or newY
    local bottom = y < newY and newY or y
    local tileSize = GetTileSize()
    local offsetX, offsetY = GetTileOffset()
    left, top = GetTilePosByPixel( left, top )
    right, bottom = GetTilePosByPixel( right, bottom )
    local texture = GUI.tabbedTemplates.tabsList[ GUI.tabbedTemplates.currentTab ].items[ GUI.tabbedTemplates.currentItem ].icon
    for tx = left, right do
    for ty = top, bottom do
      x, y = GetPixelByTile( tx, ty )
      Debug.Render( 'sprite', x, y, 8.0, x + tileSize, y + tileSize, 8.0, texture, 'ffffff44' )
    end
    end
  end --)
  --( рамка тайла под курсором
  if settings.editorMode == 0 and settings.gamePaused and GUI.tabbedTemplates.currentItem > 0 then
    x, y = GetTilePosByPixel( mousePos.x, mousePos.y )
    left, top = GetPixelByTile( x, y )
    right, bottom = GetPixelByTile( x + 1, y + 1 )
    Debug.Render( 'rect', left, top, 8.0, right, bottom, 8.0, 'ff0000ff' )
  end --)

  --( рамка мультиселекта
  if settings.editorMode == 22 then
    local cx, cy = GetCameraPos()
    left = settings.multiSelectStartPoint.x - cx
    top = settings.multiSelectStartPoint.y - cy
    right = mousePos.x
    bottom = mousePos.y
    if left > right then
      left, right = right, left
    end
    if top > bottom then
      top, bottom = bottom, top
    end
    Debug.Render( 'rect', left, top, 8.0, right, bottom, 8.0, 'ff0000ff' )
  end --)

  if settings.guiVisibility then
    -- GUI.templates.Draw()
    GUI.tabbedTemplates.Draw()
    GUIRendererRender( 0 ) -- additional GUI from gui.lua
  end
  Core.SetTimer( 1/30, RenderGUI, true )
end --RenderGUI

function TestMouseOnGUI( x, y )
  if not settings.guiVisibility then
    return false
  end
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
    if ( settings.editorMode == 0 or settings.editorMode == 10 ) and
      x >= GUI.templates.x and
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
  if GUI.tabbedTemplates.currentItem == 0 then
    return nil
  end

  local cameraX, cameraY = GetCameraPos()
  local tileSize = GetTileSize()
  local offsetX, offsetY = GetTileOffset()
  local attrs = GUI.tabbedTemplates.tabsList[ GUI.tabbedTemplates.currentTab ].items[ GUI.tabbedTemplates.currentItem ].attr
  local tags = GUI.tabbedTemplates.tabsList[ GUI.tabbedTemplates.currentTab ].items[ GUI.tabbedTemplates.currentItem ].tags
  if attrs.renderable ~= nil and attrs.renderable then
    attrs.renderableSize = tileSize..' '..tileSize
  end
  if attrs['collision'] ~= nil and attrs.collision then
    local size = tileSize
    if attrs['_collisionScale'] ~= nil then
      size = size * attrs['_collisionScale']
    end
    attrs['collisionSize'] = size..' '..size
  end

  if attrs['trigger'] ~= nil and attrs.trigger then -- init trigger
    local size = tileSize
    if attrs['_triggerPolygon'] ~= nil then  -- polygon: convert table to string
      if attrs['_triggerScale'] ~= nil then
        size = size * attrs['_triggerScale'] * 0.5
      end
      local str = ''
      for key, value in pairs( attrs['_triggerPolygon'] ) do
        str = str..( #str > 0 and ' ' or '' )..( value * size )
      end
      attrs['triggerPolygon'] = str
    else  -- default: square
      if attrs['_triggerScale'] ~= nil then
        size = size * attrs['_triggerScale']
        attrs['triggerSize'] = size..' '..size
      end
    end
  end
  --attrs.collisionSize = ( tileSize * _collisionScale )..' '..( tileSize * _collisionScale )
  --attrs.triggerSize = ( tileSize * _triggerScale )..' '..( tileSize * _triggerScale )

  local x, y = GetTilePosByPixel( px, py )
  -- local gridPos = GetGridByCoords( x * tileSize, y * tileSize )
  local name = 'wall.'..px..'.'..py..'.'..string.format( '%f', settings.timer )

  --ObjectRemove( name )
  ObjectCreate( name, x * tileSize + offsetX, y * tileSize + offsetY, 0 )

  -- remove bad attrs

  ObjectAttr( name, attrs )
  if attrs['_triggerFunc'] ~= nil and attrs.trigger ~= nil then
    ListenTrigger( name, attrs['_triggerFunc'] )
    -- attrs['_triggerFunc'] = nil
  end

  local currentItem = GUI.tabbedTemplates.tabsList[ GUI.tabbedTemplates.currentTab ].items[ GUI.tabbedTemplates.currentItem ]

  if currentItem.templateScriptName ~= nil then
    local obj = Object.Get( name )
    obj.api:SetScript( currentItem.templateScriptName )
  end

  if currentItem.creationScript ~= nil then
    currentItem.creationScript( name )
  end
  if type( currentItem.animation ) == 'table' then
    local actionAfterComplete = 'repeat'
    local animationAfterComplete = ''
    if currentItem.animation[ 3 ] ~= nil then
      actionAfterComplete = currentItem.animation[ 3 ]
      if currentItem.animation[ 4 ] ~= nil then
        animationAfterComplete = currentItem.animation[ 4 ]
      end
    end
    ObjectSetAnimation( name, currentItem.animation[ 1 ], currentItem.animation[ 2 ], actionAfterComplete, animationAfterComplete )
  end

  if tags ~= nil then
    for q = 1,#tags do
      ObjectAddTag( name, tags[ q ] )
    end
  end

  return Object.Get( name )
end --EditorInsertItemByTemplate

--[[ ToggleGrid ]]
function ToggleGrid()
  settings.showGrid = not settings.showGrid
  GUI.elements.showGrid:SetIsChecked( settings.showGrid )
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
  Object.Select( nil )
  Object.Get( 'editor' ):UpdateGuiBySelectedObject()
end -- PopFromBuffer

--[[ OnChangeIsRenderable ]]
function OnChangeIsRenderable( isRenderableGuiElement )
  local objectList = Object.GetSelected()
  local checked = isRenderableGuiElement:GetIsChecked()
  for _,object in pairs( objectList ) do --(
    ObjectAttr( object.api:GetNameFull(), { renderable = checked } )
  end
  if checked then
    local tileSize = GetTileSize()
    for _,object in pairs( objectList ) do --(
      ObjectAttr( object.api:GetNameFull(), { textureName = 'data/textures/null.png', renderableSize = tileSize..' '..tileSize } )
    end
  end
end -- OnChangeIsRenderable

--[[ OnChangeIsCollision ]]
function OnChangeIsCollision( isCollisionGuiElement )
  local objectList = Object.GetSelected()
  local checked = isCollisionGuiElement:GetIsChecked()
  for _,object in pairs( objectList ) do --(
    ObjectAttr( object.api:GetNameFull(), { collision = checked } )
  end
  if checked then
    local tileSize = GetTileSize()
    for _,object in pairs( objectList ) do --(
      ObjectAttr( object.api:GetNameFull(), { collisionSize = string.format( '%d %d', tileSize, tileSize ) } )
    end
  end
end -- OnChangeIsCollision

--[[ OnChangeIsCollision ]]
function OnChangeIsTrigger( isTriggerGuiElement )
  local objectList = Object.GetSelected()
  local checked = isTriggerGuiElement:GetIsChecked()
  for _,object in pairs( objectList ) do --(
    ObjectAttr( object.api:GetNameFull(), { trigger = checked } )
  end
  if checked then
    local tileSize = GetTileSize()
    for _,object in pairs( objectList ) do --(
      ObjectAttr( object.api:GetNameFull(), { triggerSize = string.format( '%d %d', tileSize, tileSize ) } )
    end
  end
end -- OnChangeIsCollision

--[[ OnChangeIsLightBlockByCollision ]]
function OnChangeIsLightBlockByCollision( isLightBlockByCollisionGuiElement )
  local objectList = Object.GetSelected()
  local checked = isLightBlockByCollisionGuiElement:GetIsChecked()
  for _,object in pairs( objectList ) do --(
    ObjectAttr( object.api:GetNameFull(), { lightBlockByCollision = checked } )
  end
end -- OnChangeIsLightBlockByCollision

--[[ OnChangeIsLightPoint ]]
function OnChangeIsLightPoint( isLightPointGuiElement )
  local objectList = Object.GetSelected()
  local checked = isLightPointGuiElement:GetIsChecked()
  for _,object in pairs( objectList ) do --(
    ObjectAttr( object.api:GetNameFull(), { lightPoint = checked } )
  end
  if checked then
    for _,object in pairs( objectList ) do --(
      ObjectAttr( object.api:GetNameFull(), { lightPointSize = 400, lightPointColor = ( math.random( 0, 1000 ) / 2000 + 0.5 )..' '..( math.random( 0, 1000 ) / 2000 + 0.5 )..' '..( math.random( 0, 1000 ) / 2000 + 0.5 )..' 1' } )
    end
  end
end -- OnChangeIsLightPoint

--[[ OnChangeZ ]]
function OnChangeZ( setZ )
  GUI.elements.objectZ:SetText( setZ )
  GUI.elements.objectZslider:SetText( setZ )
  local objectList = Object.GetSelected()
  for _,object in pairs( objectList ) do --(
    ObjectAttr( object.api:GetNameFull(), { z = tonumber( setZ ) } )
  end
end -- OnChangeZ

--[[ OnChangeLayer ]]
function OnChangeLayer( obj )
  local val = obj:GetValue()
  SetEditorMode( val - 1 )
  --[[
  if val > 1 then
    Debug.RenderState( math.pow( 2, val - 2 ) )
    settings.editorType = val - 1
  else
    Debug.RenderState( 0 )
    settings.editorType = 0
  end
  ]]
end -- OnChangeLayer

--[[ OnChangeTileSize ]]
function OnChangeTileSize( tileSizeGuiElement )
  local tileSize = tileSizeGuiElement:GetText()
  local objectList = Object.GetSelected()
  if TableSize( objectList ) > 0 then --(
    for _,object in pairs( objectList ) do
      if settings.editorType == 1 then
        ObjectAttr( object.api:GetNameFull(), { renderableSize = string.format( '%d %d', tileSize, tileSize ) } )
      elseif settings.editorType == 2 then
        ObjectAttr( object.api:GetNameFull(), { collisionSize = string.format( '%d %d', tileSize, tileSize ) } )
      elseif settings.editorType == 3 then
        ObjectAttr( object.api:GetNameFull(), { triggerSize = string.format( '%d %d', tileSize, tileSize ) } )
      end
    end
  end --)
end -- OnChangeTileSize

--( Выбор всех объектов в рамке в зависимости от режима редактора
function SelectObjectsInRectangle( left, top, right, bottom, editorType )
  if left > right then
    left, right = right, left
  end
  if top > bottom then
    top, bottom = bottom, top
  end
  local cx, cy = GetCameraPos()
  local width, height = GetWindowSize()
  width, height = width * 0.5, height * 0.5
  left, right, top, bottom = left - width, right - width, top - height, bottom - height

  local objects = Object.GetByRect( settings.editorType, left, top, right, bottom ) --GetObjects()

  if settings.keys.isCtrl then  -- add
    local oldObjects = Object.GetSelected()
    if( TableSize( oldObjects ) > 0 ) then
      for _,v in pairs( oldObjects ) do
        table.insert( objects, v )
      end
      -- objects = oldObject..'//'..objects
    end
  end
  if settings.keys.isAlt then --( exclude
    local oldObjects = Object.GetSelected()
    if( TableSize( oldObjects ) > 0 ) then
      objects = RemoveObjectsFromList( oldObject, objects )
    end
  end
  Object.Select( objects )
  return objects
end --)

--( Переключение режима редактора на default, renderable, collision, trigger
function SetEditorMode( setMode )
  DoPause( true )
  -- Debug.RenderState( setMode == 0 and 0 or math.pow( 2, setMode - 1 ) )
  settings.editorType = setMode
  local modeToText = {
    [0] = 'default',
    [1] = 'renderable',
    [2] = 'collision',
    [3] = 'trigger',
  }
  GUI.elements.layer:SetText( modeToText[ setMode ] )
  if setMode > 0 then
    Debug.RenderState( math.pow( 2, setMode - 1 ) )
    settings.editorType = setMode
  else
    Debug.RenderState( 0 )
    settings.editorType = 0
  end
  -- OnChangeLayer( GUI.elements.layer )
  -- settings.editorMode = 0
  -- SelectObject( '' )
  -- Object.Get( 'editor' ):UpdateGuiBySelectedObject()
end --)

--( Добавление объектов в список (для мульти-селекта)
function AddObjectsToList( list, objects )
  -- listTable = Explode( list, '//' )
  -- objectsList = Explode( objects, '//' )
  for _,obj in pairs( objects ) do
    local alreadyExist = false
    local objId = obj.api:GetId()
    for _,t in pairs( list ) do
      if objId == t.api:GetId() then
        alreadyExist = true
        break
      end
    end
    if not alreadyExist then
      -- list = list..( #list > 0 and '//' or '' )..obj
      table.insert( list, obj )
    end
  end
  return list
end --)

--( Добавление объектов в список (для мульти-селекта)
function RemoveObjectsFromList( list, objects )
  -- listTable = Explode( list, '//' )
  -- objectsList = Explode( objects, '//' )
  for _,obj in pairs( objects ) do
    local objId = obj.api:GetId()
    for kT,t in pairs( list ) do
      if objId == t.api:GetId() then
        table.remove( list, kT )
        break
      end
    end
  end
  return list
end --)

--(
function UpdateAmbientByGUI()
  local R, G, B, A = GUI.elements.ambientLightR:GetText(), GUI.elements.ambientLightG:GetText(), GUI.elements.ambientLightB:GetText(), GUI.elements.ambientLightA:GetText()
  SetLightAmbient( R, G, B, A )
end --)
