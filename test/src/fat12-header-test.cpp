#include <gtest/gtest.h>

extern "C" {
#include "fat12-header.h"
}

void MY_EXPECT_STREQ(const char *diskStr, const char *str, int size) {
    for (size_t offset = 0; offset < size; offset++) {
        if (offset < strlen(str))
            EXPECT_EQ(diskStr[offset], str[offset]);
        else
            EXPECT_EQ(diskStr[offset], ' ');
    }
}

TEST(ParseMbrTest, HandlesDisk2Mbr) {
    FILE *fp = fopen("test/disk/mbr.img", "rb");
    ASSERT_NE((long long int)fp, NULL) << "Can't find the disk file";
    unsigned char block[512];
    for (size_t i = 0; i < 512; i++) fscanf(fp, "%c", &block[i]);
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
