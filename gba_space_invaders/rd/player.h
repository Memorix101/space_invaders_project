/**********************************************\
*       player.h                                   *
*          by dovotos pcx->gba program         *
/**********************************************/
#define  player_WIDTH   16
#define  player_HEIGHT  16


const u16 playerData[] = {
                    0x0000, 0x0000, 0x0000, 0x1505, 0x0540, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
                    0xFF00, 0x3217, 0x1732, 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x1716,
                    0x1617, 0xFF06, 0x0000, 0x0000, 0x0000, 0x0000, 0x1AC5, 0x221D, 0x1D22, 0x091A,
                    0x0000, 0x0000, 0x0000, 0x0000, 0x190C, 0xFE64, 0x64FE, 0x0C19, 0x0000, 0x0000,
                    0x0000, 0x0E00, 0x02C3, 0x5C62, 0x625C, 0xC302, 0x0091, 0x0000, 0x0000, 0x2C7D,
                    0x6549, 0x6262, 0x6262, 0x5765, 0xC32C, 0x0000, 0x0000, 0x19EF, 0x19ED, 0x5E4E,
                    0x4E5E, 0x3419, 0xF518, 0x0000, 0xC500, 0x1C4A, 0x1B1E, 0x3922, 0xBD39, 0x1E1C,
                    0x4A1C, 0x00C5, 0x19C5, 0x1412, 0x321C, 0x1617, 0x1716, 0x1C47, 0x1214, 0xC519,
                    0x19EF, 0x1C0C, 0x3E24, 0x1718, 0x1839, 0x1E4E, 0x4A1C, 0xEF19, 0xEF15, 0x1C12,
                    0x322E, 0x141D, 0x1D14, 0x2F32, 0x061C, 0x1525, 0x0000, 0x2F00, 0x7534, 0x06FF,
                    0xFFF5, 0x3E75, 0x152F, 0x0000, 0x0000, 0x0000, 0x7940, 0x4C4A, 0x064C, 0x4079,
                    0x0015, 0x0000, 0x0000, 0x0000, 0xFE05, 0xC3FF, 0xFF2F, 0x05FE, 0x0000, 0x0000,
                    0x0000, 0x0000, 0x1591, 0x0015, 0x9100, 0x1515, 0x0000, 0x0000,};

const u16 playerPalette[] = {
                    0x739F, 0x673D, 0x6B5E, 0x673D, 0x14A9, 0x0C66, 0x0847, 0x1CF4, 0x14B0, 0x0846,
                    0x18D3, 0x2119, 0x0425, 0x0017, 0x0004, 0x0007, 0x000E, 0x2136, 0x0C67, 0x5ADD,
                    0x1917, 0x0002, 0x423D, 0x3A1D, 0x1D7E, 0x195D, 0x00DD, 0x1115, 0x08F5, 0x2174,
                    0x0D13, 0x35F4, 0x31D3, 0x3E13, 0x2DB0, 0x110F, 0x04EF, 0x00CE, 0x3A12, 0x256D,
                    0x214C, 0x3A13, 0x3612, 0x56F9, 0x4EB7, 0x3A33, 0x14E9, 0x10C8, 0x110C, 0x0A3F,
                    0x3E53, 0x09DA, 0x110B, 0x0174, 0x0195, 0x15F6, 0x0193, 0x3277, 0x2A77, 0x1DF4,
                    0x19D3, 0x573A, 0x1D2A, 0x2ADA, 0x0863, 0x1F7F, 0x237F, 0x47BF, 0x218C, 0x4F18,
                    0x4EF7, 0x4AD6, 0x67FF, 0x25AD, 0x0084, 0x29CC, 0x320E, 0x10A4, 0x2146, 0x29A8,
                    0x2987, 0x5F13, 0x5F06, 0x5F23, 0x5F22, 0x5F41, 0x6322, 0x2D6B, 0x7FFB, 0x6F75,
                    0x6733, 0x6733, 0x5ACF, 0x462B, 0x462A, 0x3DE8, 0x630D, 0x5EEB, 0x5EEA, 0x3184,
                    0x2941, 0x77B9, 0x7FA2, 0x7322, 0x3DA1, 0x568F, 0x45E2, 0x7F62, 0x5249, 0x5247,
                    0x6F57, 0x6F35, 0x7757, 0x6ACF, 0x6626, 0x4D85, 0x65E4, 0x6F37, 0x76CE, 0x3525,
                    0x7358, 0x5671, 0x5A2D, 0x562C, 0x49A9, 0x20C4, 0x3D67, 0x6E8D, 0x1CA3, 0x55EA,
                    0x6E8C, 0x7EEE, 0x76AD, 0x4166, 0x7ECD, 0x3946, 0x51C8, 0x76AC, 0x7ECC, 0x1882,
                    0x3104, 0x520C, 0x6A4B, 0x2CE3, 0x1461, 0x1020, 0x1C81, 0x4DCB, 0x55EB, 0x55CA,
                    0x51CA, 0x65E8, 0x51EB, 0x1C83, 0x7A09, 0x75E7, 0x7DE7, 0x75C6, 0x7DE5, 0x7DA3,
                    0x7D82, 0x7D61, 0x4C83, 0x4463, 0x1C63, 0x2063, 0x1442, 0x1842, 0x1C42, 0x7063,
                    0x4442, 0x7C63, 0x6C42, 0x7C42, 0x4022, 0x1864, 0x6EFB, 0x6EFB, 0x6699, 0x0C24,
                    0x733D, 0x310E, 0x310E, 0x6B1B, 0x352F, 0x3950, 0x310F, 0x2CEE, 0x45F3, 0x3DB1,
                    0x356F, 0x45B5, 0x4194, 0x4595, 0x4595, 0x1CA9, 0x1868, 0x0C25, 0x0804, 0x6F3D,
                    0x49D6, 0x45B6, 0x1047, 0x0805, 0x4A14, 0x563B, 0x563B, 0x521B, 0x495E, 0x451D,
                    0x1C6D, 0x184C, 0x5A5B, 0x563B, 0x413C, 0x453D, 0x451E, 0x3CFB, 0x411D, 0x0C25,
                    0x1048, 0x1048, 0x1469, 0x1049, 0x287F, 0x245F, 0x1C72, 0x1C74, 0x1C75, 0x247D,
                    0x1854, 0x1855, 0x1856, 0x1C59, 0x205D, 0x1CCA, 0x1CAE, 0x146C, 0x146D, 0x104B,
                    0x104C, 0x1451, 0x1855, 0x24F4, 0x20B3, 0x080A, 0x24F5, 0x20D4, 0x20D5, 0x6F5D,
                    0x20D5, 0x080F, 0x210A, 0x0010, 0x7FFF, 0x0000,};