-- handle parameter config file

function ThisVer() 
    local major=1
    local minor=1
    return major*1000+minor
end

--[[
Separate version string
e.g. Input:3.7.3, return 3 7 3
	 Input:2.3.1 20, return 2 3 1 20
--]]
function seprt_ver(s)
  vt={}
  for v in string.gmatch(s, "%d+") do
    vt[#vt + 1] = v
  end
  return vt
end

--[[
    Input:	ver1, ver2 -- version string be compared
	Return: -1=ver1<ver2, 0=ver1==ver2, 1=ver1>ver2
--]]
function compare_ver(ver1, ver2)
  if not ver1 then return -1 end
  if not ver2 then return 1 end
  
  vt1 = seprt_ver(ver1)
  vt2 = seprt_ver(ver2)
  for i,v in ipairs(vt1) do
    if tonumber(v)>tonumber(vt2[i]) then return 1
    elseif tonumber(v)<tonumber(vt2[i]) then return -1
    end
  end
  return 0
end

--[[
	Input:	fname -- filename
	        ftype -- file type 
	                    "cfg" : export name=value | name=value 
	                    "lst" : value:name
	                    "ary" : num:value
	                    "sys" : only for .sys/sys.inf
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

--[[
Scan the file will be update
	Input:	eqpt -- equipment type. 1=PQNet, 2=PQAny
	        upcfg -- update config
	Return: update information
--]]
function scan_upfile(eqpt, upcfg)
    local des_ver = get_cfginfo(".sys/version.inf", "cfg")
    local src_ver = get_cfginfo("upfile/version.inf", "cfg")
    local svx_ver = get_cfginfo("upfile/svx/version.inf", "cfg")
    if src_ver.svx ~= svx_ver.svx then
        src_ver.svx = svx_ver.svx
        io.output("upfile/version.inf")
        io.write("#!/bin/sh\n")
        for k,v in pairs(src_ver) do io.write(string.format("%s=%s\n", k, v)) end
        io.close()
    end
    -- print("des_ver:") for k,v in pairs(des_ver) do print(k,v) end
    -- print("src_ver:") for k,v in pairs(src_ver) do print(k,v) end

    -- Compare version
    local upinf = {}
    for k,v in pairs(src_ver) do
        if not des_ver[k] or compare_ver(src_ver[k],des_ver[k])==1 then upinf[k] = 1
        else upinf[k] = 0 end
    end
    -- print("upinf:") for k,v in pairs(upinf) do print(k,v) end
    
    -- Adjust
    if upinf.logo_bmp == 0 then
        if compare_ver(src_ver[k], des_ver[k])~=0 then upinf.logo_bmp = 1 end
    end
    if upinf.kernel == 1 then
        if upcfg.kernel == "0" then upinf.kernel = 0 end
    end
    upinf.ied_model = 0
    if des_ver.ied_model then
        if compare_ver(src_ver.ied_model, des_ver.ied_model)~=0 then 
            upinf.ied_model = 1 
            print(string.format("ied_model doesn't match: des=%s, src=%s", des_ver.ied_model, src_ver.ied_model))
        end
    end

    -- print("upfile:") for k,v in pairs(upfile) do print(k,v) end
    return upinf, des_ver
end

--[[
Scan the 61850 file will be update
	Input:	csmt -- 61850 customer. e.g. "StateGrid", "Henan"
--]]
function scan_up61850(cstm)
    local des_ver = get_cfginfo(".sys/ver61850.inf", "cfg")
    -- print("des_ver:") for k,v in pairs(des_ver) do print(k, v) end
    local src_ver = get_cfginfo(string.format("upfile/61850/%s/boyuu61850/version.inf", cstm), "cfg")
    local app_ver = get_cfginfo("upfile/61850/_app/version.inf", "cfg")
    src_ver.server = app_ver.server
    -- print("src_ver:") for k,v in pairs(src_ver) do print(k, v) end
    
    io.output("upfile/61850/ver61850.inf")
    for k,v in pairs(src_ver) do io.write(string.format("%s=%s\n", k, v)) end
    io.close()
    
    -- Compare version
    local upinf = {}
    for k,v in pairs(src_ver) do
        if not des_ver[k] or compare_ver(src_ver[k], des_ver[k])==1 then upinf[k] = 1
        else upinf[k] = 0 end
    end
    if not upinf.config then upinf.config = 1 end
    -- for k,v in pairs(upinf) do print(k, v) end
    
    -- Adjust
    if src_ver.custom ~= des_ver.custom then
        upinf.custom = 1
        upinf.model = 1
        upinf.config = 1
    end

    if upinf.server ~= 0 then
        if not des_ver.server or compare_ver(des_ver.server, "1.1.8")==-1 then upinf.custom = 2 end
    end
	-- print("up61850:") for k,v in pairs(upinf) do print(k,v) end
    return upinf
end

--[[
Create b_upload.scr file

	Input:	upf -- update file
	        up6 -- update 61850 file
	        cstm -- custom of 61850
	        vendor -- 
--]]
function create_upload(upf, up6, cstm, vendor)
    io.output(".sys/script/01/b_upload.scr")

    -- system & application
    os.execute("mkdir .sys\\up_tmp");
    os.execute("copy upfile\\script\\update.sh .sys\\up_tmp > nul");
    os.execute("copy upfile\\script\\reboot.sh .sys\\up_tmp > nul");
    os.execute("copy upfile\\version.inf .sys\\up_tmp > nul");
    
    if upf.pqnet_mn == 1 then
        os.execute("copy upfile\\dpqnet_mn .sys\\up_tmp > nul")
    end
    if upf.pqnet_gui == 1 then
        os.execute("copy upfile\\dpqnet_gui .sys\\up_tmp > nul")
    end
    if upf.kernel == 1 then
        os.execute("copy upfile\\kernel\\uImage .sys\\up_tmp > nul")
    end
    if upf.vendor == 1 then
        os.execute(string.format("copy upfile\\logo\\logo_%s.bmp .sys\\up_tmp\\logo.bmp > nul", vendor));
    end
    if upf.system ~= 0 then
        -- print(string.format(".sys\\scan_upsys.exe %d .sys", upf.sysver))
        os.execute(string.format(".sys\\scan_upsys.exe %d .sys", upf.sysver))
        local upsys = get_cfginfo(".sys/upsys_files.lst", "ary")
        for k,v in pairs(upsys) do os.execute(string.format("copy upfile\\system\\%s .sys\\up_tmp > nul", v)) end
        os.execute("copy .sys\\upsys.sh .sys\\up_tmp > nul")
    end
    
    os.execute("upfile\\script\\create_md5.bat app")
    io.write("cd /tmp/upf\n")
    io.write("put .sys/md5sum\n")
    io.write("put .sys/check.md5\n")
    io.write("mput .sys/up_tmp/*\n")
    
    -- svx
    if upf.svx == 1 then
        os.execute("upfile\\script\\create_md5.bat svx")
        io.write("put -r upfile/svx\n")
    end

    -- 61850
    if up6.custom == 2 then
        os.execute("upfile\\script\\create_md5.bat 61850 _app scl_srvr_n")
        io.write("put upfile/61850/_app/scl_srvr_n\n")
        os.execute(string.format("upfile\\script\\create_md5.bat 61850 %s boyuu61850/*", cstm))
        io.write(string.format("put -r upfile/61850/%s/boyuu61850\n", cstm))
        os.execute(string.format("upfile\\script\\create_md5.bat 61850 %s data_sv/*", cstm))
        io.write(string.format("put -r upfile/61850/%s/data_sv\n", cstm))
    elseif up6.custom ~= 0 or up6.server == 1 or up6.model == 1 or up6.config == 1 or up6.db == 1 then
        if up6.server == 1 then 
            os.execute("upfile\\script\\create_md5.bat 61850 _app scl_srvr_n")
            io.write("put upfile/61850/_app/scl_srvr_n\n")
        end
        io.write("mkdir boyuu61850\n")
        io.write("cd boyuu61850\n")
        -- io.write("cd /boyuu/save/boyuu61850\n")
        if up6.custom == 1 or up6.config == 1 then 
            os.execute(string.format("upfile\\script\\create_md5.bat 61850 %s boyuu61850/*.cfg", cstm))
            io.write(string.format("mput upfile/61850/%s/boyuu61850/*.cfg\n", cstm))
            os.execute(string.format("upfile\\script\\create_md5.bat 61850 %s boyuu61850/*.xml", cstm))
            io.write(string.format("mput upfile/61850/%s/boyuu61850/*.xml\n", cstm))
        elseif up6.model == 1 then
            os.execute(string.format("upfile\\script\\create_md5.bat 61850 %s boyuu61850/datamap.cfg", cstm))
            io.write(string.format("put upfile/61850/%s/boyuu61850/datamap.cfg\n", cstm)) 
        end
        io.write("put upfile/61850/ver61850.inf\n") 
        if up6.custom == 1 or up6.model == 1 then
            os.execute(string.format("upfile\\script\\create_md5.bat 61850 %s data_sv/*", cstm))
            io.write("cd ..\n")
            io.write("mkdir data_sv\n")
            io.write("cd data_sv\n")
            io.write(string.format("mput upfile/61850/%s/data_sv/*.icd\n", cstm)) 
        end
    end

    io.output().close()
end

--[[
Create c_update.scr file

	Input:	upf -- update file
	        up6 -- update 61850 file
	        up_cfg -- 
--]]
function create_update(upf, up6, up_cfg)
    io.output(".sys/script/01/c_update.scr")

    io.write("cd /tmp/upf\n")
    --61850
    if up6.need == 1 then
        if up6.custom ~= 0 or up6.db ~= 0 then io.write("rm /home/boyuu/save/boyuu61850/jou/*\n") end
        io.write("rm /home/boyuu/save/boyuu61850/mms*\n")
        if up6.server == 1 then
            io.write("chmod +x /tmp/upf/scl_srvr_n\n")
            io.write("chmod +x /home/boyuu/save/boyuu61850/scl_srvr_n\n")
        end
    end
    
    io.write("sleep 2\n")
    io.write("chmod +x update.sh\n")
    io.write(string.format("./update.sh %s %s\n", up_cfg.reboot, up_cfg.debug))
    io.output().close()
end

--[[
    Input:  n -- 61580 customer in number. 1, 2, ...
    Return: 61580 customer in string. e.g. "StateGrid", "Henan"
--]]
function get_61850custm(n)
    local custms = get_cfginfo(".sys/cst61850.lst", "ary")
    -- print("custms:") for k,v in pairs(custms) do print(k,v) end
    return custms[tonumber(n)]
end

--[[
    Input:  n -- vendor number. 1, 2, ...
    Return: vendor name in string. e.g. "boyuu", "hoshing"
--]]
function get_vendor(n)
    local vendors = get_cfginfo(".sys/vendors.lst", "ary")
    return vendors[tonumber(n)]
end

--[[
	Input:	eqpt -- equipment type. 1=PQNet
	        dbg -- debug model
	Return: 0=success, 1=needn't update
--]]
function preprocess(eqpt, dbg)
    local up_cfg = get_cfginfo("update.cfg", "cfg")
    up_cfg.debug = dbg
    -- print("up_cfg:") for k,v in pairs(up_cfg) do print(k,v) end
    local upfile, ver = scan_upfile(eqpt, up_cfg)
    -- print("ver:") for k,v in pairs(ver) do print(k,v) end
    -- print("upfile:") for k,v in pairs(upfile) do print(k,v) end
    local vendor = get_vendor(up_cfg.vendor)
    local custm6 = get_61850custm(up_cfg.cst61850)
    -- print("vendor:", vendor, "custom61850:", custm6)
    local up61850 = scan_up61850(custm6)
    if upfile.system==1 then upfile.sysver = ver.system end
    
    -- Force config processing
    if tonumber(up_cfg.force) == 1 then
        local frclst = get_cfginfo(".sys/force_up.lst", "lst")
        if bit32.extract(up_cfg.prog, frclst.App, 1) == 1 then upfile.pqnet_mn = 1 upfile.pqnet_gui = 1 end
        if bit32.extract(up_cfg.prog, frclst["61850"], 1) == 1 then up61850.custom = 2 end
        if bit32.extract(up_cfg.prog, frclst.Kernel, 1) == 1 then upfile.kernel = 1 end
        if bit32.extract(up_cfg.prog, frclst.System, 1) == 1 then upfile.system = 1 upfile.sysver = 0 end
        if bit32.extract(up_cfg.prog, frclst.svx, 1) == 1 then upfile.svx = 1 end
    end
    

    upfile.need = 0
    for k,v in pairs(upfile) do if v ~= 0 then upfile.need=1 break end end
    up61850.need = 0
    for k,v in pairs(up61850) do if v ~= 0 then up61850.need=1 break end end
    -- print("upfile:") for k,v in pairs(upfile) do print(k,v) end
    -- print("up61850:") for k,v in pairs(up61850) do print(k,v) end
    if upfile.need == 1 or up61850.need == 1 then
        create_upload(upfile, up61850, custm6, vendor)
        create_update(upfile, up61850, up_cfg)
        return 0
    else
        return 1
    end
end

