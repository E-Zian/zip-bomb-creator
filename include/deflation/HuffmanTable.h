//
// Created by User on 9/24/2026.
//

#ifndef ZIP_BOMB_CREATOR_HUFFMANTABLE_H
#define ZIP_BOMB_CREATOR_HUFFMANTABLE_H

class HuffmanTable {
public:
    enum BTYPE {
        STORED = 0b00,
        FIXED_HUFFMAN = 0b01,
        DYNAMIC_HUFFMAN = 0b10,
        ERROR = 0b11
    };

    enum BFINAL {
        NO = 0b00,
        YES = 0b01,
    };

private:
};
#endif //ZIP_BOMB_CREATOR_HUFFMANTABLE_H
