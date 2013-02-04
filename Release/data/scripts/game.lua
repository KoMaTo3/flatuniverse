-- 0: none
-- 1: drag object
editorMode = 0
settings = {
    guiVisibility = false
}

function Init()
  math.randomseed( GetRandomSeed() )
  ListenKeyboard( 'onKey' )
  ListenMouseKey( 'onMouseKey' )
  GuiAddTrigger( 'editor/settings.layer', 'ToggleLayer' )
  GuiAddTrigger( 'editor/object.is_renderable', 'ToggleRenderable' )
  GuiAddTrigger( 'editor/object.is_collision', 'ToggleCollision' )
  GuiAddTrigger( 'editor/object.is_trigger', 'ToggleTrigger' )
  GuiAddTrigger( 'editor/object.is_static', 'ToggleStatic' )
  GuiAddTrigger( 'editor/object.tile_add', 'TileAdd' )
  settings.guiVisibility = false
  SetGuiVisibility( settings.guiVisibility )
end
Init()

function onKey( id, isPressed )
  if isPressed then
      if id == 0x1B then
        GameExit()
      end
      if id == 0x5A then
        settings.guiVisibility = not settings.guiVisibility
        SetGuiVisibility( settings.guiVisibility )
      end
  end
end

function onMouseKey( id, isPressed )
  local mode = {
    [0] = function()
      if isPressed then
        local mx, my = GetMousePos()
        local cx, cy = GetCameraPos()
        local width, height = GetWindowSize()
        local pos = {
          x = cx + mx - width * 0.5,
          y = cy + my - height * 0.5
        }
        local layer = GuiGetText( 'editor/settings.layer' )
        local name = ''
        if layer == 'renderable' then
          name = GetObjectByPoint( 1, pos.x, pos.y )
        end
        if layer == 'collision' then
          name = GetObjectByPoint( 2, pos.x, pos.y )
        end
        if layer == 'trigger' then
          name = GetObjectByPoint( 3, pos.x, pos.y )
        end
        GuiSetText( 'editor/object.object_name', name )
      else
      end
    end,
    [1] = function ()
      if isPressed then
      else
        editorMode = 0
      end
    end,
  }
  mode[ editorMode ]()
  --[[
  if isPressed then
    if id == 1 then
        local mx, my = GetMousePos()
        local cx, cy = GetCameraPos()
        local width, height = GetWindowSize()
        local pos = {
          x = cx + mx - width * 0.5,
          y = cy + my - height * 0.5
        }
        pos.x = math.floor( pos.x / tileSize ) * tileSize + 0.5 * tileSize
        pos.y = math.floor( pos.y / tileSize ) * tileSize + 0.5 * tileSize
        -- ObjectCreate( 'wall', '', pos.x, pos.y, 0, tileSize, tileSize, 'data/temp/brick0.png' )
        while ObjectRemove( 'wall' ) do
        end
        local tileSize = GetTileSize()
        ObjectCreate( 'wall', pos.x, pos.y, 0 )
        ObjectRenderable( true, 'wall', 'data/temp/alik16.bmp', tileSize,tileSize, 1,1,1,1 )
        ObjectCollision( true, 'wall', true, 20,20 )
    else
        ObjectCollision( false, 'wall' )
    end
  end
  ]]
end

function ToggleRenderable( guiName )
  local name = GuiGetText( 'editor/object.object_name' )
  local checked = GuiGetChecked( 'editor/object.is_renderable' )
  if checked then
    local tileSize = GetTileSize()
    ObjectRenderable( true, name, 'data/temp/alik16.bmp', tileSize,tileSize, 1,1,1,1 )
  else
    ObjectRenderable( false, name )
  end
end

function ToggleCollision( guiName )
  local name = GuiGetText( 'editor/object.object_name' )
  local checked = GuiGetChecked( 'editor/object.is_collision' )
  local isStatic = GuiGetChecked( 'editor/object.is_static' )
  if checked then
    local tileSize = GetTileSize()
    ObjectCollision( true, name, isStatic, tileSize, tileSize )
  else
    ObjectCollision( false, name )
  end
end

function ToggleStatic( guiName )
  local name = GuiGetText( 'editor/object.object_name' )
  local checked = GuiGetChecked( 'editor/object.is_static' )
  CollisionSetStatic( name, checked )
end

function ToggleTrigger( guiName )
  local name = GuiGetText( 'editor/object.object_name' )
  local checked = GuiGetChecked( 'editor/object.is_trigger' )
  if checked then
    local tileSize = GetTileSize()
    ObjectTrigger( true, name, tileSize, tileSize )
  else
    ObjectTrigger( false, name )
  end
end

function GetTileSize()
  return GuiGetText( 'editor/world.tile_size' )
end

function TileAdd( guiName )
  local name = GuiGetText( 'editor/object.object_name' )
  local cameraX, cameraY = GetCameraPos()
  ObjectCreate( name, cameraX, cameraY, 0 )
  ToggleRenderable( 'editor/object.is_renderable' )
  editorMode = 1
  SetTimer( 0.01, 'UpdateEditorMode1' )
end

function UpdateEditorMode1()
  local name = GuiGetText( 'editor/object.object_name' )
  local cursorX, cursorY = GetMousePos()
  local cameraX, cameraY = GetCameraPos()
  local windowW, windowH = GetWindowSize()
  local tileSize = GetTileSize()
  ObjectSetPos( name, math.floor( ( cursorX + cameraX - windowW / 2 + 0.5 * tileSize ) / tileSize ) * tileSize, math.floor( ( cursorY + cameraY - windowH / 2 + 0.5 * tileSize ) / tileSize ) * tileSize )
  if editorMode == 1 then
    SetTimer( 0.01, 'UpdateEditorMode1' )
  end
end

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
