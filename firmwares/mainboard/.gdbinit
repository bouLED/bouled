target remote :2331
mon endian little
mon exec SetRTTAddr 0x24000000
mon exec SetRTTSearchRanges 0x24000000 0x1000

define prgm
    load
    mon reset
end

prgm
