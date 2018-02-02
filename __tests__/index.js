import * as WSUCrypt from '../src/index';

describe('WSUCrypt key functions', () => {
    test('it can parse a hex string key into left and right bits', () => {
        expect(WSUCrypt.parseKey('abcdef0123456789')).toEqual({left: 2882400001, right: 591751049});
    })
    test('it can take a parsed key and generate all subkeys', () => {
        var expectedKeys = [19,158,43,52,53,226,179,69,87,38,60,86,104,72,128,239,138,141,9,240,172,209,145,1,222,55,94,154,224,123,230,171,2,188,111,188,53,226,179,69,87,38,60,86,121,106,196,103,138,141,9,240,172,209,145,1,207,21,26,18,224,123,230,171,2,188,111,188,36,192,247,205,87,38,60,86,121,106,196,103,155,175,77,120,172,209,145,1,207,21,26,18,241,89,162,35,2,188,111,188,36,192,247,205,70,4,120,222,121,106,196,103,155,175,77,120,189,243,213,137,207,21,26,18,241,89,162,35,19,158,43,52,36,192,247,205,70,4,120,222,104,72,128,239,155,175,77,120,189,243,213,137,222,55,94,154,241,89,162,35,19,158,43,52,53,226,179,69,70,4,120,222,104,72,128,239,138,141,9,240,189,243,213,137,222,55,94,154,224,123,230,171];
        var key = WSUCrypt.parseKey('abcdef0123456789');
        var keys = WSUCrypt.subkeys(key, 16);
        expect(keys).toEqual(expect.arrayContaining(expectedKeys));
        expect(keys.length).toBe(expectedKeys.length);
    })
});

describe('WSUCrypt functions', () => {
    test('it can decrypt ciphertext it has encrypted', () => {
        var plaintext = '0123456789abcdef';
        var key = 'abcdef0123456789';
        var ciphertext = WSUCrypt.encrypt(plaintext, key);
        console.log(ciphertext);

        expect(WSUCrypt.decrypt(ciphertext, key)).toMatch(plaintext);
    })
});
