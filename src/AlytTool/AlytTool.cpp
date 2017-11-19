#include <sdw.h>

#include SDW_MSC_PUSH_PACKED
struct SAlytHeader
{
	u32 Signature;
	u16 Unknown0x4;
	u16 Unknown0x6;
	u32 LtblOffset;
	u32 LtblSize;
	u32 LmtlOffset;
	u32 LmtlSize;
	u32 LfnlOffset;
	u32 LfnlSize;
	u32 A4Offset;
	u32 A4Size;
} SDW_GNUC_PACKED;

struct SDarcHeader
{
	u32 Signature;
	u16 ByteOrder;
	u16 HeaderSize;
	u32 Version;
	u32 FileSize;
	u32 EntryOffset;
	u32 EntrySize;
	u32 DataOffset;
} SDW_GNUC_PACKED;

struct SSarcHeader
{
	u32 Signature;
	u16 HeaderSize;
	u16 ByteOrder;
	u32 FileSize;
	u32 DataOffset;
	u16 Version;
	u16 Reserved;
} SDW_GNUC_PACKED;

struct SFimbHeader
{
	u32 Signature;
	u32 Size;
	u32 DataSize;
} SDW_GNUC_PACKED;

struct SBclimHeader
{
	u32 Signature;
	u16 ByteOrder;
	u16 HeaderSize;
	u32 Version;
	u32 FileSize;
	u16 DataBlocks;
	u16 Reserved;
} SDW_GNUC_PACKED;

struct SBclimImageBlock
{
	u32 Signature;
	u32 HeaderSize;
	u16 Width;
	u16 Height;
	u32 Format;
	u32 ImageSize;
} SDW_GNUC_PACKED;

struct SBflimHeader
{
	u32 Signature;
	u16 ByteOrder;
	u16 HeaderSize;
	u32 Version;
	u32 FileSize;
	u16 DataBlocks;
	u16 Reserved;
} SDW_GNUC_PACKED;

struct SBflimImageBlock
{
	u32 Signature;
	u32 HeaderSize;
	u16 Width;
	u16 Height;
	u16 Alignment;
	u8 Format;
	u8 Flag;
	u32 ImageSize;
} SDW_GNUC_PACKED;
#include SDW_MSC_POP_PACKED

const char* getExt(const UString& a_sPath, const u8* a_pData, u32 a_uDataSize)
{
	if (a_uDataSize < 4)
	{
		return "bin";
	}
	u32 uExtU32 = *reinterpret_cast<const u32*>(a_pData);
	if (uExtU32 == SDW_CONVERT_ENDIAN32('ALYT'))
	{
		return "alyt";
	}
	if (uExtU32 == SDW_CONVERT_ENDIAN32('BCH\0'))
	{
		return "bch";
	}
	if (uExtU32 == SDW_CONVERT_ENDIAN32('CGFX'))
	{
		return "bcres";
	}
	if (uExtU32 == SDW_CONVERT_ENDIAN32('CSEQ'))
	{
		return "cseq";
	}
	if (uExtU32 == SDW_CONVERT_ENDIAN32('CWAR'))
	{
		return "cwar";
	}
	if (uExtU32 == SDW_CONVERT_ENDIAN32('DARC'))
	{
		return "darc";
	}
	if (uExtU32 == SDW_CONVERT_ENDIAN32('DVLB'))
	{
		return "shbin";
	}
	if (uExtU32 == SDW_CONVERT_ENDIAN32('FFNT'))
	{
		return "bffnt";
	}
	if (uExtU32 == SDW_CONVERT_ENDIAN32('GFBE'))
	{
		return "gfbe";
	}
	if (uExtU32 == SDW_CONVERT_ENDIAN32('SARC'))
	{
		return "sarc";
	}
	if (uExtU32 == SDW_CONVERT_ENDIAN32('SESD'))
	{
		return "sesd";
	}
	if (uExtU32 == SDW_CONVERT_ENDIAN32('SPBD'))
	{
		return "spbd";
	}
	if (uExtU32 == 0x15041213)
	{
		return "t4";
	}
	if (a_uDataSize >= sizeof(SBclimHeader) + sizeof(SBclimImageBlock))
	{
		const SBclimHeader* pBclimHeader = reinterpret_cast<const SBclimHeader*>(a_pData + a_uDataSize - (sizeof(SBclimHeader) + sizeof(SBclimImageBlock)));
		if (pBclimHeader->Signature == SDW_CONVERT_ENDIAN32('CLIM') && pBclimHeader->FileSize == a_uDataSize)
		{
			return "bclim";
		}
	}
	if (a_uDataSize >= sizeof(SBflimHeader) + sizeof(SBflimImageBlock))
	{
		const SBflimHeader* pBflimHeader = reinterpret_cast<const SBflimHeader*>(a_pData + a_uDataSize - (sizeof(SBflimHeader) + sizeof(SBflimImageBlock)));
		if (pBflimHeader->Signature == SDW_CONVERT_ENDIAN32('FLIM') && pBflimHeader->FileSize == a_uDataSize)
		{
			return "bflim";
		}
	}
	{
		u32 uCount = *reinterpret_cast<const u16*>(a_pData + 2);
		if (4 + (uCount + 1) * 4 <= a_uDataSize)
		{
			u32 uFileSize = *reinterpret_cast<const u32*>(a_pData + 4 + uCount * 4);
			if (uFileSize == a_uDataSize)
			{
				return "a2";
			}
		}
	}
	do
	{
		u32 uCount0 = *reinterpret_cast<const u32*>(a_pData);
		if (uCount0 == 0 || 4 + uCount0 * 0x40 + 4 > a_uDataSize)
		{
			break;
		}
		u32 uCount1 = *reinterpret_cast<const u32*>(a_pData + 4 + uCount0 * 0x40);
		if (uCount1 == 0 || 4 + uCount0 * 0x40 + 4 + uCount1 * 0x20 > a_uDataSize)
		{
			break;
		}
		bool bMatch = true;
		for (u32 i = 0; i < uCount0; i++)
		{
			u32 uLength = static_cast<u32>(strlen(reinterpret_cast<const char*>(a_pData + 4 + i * 0x40)));
			if (uLength >= 0x40)
			{
				bMatch = false;
				break;
			}
			for (u32 j = uLength; j < 0x40; j++)
			{
				if (a_pData[4 + i * 0x40 + j] != 0)
				{
					bMatch = false;
					break;
				}
			}
			if (!bMatch)
			{
				break;
			}
		}
		if (!bMatch)
		{
			break;
		}
		for (u32 i = 0; i < uCount1; i++)
		{
			u32 uLength = static_cast<u32>(strlen(reinterpret_cast<const char*>(a_pData + 4 + uCount0 * 0x40 + 4 + i * 0x20)));
			if (uLength >= 0x20)
			{
				bMatch = false;
				break;
			}
			for (u32 j = uLength; j < 0x20; j++)
			{
				if (a_pData[4 + uCount0 * 0x40 + 4 + i * 0x20 + j] != 0)
				{
					bMatch = false;
					break;
				}
			}
			if (!bMatch)
			{
				break;
			}
		}
		if (!bMatch)
		{
			break;
		}
		return "a4";
	} while (false);
	return "bin";
}

int unpackAlyt(const UChar* a_pAlytFileName, const UChar* a_pArcFileNamePrefix)
{
	FILE* fp = UFopen(a_pAlytFileName, USTR("rb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uAlytSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	u8* pAlyt = new u8[uAlytSize];
	fread(pAlyt, 1, uAlytSize, fp);
	fclose(fp);
	SAlytHeader* pAlytHeader = reinterpret_cast<SAlytHeader*>(pAlyt);
	if (pAlytHeader->Signature != SDW_CONVERT_ENDIAN32('ALYT'))
	{
		delete[] pAlyt;
		return 1;
	}
	if (pAlytHeader->Unknown0x4 != 2)
	{
		delete[] pAlyt;
		return 1;
	}
	if (pAlytHeader->Unknown0x6 != 4)
	{
		delete[] pAlyt;
		return 1;
	}
	if (pAlytHeader->LtblOffset != sizeof(SAlytHeader))
	{
		delete[] pAlyt;
		return 1;
	}
	if (pAlytHeader->LmtlOffset != pAlytHeader->LtblOffset + pAlytHeader->LtblSize)
	{
		delete[] pAlyt;
		return 1;
	}
	if (pAlytHeader->LfnlOffset != pAlytHeader->LmtlOffset + pAlytHeader->LmtlSize)
	{
		delete[] pAlyt;
		return 1;
	}
	if (pAlytHeader->A4Offset != static_cast<u32>(Align(pAlytHeader->LfnlOffset + pAlytHeader->LfnlSize, 0x80)))
	{
		delete[] pAlyt;
		return 1;
	}
	u32 uA4Size = pAlytHeader->A4Size;
	u8* pA4 = pAlyt + pAlytHeader->A4Offset;
	u32 uCount0 = *reinterpret_cast<u32*>(pA4);
	u32 uCount1 = *reinterpret_cast<u32*>(pA4 + 4 + uCount0 * 0x40);
	u32 uDataOffset = static_cast<u32>(Align(4 + uCount0 * 0x40 + 4 + uCount1 * 0x20, 0x80));
	u32 uDataSize = uA4Size - uDataOffset;
	u8* pData = pA4 + uDataOffset;
	UString sFileName = a_pArcFileNamePrefix;
	const char* pFileExt = getExt(sFileName, pData, uDataSize);
	if (strcmp(pFileExt, "darc") != 0 && strcmp(pFileExt, "sarc") != 0)
	{
		delete[] pAlyt;
		return 1;
	}
	sFileName += USTR(".") + AToU(pFileExt);
	if (strcmp(pFileExt, "darc") == 0)
	{
		SDarcHeader* pDarcHeader = reinterpret_cast<SDarcHeader*>(pData);
		if (pDarcHeader->FileSize != uDataSize)
		{
			delete[] pAlyt;
			return 1;
		}
	}
	else if (strcmp(pFileExt, "sarc") == 0)
	{
		SSarcHeader* pSarcHeader = reinterpret_cast<SSarcHeader*>(pData);
		if (pSarcHeader->FileSize != uDataSize)
		{
			delete[] pAlyt;
			return 1;
		}
	}
	FILE* fpSub = UFopen(sFileName.c_str(), USTR("wb"), false);
	if (fpSub == nullptr)
	{
		delete[] pAlyt;
		return 1;
	}
	fwrite(pData, 1, uDataSize, fpSub);
	fclose(fpSub);
	delete[] pAlyt;
	return 0;
}

int packAlyt(const UChar* a_pAlytFileName, const UChar* a_pArcFileNamePrefix)
{
	return 0;
}

int UMain(int argc, UChar* argv[])
{
	if (argc != 4)
	{
		return 1;
	}
	if (UCslen(argv[1]) == 1)
	{
		switch (*argv[1])
		{
		case USTR('U'):
		case USTR('u'):
			return unpackAlyt(argv[2], argv[3]);
		case USTR('P'):
		case USTR('p'):
			return packAlyt(argv[2], argv[3]);
		default:
			break;
		}
	}
	return 1;
}
