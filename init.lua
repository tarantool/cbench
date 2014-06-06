dofile('example.lua')

-- Encode the result and save to a file
json_result = box.cjson.encode(result)
filename = string.format('bench-result-%s-%s.json',
    box.info.version, box.info.build.target);
file = io.open(filename, 'w')
file:write(json_result)
file:flush()
file:close()
print('Benchmark result saved to ', filename)

box.raise(0, "finish")
