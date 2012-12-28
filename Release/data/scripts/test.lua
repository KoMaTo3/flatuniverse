-- ObjectRemove( 'new-block' )

--SetTimer( 1, 'test' )

anim = 0

function SpacePressed()
  -- anim = 0
  -- doAnimation( -1 )
  while ObjectRemove( 'test' ) do
  end
  -- ObjectCreate( 'test', '', math.random(-50,50), math.random(-50,50), 0, 16, 16, 'data/temp/brick3.png' )
  ObjectCreate( 'test', '', math.random(-50,50), math.random(-50,50), 0, 16, 16, 'data/temp/brick3.png' )
end

function doAnimation( id )
  x, y = ObjectGetPos( 'block0' )
  ObjectSetPos( 'block0', x, y+2 )
  anim = anim + 1
  if anim < 30 then
    SetTimer( 0.05, 'doAnimation' )
  end
end

function test( id )
  x, y = ObjectGetPos( 'player' )
  ObjectSetPos( 'player', x + 10, y )
  LogWrite( 'test123: timer id['..id..']' )
end
