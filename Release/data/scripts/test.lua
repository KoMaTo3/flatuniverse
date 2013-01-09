-- Init
tileSize = 16
level = {
    width  = 10,
    height = 20,
    dx = -100,
    dy = -170,
    field = {},
    speed = 5,
    speedUp = false,
    items = {
      {     -- 1: квадрат
        {   -- 1: начальное положение
            { 0,1,1,0 },
            { 0,1,1,0 },
            { 0,0,0,0 },
            { 0,0,0,0 },
            { 1,0,0,1 }, -- цвет
        },
      },
      {     -- 2: L
        {   -- 1: начальное положение
            { 0,1,0,0 },
            { 0,1,0,0 },
            { 0,1,1,0 },
            { 0,0,0,0 },
            { 0,1,0,1 }, -- цвет
        },
        {   -- 2
            { 0,1,1,1 },
            { 0,1,0,0 },
            { 0,0,0,0 },
            { 0,0,0,0 },
            { 0,1,0,1 }, -- цвет
        },
        {   -- 3
            { 0,1,1,0 },
            { 0,0,1,0 },
            { 0,0,1,0 },
            { 0,0,0,0 },
            { 0,1,0,1 }, -- цвет
        },
        {   -- 4
            { 0,0,1,0 },
            { 1,1,1,0 },
            { 0,0,0,0 },
            { 0,0,0,0 },
            { 0,1,0,1 }, -- цвет
        },
      },
      {     -- 3: z
        {   -- 1: начальное положение
            { 0,1,0,0 },
            { 0,1,1,0 },
            { 0,0,1,0 },
            { 0,0,0,0 },
            { 1,0,0,1 }, -- цвет
        },
        {   -- 2
            { 0,1,1,0 },
            { 1,1,0,0 },
            { 0,0,0,0 },
            { 0,0,0,0 },
            { 1,0,0,1 }, -- цвет
        },
      },
      {     -- 4: I
        {   -- 1: начальное положение
            { 1,1,1,1 },
            { 0,0,0,0 },
            { 0,0,0,0 },
            { 0,0,0,0 },
            { 1,0,0,1 }, -- цвет
        },
        {   -- 2
            { 0,0,1,0 },
            { 0,0,1,0 },
            { 0,0,1,0 },
            { 0,0,1,0 },
            { 1,0,0,1 }, -- цвет
        },
      },
      {     -- 5: угол
        {   -- 1: начальное положение
            { 0,1,1,1 },
            { 0,0,1,0 },
            { 0,0,0,0 },
            { 0,0,0,0 },
            { 0,1,0,1 }, -- цвет
        },
        {   -- 2
            { 0,0,1,0 },
            { 0,1,1,0 },
            { 0,0,1,0 },
            { 0,0,0,0 },
            { 0,1,0,1 }, -- цвет
        },
        {   -- 3
            { 0,0,1,0 },
            { 0,1,1,1 },
            { 0,0,0,0 },
            { 0,0,0,0 },
            { 0,1,0,1 }, -- цвет
        },
        {   -- 4
            { 0,1,0,0 },
            { 0,1,1,0 },
            { 0,1,0,0 },
            { 0,0,0,0 },
            { 0,1,0,1 }, -- цвет
        },
      },
    },
    current = {
      x = 1,    -- [1:level.width]
      y = 1,    -- [1:level.height]
      item = 1, -- [1:...]
      rot = 1,  -- [1:4]
    },

    --
    -- Init
    --
    Init = function()
      for y = 1,level.height do
        level.field[ y ] = {}
        for x = 1,level.width do
            level.field[ y ][ x ] = 0
        end
      end
      level.current = {
        x = 4,
        y = 1,
        item = math.random( 1, #level.items ),
        rot = math.random( 1, #level.items[ level.current.item ] ),
      }
      level.PushItem( level.current.x, level.current.y, level.current.item, level.current.rot )
      level.ShowBorders()
      level.SetTimer()
    end,

    --
    -- ShowBorders
    --
    ShowBorders = function()
      local x, y
      for y = -1,level.height do
        x = 0
        ObjectCreate( 'level-border-'..y..'-'..x, '', x * tileSize + level.dx, y * tileSize + level.dy, 0, tileSize, tileSize, 'data/temp/brick0.png' )
        x = level.width + 1
        ObjectCreate( 'level-border-'..y..'-'..x, '', x * tileSize + level.dx, y * tileSize + level.dy, 0, tileSize, tileSize, 'data/temp/brick0.png' )
      end
      for x = 0,level.width+1 do
        y = level.height + 1
        ObjectCreate( 'level-border-'..y..'-'..x, '', x * tileSize + level.dx, y * tileSize + level.dy, 0, tileSize, tileSize, 'data/temp/brick0.png' )
      end
    end,

    --
    -- SetTimer
    --
    SetTimer = function()
      SetTimer( level.speedUp and 0.1 or ( ( 10 - level.speed ) / 10.0 ), 'Update' )
    end,

    --
    -- Clear
    --
    Clear = function()
      for y = 1,level.height do
        for x = 1,level.width do
          if( level.field[ y ][ x ] == 1 ) then
            ObjectRemove( 'level-object-'..y..'-'..x )
          end
          level.field[ y ][ x ] = 0
        end
      end
    end,

    --
    -- PushItem
    --
    PushItem = function( itemX, itemY, item, rot )
      for y = 0,3 do
        if itemY + y >= 0 and itemY + y < level.height then
          for x = 0,3 do
            if itemX + x >= 0 and itemX + x < level.width then
              if level.items[ item ][ rot ][ y + 1 ][ x + 1 ] == 1 then
                ObjectCreate( 'level-object-'..( itemY + y + 1 )..'-'..( itemX + x + 1 ), '', ( itemX + x + 1 ) * tileSize + level.dx, ( itemY + y + 1 ) * tileSize + level.dy, 0, tileSize, tileSize, 'data/temp/brick3.png' )
                level.field[ itemY + y + 1 ][ itemX + x + 1 ] = 1;
              end
            end
          end
        end
      end
    end,

    --
    -- EraseItem
    --
    EraseItem = function( itemX, itemY, item, rot )
      for y = 0,3 do
        if itemY + y >= 0 and itemY + y < level.height then
          for x = 0,3 do
            if itemX + x >= 0 and itemX + x < level.width then
              if level.items[ item ][ rot ][ y + 1 ][ x + 1 ] == 1 then
                ObjectRemove( 'level-object-'..( itemY + y + 1 )..'-'..( itemX + x + 1 ) )
                level.field[ itemY + y + 1 ][ itemX + x + 1 ] = 0;
              end
            end
          end
        end
      end
    end,

    --
    -- CheckPlaceForItem
    --
    CheckPlaceForItem = function( itemX, itemY, item, rot )
      for y = 0,3 do
        for x = 0,3 do
          if level.items[ item ][ rot ][ y + 1 ][ x + 1 ] == 1 then
            if itemX + x >= 0 and itemX + x < level.width and
               itemY + y >= 0 and itemY + y < level.height then
                  if level.field[ itemY + y + 1 ][ itemX + x + 1 ] == 1 then
                    return false
                  end
            else
              return false
            end
          end
        end
      end
      return true
    end,

    --
    -- MovePiece
    --
    MovePiece = function( fromY, fromX, toY, toX )
      if level.field[ fromY ][ fromX ] == 0 then
        if level.field[ toY ][ toX ] == 1 then  -- удалить to
          ObjectRemove( 'level-object-'..toY..'-'..toX )
          level.field[ toY ][ toX ] = 0
        end
      else
        if level.field[ toY ][ toX ] == 0 then  -- добавить в to, удалить from
          ObjectCreate( 'level-object-'..toY..'-'..toX, '', toX * tileSize + level.dx, toY * tileSize + level.dy, 0, tileSize, tileSize, 'data/temp/brick3.png' )
          level.field[ toY ][ toX ] = 1
        end
        ObjectRemove( 'level-object-'..fromY..'-'..fromX )
        level.field[ fromY ][ fromX ] = 0
      end
    end,

    --
    -- CheckLines
    --
    CheckLines = function()
      for y = 2,level.height do
        local isLine = true
        for x = 1,level.width do
          if level.field[ y ][ x ] == 0 then
            isLine = false
            break
          end
        end
        if isLine then  -- собрали линию => опускаем всё, что выше неё
          for ny = y - 1, 1, -1 do
          for x = 1,level.width do
            level.MovePiece( ny, x, ny + 1, x )
          end
          end
        end
      end
    end,
}

function Init()
  LogWrite( GetRandomSeed() )
  math.randomseed( GetRandomSeed() )
  ListenKeyboard( 'onKey' )
  ListenMouseKey( 'onMouseKey' )
  ClearScene()
  ObjectCreate( 'camera-center', '', 0, 0, 0, 0, 0, 'data/temp/null.png' )
  SetCamera( 'camera-center' )

  level.Init()
end
Init()

function onKey( id, isPressed )
  if isPressed then
      if id == 0x25 then    -- движение влево
          level.EraseItem( level.current.x, level.current.y, level.current.item, level.current.rot )
          if level.CheckPlaceForItem( level.current.x - 1, level.current.y, level.current.item, level.current.rot ) == true then
            level.current.x = level.current.x - 1
            level.PushItem( level.current.x, level.current.y, level.current.item, level.current.rot )
          else
            level.PushItem( level.current.x, level.current.y, level.current.item, level.current.rot )
          end
      end
      if id == 0x27 then    -- движение вправо
          level.EraseItem( level.current.x, level.current.y, level.current.item, level.current.rot )
          if level.CheckPlaceForItem( level.current.x + 1, level.current.y, level.current.item, level.current.rot ) == true then
            level.current.x = level.current.x + 1
            level.PushItem( level.current.x, level.current.y, level.current.item, level.current.rot )
          else
            level.PushItem( level.current.x, level.current.y, level.current.item, level.current.rot )
          end
      end
      if id == 0x5A or id == 0x26 then    -- поворот
          level.EraseItem( level.current.x, level.current.y, level.current.item, level.current.rot )
          local nextRot = ( level.current.rot % #level.items[ level.current.item ] ) + 1
          if level.CheckPlaceForItem( level.current.x, level.current.y, level.current.item, nextRot ) == true then
            level.current.rot = nextRot
            level.PushItem( level.current.x, level.current.y, level.current.item, level.current.rot )
          else
            level.PushItem( level.current.x, level.current.y, level.current.item, level.current.rot )
          end
      end
      if id == 0x58 then    -- поворот
          level.EraseItem( level.current.x, level.current.y, level.current.item, level.current.rot )
          local nextRot = ( ( level.current.rot + #level.items[ level.current.item ] * 2 - 2 ) % #level.items[ level.current.item ] ) + 1
          if level.CheckPlaceForItem( level.current.x, level.current.y, level.current.item, nextRot ) == true then
            level.current.rot = nextRot
            level.PushItem( level.current.x, level.current.y, level.current.item, level.current.rot )
          else
            level.PushItem( level.current.x, level.current.y, level.current.item, level.current.rot )
          end
      end
      if id == 0x28 then    -- ускорение
        level.speedUp = true
      end
      if id == 0x1B then
        GameExit()
      end
  end
end

function onMouseKey( id, isPressed )
  if isPressed then
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
  end
end

-- Main code

function Update()
  level.EraseItem( level.current.x, level.current.y, level.current.item, level.current.rot )
  if level.CheckPlaceForItem( level.current.x, level.current.y + 1, level.current.item, level.current.rot ) == true then
    level.current.y = level.current.y + 1
    level.PushItem( level.current.x, level.current.y, level.current.item, level.current.rot )
  else  -- фигурка упала
    level.speedUp = false
    level.PushItem( level.current.x, level.current.y, level.current.item, level.current.rot )
    level.current.x = 4
    level.current.y = 0
    level.current.item = math.random( 1, #level.items )
    level.current.rot = math.random( 1, #level.items[ level.current.item ] )
    level.CheckLines()
    level.PushItem( level.current.x, level.current.y, level.current.item, level.current.rot )
  end

  level.SetTimer()
end
