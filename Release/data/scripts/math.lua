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
  local cameraX, cameraY = GetCameraPos()
  x = math.floor( ( cameraX + px - settings.windowSize.x * 0.5 + 0.5 * tileSize ) / tileSize )
  y = math.floor( ( cameraY + py - settings.windowSize.y * 0.5 + 0.5 * tileSize ) / tileSize )
  return x, y
end -- GetTilePosByPixel

--[[ GetPixelByTile ]]
function GetPixelByTile( x, y )
  local px, py
  local tileSize = GetTileSize()
  local cameraX, cameraY = GetCameraPos()
  px = x * tileSize - cameraX + ( settings.windowSize.x - tileSize ) * 0.5
  py = y * tileSize - cameraY + ( settings.windowSize.y - tileSize ) * 0.5
  return px, py
end -- GetPixelByTile

--[[ GetGridByCoords ]]
function GetGridByCoords( x, y )
  local size = GetGridSize()
end -- GetGridByCoords
