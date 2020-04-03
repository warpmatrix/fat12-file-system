#include <gtest/gtest.h>

extern "C" {
#include "fat.h"
}

TEST(InitFatTest, HandlesStartUpFlp) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/startup.flp");
    EXPECT_EQ(res, SIZE);

    res = initFat(ramFDD144);
    EXPECT_EQ(res, 0);
    
    const unsigned char *fat = getFat();
    size_t fat1BaseSec = 1, fat2BaseSec = 10;
    for (size_t secOfst = 0; secOfst < 9; secOfst++)
        for (size_t offset = 0; offset < BLOCKSIZE; offset++) {
            EXPECT_EQ(ramFDD144[(fat1BaseSec + secOfst) * BLOCKSIZE + offset],
                      fat[secOfst * BLOCKSIZE + offset]);
            EXPECT_EQ(ramFDD144[(fat2BaseSec + secOfst) * BLOCKSIZE + offset],
                      fat[secOfst * BLOCKSIZE + offset]);
        }
}

TEST(GetNextClusTest, HandlesIOSYS) {
    
}
