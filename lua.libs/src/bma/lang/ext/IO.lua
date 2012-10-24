-- bma/lang/ext/IO.lua

io.readContent = function(filename)
	local l = {}
	for line in io.lines(filename) do
		table.insert (l, line)
	end
	return table.concat(l,"\n")
end

io.writeContent = function(filename, content, mode)
	if mode==nil then mode = "w+" end
	local h = io.open(filename,mode)
	h:write (content)
	h:close()
end