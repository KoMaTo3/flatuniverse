--[[

Набор функций по преобразованию позиций тайлов, пикселей, мировых координат

]]


--[[ GetTilePosUnderCursor ]]
function GetTilePosUnderCursor()
  local x, y = GetTilePosByPixel( mousePos.x, mousePos.y )
  return x, y
end -- GetTilePosUnderCursor

--[[ GetTilePosByPixel ]]
function GetTilePosByPixel( px, py )
  local x, y
  local tileSize = GetTileSize()
  local offsetX, offsetY = GetTileOffset()
  local cameraX, cameraY = Camera.GetPos()
  x = math.floor( ( cameraX + px - settings.windowSize.x * 0.5 + 0.5 * tileSize - offsetX ) / tileSize )
  y = math.floor( ( cameraY + py - settings.windowSize.y * 0.5 + 0.5 * tileSize - offsetY ) / tileSize )
  return x, y
end -- GetTilePosByPixel

--[[ GetPixelByTile ]]
function GetPixelByTile( x, y )
  local px, py
  local tileSize = GetTileSize()
  local offsetX, offsetY = GetTileOffset()
  local cameraX, cameraY = Camera.GetPos()
  px = x * tileSize - cameraX + ( settings.windowSize.x - tileSize ) * 0.5 + offsetX
  py = y * tileSize - cameraY + ( settings.windowSize.y - tileSize ) * 0.5 + offsetY
  return px, py
end -- GetPixelByTile


function GetWorldToScreen( x, y ) --(
  local cameraX, cameraY = Camera.GetPos()
  x = x + settings.windowSize.x * 0.5 - cameraX
  y = y + settings.windowSize.y * 0.5 - cameraY
  return x, y
end --) GetWorldToScreen


function GetScreenToWorld( x, y ) --(
  local cameraX, cameraY = Camera.GetPos()
  x = x - settings.windowSize.x * 0.5 + cameraX
  y = y - settings.windowSize.y * 0.5 + cameraY
  return x, y
end --) GetScreenToWorld


function GetGridByCoords( x, y ) --(
  local size = Config.GetGridSize()
  x = math.floor( x / size )
  y = math.floor( y / size )
  return x, y
end --) GetGridByCoords


-- left-top point of grid
function GetCoordsByGrid( x, y ) --(
  local size = Config.GetGridSize()
  x = x * size
  y = y * size
  return x, y
end --) GetCoordsByGrid


--( разбивка строки на компоненты
function Explode( str, div )
    if (div=='') then return false end
    local pos,arr = 0,{}
    for st,sp in function() return string.find(str,div,pos,true) end do
        table.insert(arr,string.sub(str,pos,st-1))
        pos = sp + 1
    end
    table.insert(arr,string.sub(str,pos))
    return arr
end --)

--( сборка строки из компонентов
function Implode( items, div )
  return table.concat( items, div )
end --)

function TableSize( T )
  local count = 0
  for _ in pairs( T ) do
    count = count + 1
  end
  return count
end
