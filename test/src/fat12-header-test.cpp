#include <gtest/gtest.h>
#include "test-utils.hpp"

extern "C" {
#include "fat12-header.h"
}

TEST(ParseMbrTest, HandlesDisk2Mbr) {
    unsigned char block[512];
    loadFile("test/disk/mbr.img", block);
    Fat12Header mbr = parseMbr(block);
    const char BS_OEMName[] = "MSDOS5.0";
    MY_EXPECT_STREQ(mbr.BS_OEMName, BS_OEMName, 8);
    EXPECT_EQ(mbr.BPB_BytsPerSec, 0x200);
    EXPECT_EQ(mbr.BPB_SecPerClus, 0x01);
    EXPECT_EQ(mbr.BPB_RsvdSecCnt, 0x01);
    EXPECT_EQ(mbr.BPB_NumFATs, 0x02);
    EXPECT_EQ(mbr.BPB_RootEntCnt, 0xE0);
    EXPECT_EQ(mbr.BPB_ToSec16, 0xb40);
    EXPECT_EQ(mbr.BPB_Media, 0xf0);
    EXPECT_EQ(mbr.BPB_FATSz16, 0x09);
    EXPECT_EQ(mbr.BPB_SecPerTrk, 0x12);
    EXPECT_EQ(mbr.BPB_NumHeads, 0x02);
    EXPECT_EQ(mbr.BPB_HiddSec, 0x00);
    EXPECT_EQ(mbr.BPB_TotSec32, 0x0);
    EXPECT_EQ(mbr.BS_DrvNum, 0x0);
    EXPECT_EQ(mbr.BS_Reserved1, 0x0);
    EXPECT_EQ(mbr.BS_BootSig, 0x29);
    EXPECT_EQ(mbr.BS_VolID, 0x36540cf3);
    MY_EXPECT_STREQ(mbr.BS_VolLab, "TEST-DISK", 11);
    MY_EXPECT_STREQ(mbr.BS_FileSysType, "FAT12", 8);
}
