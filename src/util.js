// reads the 32 bits or 4 ASCII chars (really 4 UTF-16 characters but we discard the high 8 bits to force ASCII)
export function readBlock32(text, index) {
    return ((text.charCodeAt(index++) & 255) << 24) + ((text.charCodeAt(index++) & 255) << 16) + ((text.charCodeAt(index++) & 255) << 8) + (text.charCodeAt(index++) & 255);
}

// reads 64 bits as the left 32 bits and right 32 bits
export function readBlock64(text, index) {
    return {
        left: readBlock32(text, index),
        right: readBlock32(text, index + 4)
    };
}

// convert 32 bit block into single string of 4 ASCII characters
export function block32ToString(block) {
    return String.fromCharCode(block >> 24) + String.fromCharCode((block << 8) >> 24) + String.fromCharCode((block << 16) >> 24) + String.fromCharCode((block << 24) >> 24);    
}

// convert 64 bit block (in left and right parts) into single string of 8 ASCII characters
export function block64ToString(block) {
    return block32ToString(block.left) + block32ToString(block.right);
}

// use instead of % operator to handle negative numbers correctly
export function mod(x, m) {
    return Math.abs(x) % m;
}

export function mask(n, isRight) {
    var mask = 0;
    // fill the appropriate number of bits
    for (var i = 0; i < n; i++) {
        mask |= (1 << i);
    }
    if (isRight) return mask;
    else return mask << (32 - n);
}

// left circular bitwise roration
export function leftRotate64(block, r) {
    // clamp r: 0 <= r <= 32
    r = Math.min(Math.max(r, 0), 32);

    if (r === 32) {
        let temp = block.left;
        block.left = block.right;
        block.right = temp;
        return block;
    }

    // js has no bitwise circular rotation
    // so I use bitmasks to pull the end bits out and then bitshift
    // but first I have to create the bitmask to pull out the bits that rotate
    const bitmask = mask(r);
    const _r = 32 - r;
    const leftEnd = (block.left & bitmask) >>> _r;
    const rightEnd = (block.right & bitmask) >>> _r;
    block.left = ( block.left << r ) | rightEnd;
    block.right = ( block.right << r ) | leftEnd;
    return block;
}

// right circular bitwise roration
export function rightRotate64(block, r) {
    // clamp r: 0 <= r <= 32
    r = Math.min(Math.max(r, 0), 32);

    if (r === 32) {
        let temp = block.left;
        block.left = block.right;
        block.right = temp;
        return block;
    }
    
    // js has no bitwise circular rotation
    // so I use bitmasks to pull the end bits out and then bitshift
    // but first I have to create the bitmask to pull out the bits that rotate
    const bitmask = mask(r, true);
    const _r = 32 - r;
    const leftEnd = (block.left & bitmask) << _r;
    const rightEnd = (block.right & bitmask) << _r;
    block.left = ( block.left >>> r ) | rightEnd;
    block.right = ( block.right >>> r ) | leftEnd;
    return block;
}