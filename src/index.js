import { readBlock64, block64ToString, mod, leftRotate64 } from './util';

// SKIPJACK F-table
// converted from hex strings: ['0xa3','0xd7','0x09','0x83','0xf8','0x48','0xf6','0xf4','0xb3','0x21','0x15','0x78','0x99','0xb1','0xaf','0xf9','0xe7','0x2d','0x4d','0x8a','0xce','0x4c','0xca','0x2e','0x52','0x95','0xd9','0x1e','0x4e','0x38','0x44','0x28','0x0a','0xdf','0x02','0xa0','0x17','0xf1','0x60','0x68','0x12','0xb7','0x7a','0xc3','0xe9','0xfa','0x3d','0x53','0x96','0x84','0x6b','0xba','0xf2','0x63','0x9a','0x19','0x7c','0xae','0xe5','0xf5','0xf7','0x16','0x6a','0xa2','0x39','0xb6','0x7b','0x0f','0xc1','0x93','0x81','0x1b','0xee','0xb4','0x1a','0xea','0xd0','0x91','0x2f','0xb8','0x55','0xb9','0xda','0x85','0x3f','0x41','0xbf','0xe0','0x5a','0x58','0x80','0x5f','0x66','0x0b','0xd8','0x90','0x35','0xd5','0xc0','0xa7','0x33','0x06','0x65','0x69','0x45','0x00','0x94','0x56','0x6d','0x98','0x9b','0x76','0x97','0xfc','0xb2','0xc2','0xb0','0xfe','0xdb','0x20','0xe1','0xeb','0xd6','0xe4','0xdd','0x47','0x4a','0x1d','0x42','0xed','0x9e','0x6e','0x49','0x3c','0xcd','0x43','0x27','0xd2','0x07','0xd4','0xde','0xc7','0x67','0x18','0x89','0xcb','0x30','0x1f','0x8d','0xc6','0x8f','0xaa','0xc8','0x74','0xdc','0xc9','0x5d','0x5c','0x31','0xa4','0x70','0x88','0x61','0x2c','0x9f','0x0d','0x2b','0x87','0x50','0x82','0x54','0x64','0x26','0x7d','0x03','0x40','0x34','0x4b','0x1c','0x73','0xd1','0xc4','0xfd','0x3b','0xcc','0xfb','0x7f','0xab','0xe6','0x3e','0x5b','0xa5','0xad','0x04','0x23','0x9c','0x14','0x51','0x22','0xf0','0x29','0x79','0x71','0x7e','0xff','0x8c','0x0e','0xe2','0x0c','0xef','0xbc','0x72','0x75','0x6f','0x37','0xa1','0xec','0xd3','0x8e','0x62','0x8b','0x86','0x10','0xe8','0x08','0x77','0x11','0xbe','0x92','0x4f','0x24','0xc5','0x32','0x36','0x9d','0xcf','0xf3','0xa6','0xbb','0xac', '0x5e','0x6c','0xa9','0x13','0x57','0x25','0xb5','0xe3','0xbd','0xa8','0x3a','0x01','0x05','0x59','0x2a','0x46']
var f_table = [163,215,9,131,248,72,246,244,179,33,21,120,153,177,175,249,231,45,77,138,206,76,202,46,82,149,217,30,78,56,68,40,10,223,2,160,23,241,96,104,18,183,122,195,233,250,61,83,150,132,107,186,242,99,154,25,124,174,229,245,247,22,106,162,57,182,123,15,193,147,129,27,238,180,26,234,208,145,47,184,85,185,218,133,63,65,191,224,90,88,128,95,102,11,216,144,53,213,192,167,51,6,101,105,69,0,148,86,109,152,155,118,151,252,178,194,176,254,219,32,225,235,214,228,221,71,74,29,66,237,158,110,73,60,205,67,39,210,7,212,222,199,103,24,137,203,48,31,141,198,143,170,200,116,220,201,93,92,49,164,112,136,97,44,159,13,43,135,80,130,84,100,38,125,3,64,52,75,28,115,209,196,253,59,204,251,127,171,230,62,91,165,173,4,35,156,20,81,34,240,41,121,113,126,255,140,14,226,12,239,188,114,117,111,55,161,236,211,142,98,139,134,16,232,8,119,17,190,146,79,36,197,50,54,157,207,243,166,187,172,94,108,169,19,87,37,181,227,189,168,58,1,5,89,42,70];

function FTable(word_8b) {
    var row = (word_8b >> 4) * 16,
        col = (word_8b << 28) >> 28;

    return f_table[row + col];
}

export function subkey(key, x) {
    // left circular rotate
    leftRotate64(key, 1);
    // get byte index to pull out
    var b = 7 - mod(x, 8);
    // bytes 0-3 are in key.left, and 4-7 are in key.right
    if (b < 4) {
        return ( key.left << (8 * b) ) >>> 24;
    } else { // 4 <= b <= 7
        return ( key.right << (8 * (b - 4)) ) >>> 24;
    }
}

export function subkeys(splitKey, rounds) {
    var keyList = [],
        roundKey = Object.assign({}, splitKey); // copy splitKey to preserve the original

    for (var r = 0; r < rounds; r++) {
        // each round uses 12 subkeys
        for (var _ = 0; _ < 3; _++) {
            for (var i = 0; i < 4; i++) {
                keyList.push( subkey(roundKey, 4 * r + i) );
            }
        }
    }

    r = 0;
    keyList.next = () => r < keyList.length ? keyList[r++] : undefined;

    return keyList;
}

export function parseKey(key) {
   return {
        left: parseInt("0x" + key.slice(0, 8), 16),
        right: parseInt("0x" + key.slice(8, 16), 16)
    };
}

function G(word_16b, subkeys) {
    var g0 = word_16b >>> 8,
        g1 = word_16b & 255,
        g2 = FTable(g1 ^ subkeys.next()) ^ g0,
        g3 = FTable(g2 ^ subkeys.next()) ^ g1,
        g4 = FTable(g3 ^ subkeys.next()) ^ g2,
        g5 = FTable(g4 ^ subkeys.next()) ^ g3;

    return (g4 << 8) | g5;
}

function F(word_32b, subkeys) {
    var t0 = G(word_32b >>> 16, subkeys),
        t1 = G((word_32b << 16) >>> 16, subkeys),
        f0 = mod(t0 + 2 * t1 + subkeys.next() << 8 + subkeys.next(), 65536),
        f1 = mod(2 * t0 + t1 + subkeys.next() << 8 + subkeys.next(), 65536);

    return f0 << 16 | f1;
}

function encryptBlock(block, key, subkeys) {
    var temp;

    // input whitening step
    block.left ^= key.left;
    block.right ^= key.right;

    console.log(block.left + " " + block.right);

    // F rounds
    for (var r = 0; r < 16; r++)  {
        block.right ^= F(block.left, subkeys);
        // swap left and right sides of block if not the final round
        if (r < 15) {
            temp = block.right
            block.right = block.left;
            block.left = temp;
        }
    }

    // output whitening
    block.left ^= key.left;
    block.right ^= key.right;

    return block64ToString(block);
}

function WSUCrypt(plaintext, key, isDecrypt) {

    var ciphertext = "",
        splitKey = parseKey(key),
        keys = subkeys(splitKey, 16); // pregenerate all the subkeys

    // reverse order of subkeys to decrypt
    if (isDecrypt) keys.reverse();

    // pad plaintext so we can make even blocks of 64 bits
    while (plaintext.length % 8) plaintext += String.fromCharCode(0);

    // encrypt plaintext block by block
    const len = plaintext.length / 8;
    var i = -1;
    while (++i < len) {
        ciphertext += encryptBlock( readBlock64(plaintext, i * 8), splitKey, keys );
    }
    return ciphertext;
}

export const encrypt = (plaintext, key) => WSUCrypt(plaintext, key, false);
export const decrypt = (ciphertext, key) => WSUCrypt(ciphertext, key, true);