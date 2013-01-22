require("bma.test.bootstrap")
require("bma.lang.Class")
require("bma.lang.ext.Dump")

include("bma.matrix.Matrix")
include("bma.matrix.BoxMatrix")
include("bma.matrix.DataStore")

local Class = class.define("test.Matrix",{bma.matrix.BoxMatrix})

function Class:start(reader, writer)
	self:initObject()
	self.timeLabelFun = Class.TLF_SECOND
	self:historyWriter(writer)
	self:historyReader(reader)
	self:setup(0,0)
	
	self:begin()
end

function Class:handleCommand(cmd)
	if cmd.id >= 3 then
		print(cmd.id.." for random", RAND(1,100))
	end
	var_dump(cmd)
	return true
end

function Class:handleHistory(data)
	if bma.matrix.BoxMatrix.handleHistory(self, data) then return true end
	return false
end

local DataStore = bma.matrix.DataStore
local m = Class.new()
local file = "E:/tmpwork/box_matrix.dat"

if false then
	-- new
	os.remove(file)
	
	print("new matrix!!!!!!")
	m:start(nil, DataStore.textWriter(file))	
	m:postCommand({v=1})
	m:postCommand({v=2})
	m:postCommand({v=3})	
	m:postCommand({v=4})	
end

if true then
	print("load matrix!!!!!!")
	m:start(DataStore.textReader(file), DataStore.textWriter(file))		
	while not m:processHistory(100) do
	end
end
