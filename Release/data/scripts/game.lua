tileSize = 16

function Init()
  math.randomseed( GetRandomSeed() )
  ListenKeyboard( 'onKey' )
  ListenMouseKey( 'onMouseKey' )
  GuiAddTrigger( 'editor/object.is_renderable', 'ToggleRenderable' )
  GuiAddTrigger( 'editor/object.reset_position', 'ResetPosition' )
end
Init()

function onKey( id, isPressed )
  if isPressed then
      if id == 0x1B then
        GameExit()
      end
  end
end

function onMouseKey( id, isPressed )
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
        ObjectCreate( 'wall', pos.x, pos.y, 0 )
        ObjectRenderable( true, 'wall', 'data/temp/alik16.bmp', 20, 20, 1,1,1,1 )
        ObjectCollision( true, 'wall', true, 20,20 );
    else
        ObjectCollision( false, 'wall' );
    end
  end
end

function ToggleRenderable( guiName )
    Alert( 'ToggleRenderable' )
end

function ResetPosition( guiName )
    Alert( 'test', GuiGetText( 'editor/object.object_name' ) )
end
