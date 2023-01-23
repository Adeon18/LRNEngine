#pragma once

namespace engn {

    //! A struct that storeal all data needed for ray cast
    struct RayCastData {
        float pixelWidth;
        float pixelHeight;

        float halfPixelWidth;
        float halfPixelHeight;

        float strideX;
        float strideY;
    };

} // engn