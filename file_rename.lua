os.execute("rm -rf source.bak") -- Remove old backup if it exists

-- First, collect a list of files in the source directory that are .h

local files = {}

for line in io.popen("find source -type f -name *.h"):lines() do
    files[#files+1] = line
end

print(#files, "files to rename")

-- Next we collect a list of all source files

local sourceFiles = {}

for line in io.popen("find source -type f"):lines() do
    sourceFiles[#sourceFiles+1] = line
end

-- Then we back up the old source

os.execute("cp -r source source.bak")

-- After that, go through all the source files and rename all header includes found

local function patternEscape(str)
    return str:gsub("%.", "%%.")
end

local function dir(fn)
    return fn:sub(1, fn:match'^.*()/')
end

local function processFile(fn)
    local fh = io.open(fn, "r")
    local src = fh:read("*a")
    fh:close()

    local fnDir = dir(fn)

    local replacementCount, rctmp = 0
    for _, header in ipairs(files) do
        if header ~= fn then
            local eheader = patternEscape(header:sub(8))
            src, rctmp = src:gsub(eheader, eheader.."pp")
            replacementCount = replacementCount+rctmp
            src, rctmp = src:gsub("#include \""..eheader.."pp\"", "#include <"..eheader.."pp>")
            replacementCount = replacementCount+rctmp
            -- Handle local includes if a subdir of fn
            -- Lua has no startsWith but who gives a shit?
            if header:sub(1, #fnDir) == fnDir then
                local localFile = header:sub(#fnDir+1)
                local elocalFile = patternEscape(localFile)
                src, rctmp = src:gsub("\""..localFile.."\"", "\""..localFile.."pp\"")
                replacementCount = replacementCount+rctmp
            end
        end
    end

    fh = io.open(fn, "w")
    fh:write(src)
    fh:close()

    print("Processed", fn, "RC", replacementCount)
end

for _, file in ipairs(sourceFiles) do
    processFile(file)
end

-- Then rename the header files

for _, header in ipairs(files) do
    print("Rename", header)
    os.execute("mv "..header.." "..header.."pp")
end

print("Done!")
