--[[

���������� ��������

settings.editorMode:
-- 0: none, default
-- 1: ����� ��� � �������, ����� �� ������� �����
-- 2: ������ ������
-- 3: ����������� ���������� �������
-- 10: mouseover �� GUI.templates
-- 11: ����������� GUI.templates
-- 12: ������ GUI.templates
-- 20: ������������� ����� ������� �����: ����� ���� � ��� ���������� ��� ��������
-- 21: ������� ����� ������: ��������� ������������� �������
-- 22: ������-������ (��������� �����)
-- 30: mouseover �� GUI.tabbedTemplates
-- 11: ����������� GUI.tabbedTemplates
-- 12: ������ GUI.tabbedTemplates
]]
settings = {
    guiVisibility     = false,  -- ����������� GUI

    objectMode3Moved  = false,  -- bool ���� ���������� ����������� ������� ������
    objectMode3StartPosition = { 0, 0 },  -- ��������� ��������� �������, �� �����������

    editorMode        = 0,      -- ������� ����� ���������
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
    },
    timer = 0.0,
    windowSize = { x = 0, y = 0 },
    showGrid = false,
    editorType = 0, -- 0 - ����� �����, 1 - �����������, 2 - ��������, 3 - ��������
    keys = {
      isShift = false,
      isCtrl  = false,
      isAlt   = false,
    },
    buffer = {},  -- ����� �������� ��� ������
    gridSize = GetGridSize(),
    gamePaused = false,
    cameraDirection = 0,  -- ����������� ������
    multiSelectStartPoint = { x = 0, y = 0 }, -- ��������� ����� �������������
    ambientLight = { R = 1.0, G = 1.0, B = 1.0, A = 0.2 },
}
mousePos = {    -- �������� ������� �������
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
      Render( 'sprite', GUI.templates.x, GUI.templates.y, GUI.templates.zIndex, GUI.templates.x + GUI.templates.width, GUI.templates.y + GUI.templates.height, GUI.templates.zIndex, 'data/temp/blank.png', '333333FF' )
      -- scroll bg
      Render( 'sprite', GUI.templates.x + GUI.templates.width - 15, GUI.templates.y, GUI.templates.zIndex, GUI.templates.x + GUI.templates.width, GUI.templates.y + GUI.templates.height, GUI.templates.zIndex, 'data/temp/blank.png', '555555FF' )
      -- scroll bar
      y = GUI.templates.y + 15 + GUI.templates.scroll*(GUI.templates.height-29)
      Render( 'sprite', GUI.templates.x + GUI.templates.width - 14, y + 1, GUI.templates.zIndex, GUI.templates.x + GUI.templates.width - 1, y + 13, GUI.templates.zIndex, 'data/temp/blank.png', '888888FF' )
      -- header
      Render( 'sprite', GUI.templates.x, GUI.templates.y, GUI.templates.zIndex, GUI.templates.x + GUI.templates.width, GUI.templates.y + 15, GUI.templates.zIndex, 'data/temp/blank.png', '999999FF' )
      -- items
      -- Render( 'scissorEnable', GUI.templates.x, settings.windowSize.y - ( GUI.templates.y + GUI.templates.height - 0 ), GUI.templates.x + GUI.templates.width - 15, settings.windowSize.y - 16 )
      Render( 'scissorEnable', GUI.templates.x + 1, settings.windowSize.y - ( GUI.templates.y + GUI.templates.height - 1 ), GUI.templates.x + GUI.templates.width - 17, settings.windowSize.y - ( GUI.templates.y + 17 ) )
      i = 0
      for key, item in pairs( GUI.templates.items ) do
        GUI.templates.DrawItem( key, item )
      end
      if GUI.templates.currentItem > 0 and GUI.templates.currentItem <= #GUI.templates.items then
        y = GUI.templates.y - GUI.templates.scroll * GUI.templates.maxScroll + ( GUI.templates.currentItem - 1 ) * ( GUI.templates.itemSize + 5 ) + 20
        Render( 'rect', GUI.templates.x + 5, y, GUI.templates.zIndex, GUI.templates.x + 5 + GUI.templates.itemSize, y + GUI.templates.itemSize, GUI.templates.zIndex, '00ff00ff' )
      end
      Render( 'scissorDisable' )
    end,
    DrawItem = function( i, item )
      y = GUI.templates.y - GUI.templates.scroll * GUI.templates.maxScroll + ( i - 1 ) * ( GUI.templates.itemSize + 5 ) + 20
      Render( 'sprite', GUI.templates.x + 5, y, GUI.templates.zIndex, GUI.templates.x + 5 + GUI.templates.itemSize, y + GUI.templates.itemSize, GUI.templates.zIndex, item.icon, 'ffffffff' )
    end,
  }, -- templates

  tabbedTemplates = {
    tabsList = {
      {
        id = 'blocks',
        title = '�����',
        items = {},
      },
      {
        id = 'grass',
        title = '�����',
        items = {},
      },
      {
        id = 'sky',
        title = '����',
        items = {},
      },
      {
        id = 'light',
        title = '����',
        items = {},
      },
      {
        id = 'go',
        title = '�������',
        items = {},
      },
      {
        id = 'npc',
        title = 'NPC',
        items = {},
      },
      {
        id = 'other',
        title = '������',
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
        local width = Render( 'getTextWidth', tab.title ) + 4
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
      Render( 'sprite', x, y, z, x + width, y + height, z, 'data/temp/blank.png', 'EDEDEDFF' )
      Render( 'rect', x, y, z, x + width, y + height, z, 'A9A9A9FF' )
      -- title
      Render( 'sprite', x + 1, y + 2, z, x + width - 2, y + 10, z, 'data/temp/blank.png', GUI.tabbedTemplates.isTitleHovered and 'ffaaaaff' or 'ffaaaa77' )
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
          Render( 'sprite', tx, ty, tz, tx + GUI.tabbedTemplates.tileSize - 1, ty + GUI.tabbedTemplates.tileSize, tz, 'data/temp/blank.png', '000000ff' )
          Render( 'sprite', tx, ty, tz, tx + GUI.tabbedTemplates.tileSize, ty + GUI.tabbedTemplates.tileSize, tz, item.icon, 'ffffffff' )
          Render( 'rect', tx, ty, tz, tx + GUI.tabbedTemplates.tileSize, ty + GUI.tabbedTemplates.tileSize, tz, borderColor )
        end
      end
    end,  --) Draw
    DrawTab = function( id, tab, dx, dy ) --(
      local x, y, z = dx + tab.offset + 2, dy + 2, GUI.tabbedTemplates.zIndex
      local isCurrent = GUI.tabbedTemplates.currentTab == id
      local
        colorBG, colorBGbottom, colorBorder =
        ( isCurrent and 'ffaaaaff' or ( tab.isHovered and 'ffeeffff' or 'ffffffff' ) ), ( isCurrent and 'ff8888ff' or ( tab.isHovered and 'ffccccff' or 'ffffffff' ) ), 'A9A9A9FF'
      Render( 'sprite', x, y, z, x + tab.width, y + 8, z, 'data/temp/blank.png', colorBG )
      Render( 'sprite', x, y + 8, z, x + tab.width, y + 15, z, 'data/temp/blank.png', colorBGbottom )
      Render( 'rect', x, y, z, x + tab.width, y + 15, z, colorBorder )
      Render( 'text', x + 2, y + 2, z, tab.title, '000000ff' )
    end,  --) DrawTab
    OnMouseMove = function( x, y )  --(
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
      if GUI.tabbedTemplates.isHovered then
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
        end
        if hovered > 0 then
          GUI.tooltip:SetPosition( x + 1, y - 25 )
          GUI.tooltip:SetText( GUI.tabbedTemplates.tabsList[ GUI.tabbedTemplates.currentTab ].items[ hovered ].name )
          GUI.tooltip:CropByTextWidth()
        else
          GUI.tooltip:SetPosition( 0, settings.windowSize.y )
        end
      end
      if GUI.tabbedTemplates.isHovered then
        if settings.editorMode == 0 then
          settings.editorMode = 30
        end
      else
        if settings.editorMode == 30 then
          settings.editorMode = 0
        end
      end
      for num,tab in pairs( GUI.tabbedTemplates.tabsList ) do --(
        tab.isHovered = 
          x >= GUI.tabbedTemplates.position.x + tab.offset
          and x <= GUI.tabbedTemplates.position.x + tab.offset + tab.width
          and y >= GUI.tabbedTemplates.position.y + 10
          and y <= GUI.tabbedTemplates.position.y + 28
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
        if settings.editorMode == 31 then
          if GUI.tabbedTemplates.moving.mousePos.x == mousePos.x and GUI.tabbedTemplates.moving.mousePos.y == mousePos.y then
            GUI.tabbedTemplates.isMinimized = not GUI.tabbedTemplates.isMinimized
          end
          settings.editorMode = 30
        else
          settings.editorMode = 30
        end
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
        Render( 'line', sx + x, 0, GUI.grid.zIndex, sx + x, settings.windowSize.y, GUI.grid.zIndex, '00FF0044' )
      end
      for y = 0, settings.windowSize.y, tileSize do
        Render( 'line', 0, sy + y, GUI.grid.zIndex, settings.windowSize.x, sy + y, GUI.grid.zIndex, '00FF0044' )
      end
      for gx = 0,1 do
        local x = ( math.floor( cameraX / settings.gridSize ) + gx ) * settings.gridSize - cameraX + settings.windowSize.x * 0.5
        Render( 'line', x, 0, GUI.grid.zIndex, x, settings.windowSize.y, GUI.grid.zIndex, 'FF0000FF' )
      end
      for gy = 0,1 do
        local y = ( math.floor( cameraY / settings.gridSize ) + gy ) * settings.gridSize - cameraY + settings.windowSize.y * 0.5
        Render( 'line', 0, y, GUI.grid.zIndex, settings.windowSize.x, y, GUI.grid.zIndex, 'FF0000FF' )
      end
    end,
  }, -- grid
}


-- �������������
function EditorInit()
  LoadScript( 'data/scripts/tools.lua' )
  LoadScript( 'data/scripts/objecttemplates.lua' )
  math.randomseed( GetRandomSeed() )
  local x,y = GetWindowSize()
  settings.windowSize.x = x
  settings.windowSize.y = y

  -- ����������� ���� � ���������
  -- GUI.templates.height = settings.windowSize.y - 10
  -- GUI.templates.items = GetObjectsTemplates()
  -- GUI.templates.width = GUI.templates.itemSize + 25
  GUI.tabbedTemplates.Init()

  -- ������ ����������� �� ��: �����, ������ � �������� ����
  -- ListenKeyboard( 'OnEditorKey' )
  -- ListenMouseKey( 'OnEditorMouseKey' )
  -- ListenMouseMove( 'OnEditorMouseMove' )
  GUIRenderer.OnClickDefault = OnEditorMouseKey
  GUIRenderer.OnMouseMoveDefault = OnEditorMouseMove
  GUIRenderer.OnKeyboardDefault = OnEditorKey

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
  GUI.elements.objectName   = GUIEdit:Create( 50, y, 165, '', function( obj ) Alert( obj:GetText() ) end, GUI.elements.windowObject )
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

  -- GUIEdit:Create( 200, 50, 100, 'test123', function() Alert(2) end )

  -- ������ ����������� ��������� � ������ ���������
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

  -- ���������� GUI
  settings.guiVisibility = true
  --SetGuiVisibility( settings.guiVisibility )

  UpdateGuiBySelectedObject()

  EditorUpdateDebug( 0 )
  RenderGUI( 0 )
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
  SetTimer( 0.01, 'UpdateEditorCamera', true )
end -- UpdateEditorCamera

--( ��������� ������ �����
function OnEditorKey( id, isPressed )
  if settings.gamePaused then
    if id == 0x26 or id == 0x57 then  -- Up
      if isPressed then --(
        settings.cameraDirection = bit32.bor( settings.cameraDirection, 8 )
      else --) (
        settings.cameraDirection = bit32.band( settings.cameraDirection, bit32.bnot( 8 ) )
      end --)
    end
    if id == 0x25 or id == 0x41 then  -- Left
      if isPressed then --(
        settings.cameraDirection = bit32.bor( settings.cameraDirection, 4 )
      else --) (
        settings.cameraDirection = bit32.band( settings.cameraDirection, bit32.bnot( 4 ) )
      end --)
    end
    if id == 0x27 or id == 0x44 then  -- Right
      if isPressed then --(
        settings.cameraDirection = bit32.bor( settings.cameraDirection, 1 )
      else --) (
        settings.cameraDirection = bit32.band( settings.cameraDirection, bit32.bnot( 1 ) )
      end --)
    end
    if id == 0x28 or id == 0x53 then  -- Down
      if isPressed then --(
        settings.cameraDirection = bit32.bor( settings.cameraDirection, 2 )
      else --) (
        settings.cameraDirection = bit32.band( settings.cameraDirection, bit32.bnot( 2 ) )
      end --)
    end
  end
  if isPressed then
      if id == 0x1B then    -- Esc
        GameExit()
      end
      if id == 0x5A then    -- Z
        if settings.keys.isCtrl then  -- Ctrl+Z
          PopFromBuffer()
        else
          settings.guiVisibility = not settings.guiVisibility
          --SetGuiVisibility( settings.guiVisibility )
        end
      end
      if id == 0xC0 then    -- ~
        SetEditorMode( 0 )
      end
      if id == 0x31 then    -- 1
        SetEditorMode( 1 )
      end
      if id == 0x32 then    -- 2
        SetEditorMode( 2 )
      end
      if id == 0x33 then    -- 3
        SetEditorMode( 3 )
      end
      if id == 0x2E then    -- Del
        local object = GetSelectedObject() -- GuiGetText( 'editor/object.object_name' )
        local objectList = Explode( object, '//' )
        for num,name in pairs( objectList ) do --(
          -- buffer
          local
            x, y, z
            , isRenderable, isCollision, isTrigger, isLightBlockByCollision, isLightPoint
            , setTextureName, renderableSizeX, renderableSizeY, renderablePositionX, renderablePositionY, colorR, colorG, colorB, colorA, renderableScaleX, renderableScaleY, setRenderableRotation
            , collisionSizeX, collisionSizeY, collisionAccelerationX, collisionAccelerationY
            =
            ObjectAttr( name, {
              'position'
              , 'renderable', 'collision', 'trigger', 'lightBlockByCollision', 'lightPoint'
              , 'textureName', 'renderableSize', 'renderablePosition', 'color', 'renderableScale', 'renderableRotation'
              , 'collisionSize', 'collisionAcceleration'
            } )
          local
            triggerType
            , setCollisionStatic, collisionForceX, collisionForceY, collisionVelocityX, collisionVelocityY
            =
            ObjectAttr( name, {
              'triggerType'
              , 'collisionStatic', 'collisionForce', 'collisionVelocity'
            } )
          local setTriggerPolygon = ''
          if triggerType == "polygon" then
            setTriggerPolygon = ObjectAttr( name, { 'triggerPolygon' } )
          end
          PushToBuffer( function()
            end, function()
              ObjectCreate( name, x, y, z )
              ObjectAttr( name, { renderable = isRenderable, collision = isCollision, trigger = isTrigger, lightBlockByCollision = isLightBlockByCollision, lightPoint = isLightPoint } )
              if isRenderable then
                ObjectAttr( name, {
                  textureName = setTextureName, renderableSize = renderableSizeX..' '..renderableSizeY, renderablePosition = renderablePositionX..' '..renderablePositionY
                  , color = colorR..' '..colorG..' '..colorB..' '..colorA, renderableScale = renderableScaleX..' '..renderableScaleY, renderableRotation = setRenderableRotation
                } )
              end
              if isCollision then
                ObjectAttr( name, {
                  collisionSize = collisionSizeX..' '..collisionSizeY, collisionAcceleration = collisionAccelerationX..' '..collisionAccelerationY
                  -- , collisionVelocity = collisionVelocityX..' '..collisionVelocityY --, collisionStatic = setCollisionStatic, collisionForce = collisionForceX..' '..collisionForceY
                } )
              end
              if isTrigger then
                if triggerType == "polygon" then
                  ObjectAttr( name, { triggerPolygon = setTriggerPolygon } )
                end
              end
            end
          )
          --
          ObjectRemove( name )
        end --)
        SelectObject( '' )
        if settings.editorMode == 2 then
          settings.editorMode = 0
        end
        UpdateGuiBySelectedObject()
      end
      if id == 0x20 then    -- Space
        SelectObject( '' )
        UpdateGuiBySelectedObject()
        if settings.editorMode == 2 then
          settings.editorMode = 0
        end
        -- GUI.templates.currentItem = 0
        GUI.tabbedTemplates.currentItem = 0
      end
      --[[
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
      ]]
      if id == 0x47 then    -- G: toggle showing grid
        ToggleGrid()
      end
      if id == 0x0D then    -- Enter
        DoPause( not settings.gamePaused )
      end
      if id == 0x72 then    -- F3
        settings.gamePaused = true
        settings.editorMode = 0
        settings.buffer = {}
        settings.keys.isShift = false
        settings.keys.isCtrl = false
        settings.keys.isAlt = false
      end
  end
  if id == 0x10 then    -- Shift
    settings.keys.isShift = isPressed
  end
  if id == 0x11 then    -- Control
    settings.keys.isCtrl = isPressed
  end
  if id == 0x12 then    -- Alt
    settings.keys.isAlt = isPressed
  end
end --) OnEditorKey

--( ������������ ������ �����(���������) � ����
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
    DebugRender( 0 )
    settings.editorType = 0
    GUI.elements.layer:SetText( 'default' )
    -- OnChangeLayer( GUI.elements.layer )
  end
end --)

--( ��������� ������ ����
function OnEditorMouseKey( id, isPressed )
  -- if not settings.gamePaused then
  --   return false
  -- end
  local mode = {
    [0] = function() --(
      if isPressed then --(
        local doMultiSelect = false
        if settings.editorType == 0 then --(
          -- if GUI.templates.currentItem > 0 then --(
          if GUI.tabbedTemplates.currentItem > 0 then --(
            settings.editorMode = 20
            settings.move.mouseStart.x = mousePos.x
            settings.move.mouseStart.y = mousePos.y
            SelectObject( '' )
            UpdateGuiBySelectedObject()
          else --)(
            doMultiSelect = true
          end --)
          --[[
        else
          local object = GetObjectUnderCursorByMode()
          if #object > 0 then
              SelectObject( object )
              UpdateGuiBySelectedObject()
              settings.editorMode = 1
          else
            settings.editorMode = 1
              -- multiselect?
          end
          ]]
        else --)(
          doMultiSelect = true
        end --)
        if doMultiSelect then
          settings.editorMode = 22
          local cx, cy = GetCameraPos()
          settings.multiSelectStartPoint.x = mousePos.x + cx
          settings.multiSelectStartPoint.y = mousePos.y + cy
          LogWrite( 'start multiselect' )
        end
      else --)( released
      end --)
    end, --)
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
        if settings.keys.isCtrl --[[ or settings.keys.isAlt ]] then --(
          settings.editorMode = 22
          local cx, cy = GetCameraPos()
          settings.multiSelectStartPoint.x = mousePos.x + cx
          settings.multiSelectStartPoint.y = mousePos.y + cy
        else --)(
          settings.editorMode = 3
          settings.objectMode3Moved = false
        end --)
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
          --[[
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
          ]]
        else
          local object = GetObjectUnderCursorByMode()
          LogWrite( 'GetObjectUnderCursorByMode => '..object )
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
            DoPause( true )
            GUI.templates.currentItem = num
            -- DebugRender( 0 )
            settings.editorType = 0
            -- SelectObject( '' )
            -- UpdateGuiBySelectedObject()
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
      SelectObject( name )
      UpdateGuiBySelectedObject()
      PushToBuffer( function()
        end, function()
          ObjectRemove( name )
        end
      )
    end,
    [21] = function() --(
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
        left, top = GetTilePosByPixel( left, top )
        right, bottom = GetTilePosByPixel( right, bottom )
        -- local texture = GUI.tabbedTemplates.tabsList[ GUI.tabbedTemplates.currentTab ].items[ GUI.tabbedTemplates.currentItem ].icon
        local names = {}
        for tx = left, right do
        for ty = top, bottom do
          x, y = GetPixelByTile( tx, ty )
          names[ #names + 1 ] = EditorInsertItemByTemplate( x, y )
        end
        end
        SelectObject( Implode( names, '//' ) )
        UpdateGuiBySelectedObject()
        PushToBuffer( function()
          end, function()
            for q = 1,#names do
              ObjectRemove( names[ q ] )
            end
          end
        )
      end
    end, --) 21
    [22] = function() --(
      if isPressed then --(
      else --)(
        local cx, cy = GetCameraPos()
        local newX = mousePos.x + cx
        local newY = mousePos.y + cy
        if settings.multiSelectStartPoint.x == newX and settings.multiSelectStartPoint.y == newY then --( one-select
          local object = GetObjectUnderCursorByMode()
          if #object > 0 then --(
            if settings.keys.isCtrl then  --( add
              local oldObject = GetSelectedObject()
              if( #oldObject > 0 ) then
                object = AddObjectsToList( oldObject, object )
              end
            end --)
            if settings.keys.isAlt then --( exclude
              local oldObject = GetSelectedObject()
              if( #oldObject > 0 ) then
                object = RemoveObjectsFromList( oldObject, object )
              end
            end --)
            SelectObject( object )
            settings.editorMode = 2
          else --)(
            settings.editorMode = 0
          end --)
        else --)( multi-select
          local objects = SelectObjectsInRectangle( settings.multiSelectStartPoint.x, settings.multiSelectStartPoint.y, newX, newY, settings.editorType )
          if #objects > 0 then
            settings.editorMode = 2
          else
            settings.editorMode = 0
          end
        end --)
        UpdateGuiBySelectedObject()
      end --)
    end, --) 22
    [30] = function() --(
      GUI.tabbedTemplates.OnMouseClick( id, isPressed )
    end,  --) 30
    [31] = function() --(
      GUI.tabbedTemplates.OnMouseClick( id, isPressed )
    end,  --) 31
    [32] = function() --(
      GUI.tabbedTemplates.OnMouseClick( id, isPressed )
    end,  --) 32
  }
  mode[ settings.editorMode ]()
end --) OnEditorMouseKey

-- ��������� �������� ����
function OnEditorMouseMove( x, y )  --(
  --[[
  if TestMouseOnGUI( x, y ) then
    local num = math.floor( ( mousePos.y - GUI.templates.y - 15 + GUI.templates.scroll * GUI.templates.maxScroll ) / ( GUI.templates.itemSize + 5 ) ) + 1
    if num > 0 and num <= #GUI.templates.items then
      GUI.tooltip:SetPosition( x + 1, y - 25 )
      GUI.tooltip:SetText( GUI.templates.items[ num ].name )
      GUI.tooltip:CropByTextWidth()
    else
      GUI.tooltip:SetPosition( 0, settings.windowSize.y )
    end
  else ]]
  if GUI.tabbedTemplates.isHovered then
  else
    GUI.tooltip:SetPosition( 0, settings.windowSize.y )
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
        -- local oldX, oldY = ObjectGetPos( object )
        if not settings.objectMode3Moved then
          -- settings.move.objectStart.x = oldX
          -- settings.move.objectStart.y = oldY
          settings.move.mouseStart.x = mousePos.x
          settings.move.mouseStart.y = mousePos.y
          settings.move.lastOffset.x = 0
          settings.move.lastOffset.y = 0
        end
        -- local newX = settings.move.objectStart.x + x - settings.move.mouseStart.x
        -- local newY = settings.move.objectStart.y + y - settings.move.mouseStart.y
        local tileSize = tonumber( GetTileSize() )
        local offsetX, offsetY = GetTileOffset()
        objectList = Explode( object, '//' )
        local dx, dy = math.floor( ( mousePos.x - settings.move.mouseStart.x ) / tileSize ) * tileSize, math.floor( ( mousePos.y - settings.move.mouseStart.y ) / tileSize ) * tileSize
        if settings.keys.isAlt then
          dx, dy = mousePos.x - settings.move.mouseStart.x, mousePos.y - settings.move.mouseStart.y
        end
        local moveByX, moveByY = -settings.move.lastOffset.x + dx, -settings.move.lastOffset.y + dy
        settings.move.lastOffset.x = settings.move.lastOffset.x + moveByX
        settings.move.lastOffset.y = settings.move.lastOffset.y + moveByY
        LogWrite( settings.move.lastOffset.x..':'..settings.move.lastOffset.y..' => '..moveByX..':'..moveByY )
        for num,name in pairs( objectList ) do
          local x, y = ObjectGetPos( name )
          ObjectSetPos( name, x + moveByX, y + moveByY )
          -- newX = math.floor( newX / tileSize ) * tileSize + offsetX
          -- newY = math.floor( newY / tileSize ) * tileSize + offsetY
        end
        -- buffer
        if moveByX ~= 0 or moveByY ~= 0 then
          PushToBuffer( function()
            end, function()
              for num,name in pairs( objectList ) do
                local x, y = ObjectGetPos( name )
                ObjectSetPos( name, x - moveByX, y - moveByY )
              end
            end
          )
        end
        --
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
      [22] = function()
      end,
    }
    mode[ settings.editorMode ]()
  end
  GUI.tabbedTemplates.OnMouseMove( x, y )
  mousePos.x = x
  mousePos.y = y
end --) OnEditorMouseMove


-- ���������� ������ ��� ��������
-- ���� ��� ������ �����-�� ������, �� ������ ��������� �� ���
function GetObjectUnderCursorByMode()
    local mx, my = GetMousePos()
    local cx, cy = GetCameraPos()
    local width, height = GetWindowSize()
    local pos = {
      x = cx + mx - width * 0.5,
      y = cy + my - height * 0.5
    }
    -- local layer = 'default' -- GuiGetText( 'editor/settings.layer' )
    local name = GetSelectedObject()
    if settings.editorType == 1 then
      name = GetObjectByPoint( 1, pos.x, pos.y, name )
      LogWrite( string.format( 'GetObjectByPoint => [1][%3.1f][%3.1f]["%s"]', pos.x, pos.y, name ) )
    end
    if settings.editorType == 2 then
      name = GetObjectByPoint( 2, pos.x, pos.y, name )
    end
    if settings.editorType == 3 then
      name = GetObjectByPoint( 3, pos.x, pos.y, name )
    end
    return name
end --GetObjectUnderCursorByMode

-- ������� "Renderable"
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

-- ������� "Collision"
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

-- ������� "Collision:Static"
function ToggleStatic( guiName )
  local name = GetSelectedObject()
  if #name < 1 then
    return
  end
  local checked = GuiGetChecked( 'editor/object.is_static' )
  ObjectAttr( name, { collisionStatic = checked } )
end --ToggleStatic

-- ������� "Trigger"
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

-- ���������� ������� ������ ������ (�� ���������)
function GetTileSize()
  return GUI.elements.gridSize:GetText()
end --GetTileSize

-- ���������� ������� �������� ����� (�� ���������)
function GetTileOffset()
  return GUI.elements.gridOffsetX:GetText(), GUI.elements.gridOffsetY:GetText()
end --GetTileSize

-- ������ "Apply"
-- TODO: ����������
--[[
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
]]

-- �������� "Layer"
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
  DebugRender( flags )
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
  GUI.elements.labelDebug:SetText( string.format( 'grid[%d; %d] pixel[%d; %d] mode[%d]', math.floor( cx / settings.gridSize ), math.floor( cy / settings.gridSize ), math.floor( pos.x ), math.floor( pos.y ), settings.editorMode ) )
  -- GuiSetText( 'editor/debug', settings.editorMode..':buffer['..#settings.buffer..']' )
  SetTimer( 0.01, 'EditorUpdateDebug', true )
end -- EditorUpdateDebug

function UpdateGuiBySelectedObject()
  local object = GetSelectedObject()
  local objectList = Explode( object, '//' )
  local doReset = false
  if( #objectList == 1 ) then --( one object
    if #object < 1 then
      doReset = true
    else
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
    for num,name in pairs( objectList ) do
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
  Render( 'clrscr' )
  if settings.showGrid then
    GUI.grid.Render()
  end
  --( ������� ����� ������
  if settings.editorMode == 21 then
    local cx, cy = GetCameraPos()
    local x = settings.move.mouseStart.x - cx + settings.windowSize.x * 0.5
    local y = settings.move.mouseStart.y - cy + settings.windowSize.y * 0.5
    local newX = mousePos.x
    local newY = mousePos.y
    Render( 'rect', x, y, 8.0, newX, newY, 8.0, '00ff00ff' )
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
      Render( 'sprite', x, y, 8.0, x + tileSize, y + tileSize, 8.0, texture, 'ffffff44' )
    end
    end
  end --)
  --( ����� ����� ��� ��������
  if settings.editorMode == 0 and settings.gamePaused and GUI.tabbedTemplates.currentItem > 0 then
    x, y = GetTilePosByPixel( mousePos.x, mousePos.y )
    left, top = GetPixelByTile( x, y )
    right, bottom = GetPixelByTile( x + 1, y + 1 )
    Render( 'rect', left, top, 8.0, right, bottom, 8.0, 'ff0000ff' )
  end --)

  --( ����� �������������
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
    Render( 'rect', left, top, 8.0, right, bottom, 8.0, 'ff0000ff' )
  end --)

  if settings.guiVisibility then
    -- GUI.templates.Draw()
    GUI.tabbedTemplates.Draw()
    GUIRendererRender( 0 ) -- additional GUI from gui.lua
  end
  SetTimer( 1/30, 'RenderGUI', true )
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
    return ''
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

  return name
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
  SelectObject( '' )
  UpdateGuiBySelectedObject()
end -- PopFromBuffer

--[[ OnChangeIsRenderable ]]
function OnChangeIsRenderable( isRenderableGuiElement )
  local object = GetSelectedObject()
  local checked = isRenderableGuiElement:GetIsChecked()
  local objectList = Explode( object, '//' )
  for num,name in pairs( objectList ) do --(
    ObjectAttr( name, { renderable = checked } )
  end
  if checked then
    local tileSize = GetTileSize()
    for num,name in pairs( objectList ) do --(
      ObjectAttr( name, { textureName = 'data/textures/null.png', renderableSize = tileSize..' '..tileSize } )
    end
  end
end -- OnChangeIsRenderable

--[[ OnChangeIsCollision ]]
function OnChangeIsCollision( isCollisionGuiElement )
  local object = GetSelectedObject()
  local checked = isCollisionGuiElement:GetIsChecked()
  local objectList = Explode( object, '//' )
  for num,name in pairs( objectList ) do --(
    ObjectAttr( name, { collision = checked } )
  end
  if checked then
    local tileSize = GetTileSize()
    for num,name in pairs( objectList ) do --(
      ObjectAttr( name, { collisionSize = string.format( '%d %d', tileSize, tileSize ) } )
    end
  end
end -- OnChangeIsCollision

--[[ OnChangeIsCollision ]]
function OnChangeIsTrigger( isTriggerGuiElement )
  local object = GetSelectedObject()
  local checked = isTriggerGuiElement:GetIsChecked()
  local objectList = Explode( object, '//' )
  for num,name in pairs( objectList ) do --(
    ObjectAttr( name, { trigger = checked } )
  end
  if checked then
    local tileSize = GetTileSize()
    for num,name in pairs( objectList ) do --(
      ObjectAttr( name, { triggerSize = string.format( '%d %d', tileSize, tileSize ) } )
    end
  end
end -- OnChangeIsCollision

--[[ OnChangeIsLightBlockByCollision ]]
function OnChangeIsLightBlockByCollision( isLightBlockByCollisionGuiElement )
  local object = GetSelectedObject()
  local checked = isLightBlockByCollisionGuiElement:GetIsChecked()
  local objectList = Explode( object, '//' )
  for num,name in pairs( objectList ) do --(
    ObjectAttr( name, { lightBlockByCollision = checked } )
  end
end -- OnChangeIsLightBlockByCollision

--[[ OnChangeIsLightPoint ]]
function OnChangeIsLightPoint( isLightPointGuiElement )
  local object = GetSelectedObject()
  local checked = isLightPointGuiElement:GetIsChecked()
  local objectList = Explode( object, '//' )
  for num,name in pairs( objectList ) do --(
    ObjectAttr( name, { lightPoint = checked } )
  end
  if checked then
    for num,name in pairs( objectList ) do --(
      ObjectAttr( name, { lightPointSize = 400, lightPointColor = ( math.random( 0, 1000 ) / 2000 + 0.5 )..' '..( math.random( 0, 1000 ) / 2000 + 0.5 )..' '..( math.random( 0, 1000 ) / 2000 + 0.5 )..' 1' } )
    end
  end
end -- OnChangeIsLightPoint

--[[ OnChangeZ ]]
function OnChangeZ( setZ )
  GUI.elements.objectZ:SetText( setZ )
  GUI.elements.objectZslider:SetText( setZ )
  local object = GetSelectedObject()
  local objectList = Explode( object, '//' )
  for num,name in pairs( objectList ) do --(
    ObjectAttr( name, { z = tonumber( setZ ) } )
  end
end -- OnChangeZ

--[[ OnChangeLayer ]]
function OnChangeLayer( obj )
  local val = obj:GetValue()
  SetEditorMode( val - 1 )
  --[[
  if val > 1 then
    DebugRender( math.pow( 2, val - 2 ) )
    settings.editorType = val - 1
  else
    DebugRender( 0 )
    settings.editorType = 0
  end
  ]]
end -- OnChangeLayer

--[[ OnChangeTileSize ]]
function OnChangeTileSize( tileSizeGuiElement )
  local tileSize = tileSizeGuiElement:GetText()
  local object = GetSelectedObject()
  if #object > 0 then --(
    if settings.editorType == 1 then
      ObjectAttr( object, { renderableSize = string.format( '%d %d', tileSize, tileSize ) } )
    elseif settings.editorType == 2 then
      ObjectAttr( object, { collisionSize = string.format( '%d %d', tileSize, tileSize ) } )
    elseif settings.editorType == 3 then
      ObjectAttr( object, { triggerSize = string.format( '%d %d', tileSize, tileSize ) } )
    end
  end --)
end -- OnChangeTileSize

--( ����� ���� �������� � ����� � ����������� �� ������ ���������
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

  local objects = GetObjectByRect( settings.editorType, left, top, right, bottom ) --GetObjects()
  if settings.keys.isCtrl then  -- add
    local oldObject = GetSelectedObject()
    if( #oldObject > 0 ) then
      objects = oldObject..'//'..objects
    end
  end
  if settings.keys.isAlt then --( exclude
    local oldObject = GetSelectedObject()
    if( #oldObject > 0 ) then
      objects = RemoveObjectsFromList( oldObject, objects )
    end
  end
  SelectObject( objects )
  -- LogWrite( left..':'..top..' => '..right..':'..bottom )
  return objects
end --)

--( ������������ ������ ��������� �� default, renderable, collision, trigger
function SetEditorMode( setMode )
  DoPause( true )
  -- DebugRender( setMode == 0 and 0 or math.pow( 2, setMode - 1 ) )
  settings.editorType = setMode
  local modeToText = {
    [0] = 'default',
    [1] = 'renderable',
    [2] = 'collision',
    [3] = 'trigger',
  }
  GUI.elements.layer:SetText( modeToText[ setMode ] )
  if setMode > 0 then
    DebugRender( math.pow( 2, setMode - 1 ) )
    settings.editorType = setMode
  else
    DebugRender( 0 )
    settings.editorType = 0
  end
  -- OnChangeLayer( GUI.elements.layer )
  -- settings.editorMode = 0
  -- SelectObject( '' )
  -- UpdateGuiBySelectedObject()
end --)

--( ���������� �������� � ������ (��� ������-�������)
function AddObjectsToList( list, objects )
  listTable = Explode( list, '//' )
  objectsList = Explode( objects, '//' )
  for kObj,obj in pairs( objectsList ) do
    local alreadyExist = false
    for kT,t in pairs( listTable ) do
      if t == obj then
        alreadyExist = true
        break
      end
    end
    if not alreadyExist then
      list = list..( #list > 0 and '//' or '' )..obj
    end
  end
  return list
end --)

--( ���������� �������� � ������ (��� ������-�������)
function RemoveObjectsFromList( list, objects )
  listTable = Explode( list, '//' )
  objectsList = Explode( objects, '//' )
  for kObj,obj in pairs( objectsList ) do
    for kT,t in pairs( listTable ) do
      if t == obj then
        table.remove( listTable, kT )
        break
      end
    end
  end
  list = Implode( listTable, '//' )
  return list
end --)

--(
function UpdateAmbientByGUI()
  local R, G, B, A = GUI.elements.ambientLightR:GetText(), GUI.elements.ambientLightG:GetText(), GUI.elements.ambientLightB:GetText(), GUI.elements.ambientLightA:GetText()
  SetLightAmbient( R, G, B, A )
end --)
