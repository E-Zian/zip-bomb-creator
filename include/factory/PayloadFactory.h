//
// Created by LeeEeZian on 30/9/2026.
//

#ifndef ZIP_BOMB_CREATOR_PAYLOADFACTORY_H
#define ZIP_BOMB_CREATOR_PAYLOADFACTORY_H

#include "deflation/HuffmanTable.h"

namespace payloadFactory {
    DeflateResult createFixedBombPayload(size_t size);

}
#endif //ZIP_BOMB_CREATOR_PAYLOADFACTORY_H
