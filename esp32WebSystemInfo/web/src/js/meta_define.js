const flash_mode = [
    "QIO",
    "QOUT",
    "DIO",
    "DOUT",
    "FAST_READ",
    "SLOW_READ"

]

export function getFlashModeName(mode) {
    if (mode === 0xFF || mode >= flash_mode.length) {
        return "UNKNOWN"
    }
    return flash_mode[mode];
}