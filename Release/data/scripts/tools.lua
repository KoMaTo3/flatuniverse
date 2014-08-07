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
  local size = Config.GetGridSize()
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

function TableSize( T )
  local count = 0
  for _ in pairs( T ) do
    count = count + 1
  end
  return count
end

--[[
function char(c) return ("\\%3d"):format(c:byte()) end
function szstr( s ) return ('"%s"'):format(s:gsub("[^ !#-~]", char)) end

function Serialize( value )
  local result = ''
  local valueType = type( value )
  if valueType == 'nil' then
    result = 'nil'
  elseif valueType == 'boolean' then
    result = value and 'true' or 'false'
  elseif valueType == 'number' then
    result = string.format( "%f", value )
  elseif valueType == 'string' then
    result = string.format( "%q", value )
  elseif valueType == 'table' then
    result = '{'
    local num = 0
    for key,val in pairs( value ) do
      result = result..( num == 0 and '' or ',' )..key..' = '..Serialize( val )
      num = num + 1
    end
    result = result..'}'
  elseif valueType == 'function' then
    result = 'loadstring'..szstr( string.dump( value ) )
  end
  return result
end
]]

local SPEC_CHARS = {}
local formatStr = string.format;
local function quote (s)
  return '"'..s:gsub('[%z\1-\31%\\%"\127]', function (ch)
    local c = SPEC_CHARS[ch];
    if c then return c;
    elseif ch < " " then return formatStr("\\%03i", ch:byte());
    else return "\\"..ch;  --"
    end;
  end)..'"';
end;
string.quote = quote;


local dump = string.dump;
local join = table.concat;


--[[
  based on the code by Manuel Fernandez-Diaz, modified by Rici Lake

  Formats tables with cycles recursively to any depth.
  Can serialize Lua functions with proper upvalue serialization.
  The output is returned as a string, suitable for passing to
  loadstring(). Self references are indicated.

]]
function serialize(t, knownfnlist)
  local indent = "";
  local name = "__";
  local cart = {}; -- a container
  local autoref = {}; -- for self references
  local cache = {}; -- cross-refs, etc
  local fnfixups = {}; -- post-fixups for upvalues
  local badIdx = { "table", "function", "userdata" };

  local basicSerialize;
  local function isTableEmpty (t) return next(t) == nil; end;

  local function serializeFunc (o, name, field)
    if knownfnlist and knownfnlist[o] then
      -- known function
      return knownfnlist[o];
    end;
    local info = debug.getinfo(o, "Su");
    if info.what == "C" then error("can't serialize C functions!"); end;
    if info.nups ~= 0 then
      local fnfix = {};
      fnfix.name = name; -- TODO
      fnfix.ups = {};
      for f = 1, info.nups do
        local n, v = debug.getupvalue(o, f);
        fnfix.ups[#fnfix.ups+1] = {
          name = n;
          value = v;
        };
      end;
      fnfixups[#fnfixups+1] = fnfix;
    end;
    local fn = dump(o);
    return formatStr("loadstring(%s)", fn:quote());
  end;

  basicSerialize = function (o, name, field)
    local to, so = type(o), tostring(o);
    if to == "function" then return serializeFunc(o, name, field);
    elseif to == "number" or to == "boolean" or o == nil then return so;
    else return so:quote();
    end;
  end;

  local function addToCart (value, name, indent, field)
    indent = indent or "";
    field = field or name;
    cart[#cart+1] = indent;
    cart[#cart+1] = field;
    local tx = type(value);
    if tx ~= "table" and tx ~= "function" then
      cart[#cart+1] = "=";
      cart[#cart+1] = basicSerialize(value, name, field);
      cart[#cart+1] = ";\n";
      return;
    end;
    -- cached?
    if cache[value] then
      cart[#cart] = "--";
      cart[#cart+1] = field;
      cart[#cart+1] = "=";
      cart[#cart+1] = cache[value];
      cart[#cart+1] = "\n";
      autoref[#autoref+1] = name;
      autoref[#autoref+1] = "=";
      autoref[#autoref+1] = cache[value];
      autoref[#autoref+1] = ";\n";
      return;
    end;
    -- not cached, output it and cache
    cache[value] = name;
    if tx == "function" then
      cart[#cart+1] = "=";
      cart[#cart+1] = serializeFunc(value, name, field);
      cart[#cart+1] = ";\n";
      return;
    end;
    -- table
    if isTableEmpty(value) then cart[#cart+1] = "={};\n";
    else
      cart[#cart+1] = "={\n";
      for k, v in pairs(value) do
        if badIdx[type(k)] then error("invalid table index type"); end;
        k = basicSerialize(k);
        local fname = formatStr("%s[%s]", name, k);
        field = formatStr("[%s]", k);
        -- one space between levels
        addToCart(v, fname, indent.." ", field);
      end;
      cart[#cart+1] = indent;
      cart[#cart+1] = "};\n";
    end;
  end;

  local res;
  if type(t) ~= "table" then
    res = "do\nlocal "..name.." = "..basicSerialize(t, name)..";\n";
  else
    cart[1] = "do\nlocal ";
    addToCart(t, name, indent);
    res = join(cart)..join(autoref).."\n";
  end;

  -- has up-fixups?
  if #fnfixups then
    -- first, serialize all upvalues
    --res = res.."-- serialize upvalues\n";
    for n, fix in ipairs(fnfixups) do
      local ups = fix.ups;
      for fn, up in ipairs(ups) do
        local uname = "__up_"..n.."_"..fn;
        res = res.."local ";
        cart = {}; autoref = {};
        local v = up.value;
        if (type(v) == "table" or type(v) == "function") and cache[v]
  then res = res..uname.."="..cache[v]..";\n";
        else
          addToCart(up.value, uname, indent);
          res = res..join(cart)..join(autoref);
        end;
      end;
    end;
    --res = res.."\n-- fix upvalues\n";
    res = res.."\nlocal _dsu=debug.setupvalue;\n";
    for n, fix in ipairs(fnfixups) do
      local ups = fix.ups;
      if n == 1 then res = res.."local "; end;
      res = res.."_fn="..fix.name..";\n";
      for fn, up in ipairs(ups) do
        local uname = "__up_"..n.."_"..fn;
        res = res.."_dsu(_fn, "..fn..", "..uname..");\n";
      end;
    end;
    res = res.."\n";
  end;

  res = res.."return "..name..";\nend;";
  return res;
end;
-- module.serialize = serialize;