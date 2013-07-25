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
  local cameraX, cameraY = GetCameraPos()
  x = math.floor( ( cameraX + px - settings.windowSize.x * 0.5 + 0.5 * tileSize - offsetX ) / tileSize )
  y = math.floor( ( cameraY + py - settings.windowSize.y * 0.5 + 0.5 * tileSize - offsetY ) / tileSize )
  return x, y
end -- GetTilePosByPixel

--[[ GetPixelByTile ]]
function GetPixelByTile( x, y )
  local px, py
  local tileSize = GetTileSize()
  local offsetX, offsetY = GetTileOffset()
  local cameraX, cameraY = GetCameraPos()
  px = x * tileSize - cameraX + ( settings.windowSize.x - tileSize ) * 0.5 + offsetX
  py = y * tileSize - cameraY + ( settings.windowSize.y - tileSize ) * 0.5 + offsetY
  return px, py
end -- GetPixelByTile

--[[ GetGridByCoords ]]
function GetGridByCoords( x, y )
  local size = GetGridSize()
end -- GetGridByCoords

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
