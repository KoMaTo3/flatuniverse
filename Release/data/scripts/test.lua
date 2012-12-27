-- ObjectRemove( 'new-block' );

SetTimer( 1, 'test' );

function test( id )
  x, y = ObjectGetPos( 'player' )
  ObjectSetPos( 'player', x + 10, y )
  LogWrite( 'test123: timer id['..id..']' );
end

-- Alert( 'player position', x..':'..y )
-- Alert('test', 'ololo');