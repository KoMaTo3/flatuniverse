anim = 0
time = 0
maxBlocksInAnim0 = 20
animTimer0 = -1
tileSize = 16

ListenKeyboard( 'onKey' )
ListenMouseKey( 'onMouseKey' )

function onKey( id, isPressed )
  if id == 32 and animTimer0 == -1 then
    SpacePressed()
  end
  if id == 27 then
    GameExit()
  end
end

function onMouseKey( id, isPressed )
  if isPressed then
    mx, my = GetMousePos()
    cx, cy = GetCameraPos()
    width, height = GetWindowSize()
    pos = {
      x = cx + mx - width * 0.5,
      y = cy + my - height * 0.5
    }
    pos.x = math.floor( pos.x / tileSize ) * tileSize + 0.5 * tileSize
    pos.y = math.floor( pos.y / tileSize ) * tileSize + 0.5 * tileSize
    ObjectCreate( 'wall', '', pos.x, pos.y, 0, tileSize, tileSize, 'data/temp/brick0.png' )
  end
end

function Init()
  for q = 0,maxBlocksInAnim0-1,1 do
    ObjectRemove( 'wall-test-'..q )
    ObjectRemove( 'wall-test-b-'..q )
    ObjectCreate( 'wall-test-'..q, '', ( q - 5 ) * 16, 0, 0, 16, 16, 'data/temp/brick3.png' )
    ObjectCreate( 'wall-test-b-'..q, '', ( q - 5 ) * 16, 0, 0, 16, 16, 'data/temp/brick0.png' )
  end
  animTimer0 = SetTimer( 0, 'doAnimation' )
end
Init()

function SpacePressed()
  -- while ObjectRemove( 'test' ) do end
  -- ObjectCreate( 'test', '', math.random(-50,50), math.random(-50,50), 0, 16, 16, 'data/temp/brick3.png' )
  -- ObjectCreate( 'test', '', math.random(-50,50), math.random(-50,50), 0, 16, 16, 'data/temp/brick3.png' )
end

function doAnimation( id )
  time = time + 0.1
  for q = 0,maxBlocksInAnim0-1,1 do
    x, y = ObjectGetPos( 'wall-test-'..q )
    ObjectSetPos( 'wall-test-'..q, x, math.sin( time + q ) * 5 )
    x, y = ObjectGetPos( 'wall-test-b-'..q )
    ObjectSetPos( 'wall-test-b-'..q, x, math.sin( time + q ) * 5 + 100 )
  end
  SetTimer( 0.01, 'doAnimation' )
end

function test( id )
  x, y = ObjectGetPos( 'player' )
  ObjectSetPos( 'player', x + 10, y )
  LogWrite( 'test123: timer id['..id..']' )
end
