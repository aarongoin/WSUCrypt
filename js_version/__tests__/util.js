import * as util from '../src/util';

describe('mod function', () => {
    test('it works a mod function should', () => {
        expect(util.mod(2, 1)).toBe(0);
        expect(util.mod(5, 2)).toBe(1);
    })
    test('it can handle negative numbers to mod', () => {
        expect(util.mod(-3, 2)).toBe(1);
        expect(util.mod(-2, 1)).toBe(0);
    })
    test('it can handle negative mod numbers', () => {
        expect(util.mod(3, -2)).toBe(1);
    })
});

describe('mask function', () => {
    test('it can make left-side masks, ex. 1110...0', () => {
        expect(util.mask(1, false)).toBe(-2147483648);
        expect(util.mask(3, false)).toBe(-536870912);
        expect(util.mask(31, false)).toBe(-2);
    })
    test('it can make right-side masks, ex. 0...0111', () => {
        expect(util.mask(1, true)).toBe(1);
        expect(util.mask(3, true)).toBe(7);
        expect(util.mask(31, true)).toBe(2147483647);
    })
});

describe('block32 functions', () => {
    test('it can read 32 bit blocks', () => {
        expect(util.readBlock32('abcdefgh', 0)).toBe(6357090);
        expect(util.readBlock32('abcdefgh', 4)).toBe(6619238);
    })
    test('it can write 32 bit blocks back as strings', () => {
        expect(util.block32ToString(6357090)).toBe('ab');
        expect(util.block32ToString(6619238)).toBe('ef');
    })
});

describe('block64 functions', () => {
    test('it can read 64 bit blocks as left 32 bits and right 32 bits', () => {
        expect(util.readBlock64('abcdefgh', 0)).toMatchObject({ left: 6357090, right: 6488164 });
    })
    test('it can write a 64 bit block { left, right } as a string', () => {
        expect(util.block64ToString({ left: 6357090, right: 6488164 })).toBe('abcd');
    })
});

describe('buffer functions', () => {
    test('it can read and write strings to arraybuffers', () => {
        expect(util.bufferToString(util.stringToBuffer("abcdefg"))).toBe("abcdefg");
    })
});

describe('block64 circular rotation function', () => {
    test('it can left circular rotate a block64', () => {
        expect(util.leftRotate64({left: 1431655765, right: 1431655765 }, 2)).toMatchObject({left: 1431655765, right: 1431655765 });
        expect(util.leftRotate64({left: 1431655765, right: 1431655765 }, 1)).toMatchObject({left: -1431655766, right: -1431655766 });
        expect(util.leftRotate64({left: -16777216, right: 16777215 }, 8)).toMatchObject({left: 0, right: -1 });
        expect(util.leftRotate64({left: -134217728, right: 0}, 5)).toMatchObject({left: 0, right: 31});
        expect(util.leftRotate64(util.leftRotate64({left: -1, right: 0 }, 31), 1)).toMatchObject({left: 0, right: -1 });
        expect(util.leftRotate64({left: -1, right: 0 }, 32)).toMatchObject({left: 0, right: -1 });        
    })
    test('it can right circular rotate a block64', () => {
        expect(util.rightRotate64({left: 1431655765, right: 1431655765 }, 2)).toMatchObject({left: 1431655765, right: 1431655765 });
        expect(util.rightRotate64({left: 1431655765, right: 1431655765 }, 1)).toMatchObject({left: -1431655766, right: -1431655766 });
    })
});