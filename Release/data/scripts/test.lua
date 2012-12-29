-- ObjectRemove( 'new-block' )

--SetTimer( 1, 'test' )

t = {}
function t:get()
  return 1
end

anim = 0
time = 0
maxBlocksInAnim0 = 20

function SpacePressed()
  -- while ObjectRemove( 'test' ) do end
  -- ObjectCreate( 'test', '', math.random(-50,50), math.random(-50,50), 0, 16, 16, 'data/temp/brick3.png' )
  -- ObjectCreate( 'test', '', math.random(-50,50), math.random(-50,50), 0, 16, 16, 'data/temp/brick3.png' )
  for q = 0,maxBlocksInAnim0-1,1 do
    ObjectRemove( 'wall-test-'..q )
    ObjectRemove( 'wall-test-b-'..q )
    ObjectCreate( 'wall-test-'..q, '', ( q - 5 ) * 16, 0, 0, 16, 16, 'data/temp/brick3.png' )
    ObjectCreate( 'wall-test-b-'..q, '', ( q - 5 ) * 16, 0, 0, 16, 16, 'data/temp/brick0.png' )
  end
  SetTimer( 0, 'doAnimation' )
end

function doAnimation( id )
  time = time + 0.05
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
