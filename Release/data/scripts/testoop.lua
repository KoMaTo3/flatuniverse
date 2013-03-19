local setmetatable = setmetatable

function tmpInit()
  local tmp = {}
  local obj = {}
  setmetatable( obj, tmp )
end
tmpInit()

Alert( 'ok' )
