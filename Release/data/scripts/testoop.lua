TestClass = {}
TestClass_mt = { __index = TestClass }

function TestClass:new( val )
  local newObj = {
    value = val,
  }
  setmetatable( newObj, TestClass_mt )
  return newObj
end

function TestClass:alert()
  Debug.Alert( self.value )
end


--[[ InheritsFrom ]]
function InheritsFrom( baseClass )
  local new_class = {}
  local class_mt = { __index = new_class }

  function new_class:create()
    local newinst = {}
    setmetatable( newinst, class_mt )
    return newinst
  end

  if baseClass then
    setmetatable( new_class, { __index = baseClass } )
  end

  return new_class
end -- InheritsFrom

TestClassNew = InheritsFrom( TestClass )

function TestClassNew:alert()
  Debug.Alert( 'class 2: '..( self.value ) )
end

obj1 = TestClass:new( 8 )
obj2 = TestClassNew:new( 3 )
-- obj1:alert()
-- obj2:alert()
