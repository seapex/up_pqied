--[[
	Input:	fname -- filename
	        ftype -- file type. "cfg", "lst", "ary", "sys"
--]]
function get_cfginfo(fname, ftype)
    local tmp_tbl = {}
    local pf = io.open(fname)
    if pf ~= nil then
        local buf = pf:read("*a")
        if ftype=="cfg" then
            buf = string.gsub(buf, "export", "")
            for k, v in buf:gmatch("%s-(%g+)%s*=%s*[\"\']-([^\'\"%c]+)[\"\']-") do
                tmp_tbl[k]=v
            end
        elseif ftype=="lst" then
            for k, v in buf:gmatch("%s-(%d+)%s*:%s*[\"\']-([^\'\"%c]+)[\"\']-") do
                tmp_tbl[v] = tonumber(k)
            end
        elseif ftype=="ary" then
            for k, v in buf:gmatch("%s-(%d+)%s*:%s*[\"\']-([^\'\"%c%s]+)[\"\']-") do
                tmp_tbl[#tmp_tbl+1] = v
            end
        elseif ftype=="sys" then
            tmp_tbl.size = buf:match("/dev/mtdblock/5%s-(%d+)")
            tmp_tbl.sys = buf:match("new")
        end
        pf:close()
    end
    return tmp_tbl
end

function reset_syscfg(t61850)

    local cstms = get_cfginfo(".sys/t61850.lst", "ary")
    --print("cstms:") for k,v in pairs(cstms) do print(k,v) end
    local custom6 = cstms[t61850+1]  -- custom of 61850. e.g. "StateGrid", "Henan"

    os.execute("mkdir .sys\\up_tmp");
    os.execute("copy upfile\\script\\cold_reset .sys\\up_tmp > nul");
    os.execute(string.format("copy config\\%s\\pqm3.sys.conf .sys\\up_tmp > nul", custom6))
    os.execute("upfile\\script\\create_md5.bat app")

    io.output(".sys/script/23/b_upload.scr")
    io.write("cd /tmp/upf\n")
    io.write("put .sys/script/23/reset_syscfg.sh\n")
    io.write("put upfile/script/md5sum\n")
    io.write("put .sys/check.md5\n")
    io.write("mput .sys/up_tmp/*\n")
    io.output().close()

    io.output(".sys/script/23/c_update.scr")
    io.write("cd /tmp/upf\n")
    io.write("chmod +x reset_syscfg.sh\n")
    io.write("./reset_syscfg.sh\n")
    io.output().close()
end
