function get_mac(ip, port)

    io.input("debug/rc.driver")
    buf = io.read("*a")
    mac5, mac6 = buf:match("HWADDR%s*=%s*\"%w+:%w+:%w+:%w+:(%w+):(%w+)\"")
    io.input():close()

    buf = nil
    fp = io.open("debug/mac.txt", "r")
    if fp ~= nil then
        buf = fp:read("*a")
        fp:close()
    end 
    
    exist = nil
    newline = string.format("%s %s 0A:C5:33:AA:%s:%s\n", ip, port, mac5, mac6)
    if buf ~= nil then exist = buf:find(newline) end
    if exist == nil then
        io.output("debug/mac.txt")
        if buf ~= nil then io.write(buf) end
        io.write(newline)
        io.output():close()
    end
end

