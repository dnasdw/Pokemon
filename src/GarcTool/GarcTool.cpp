#include <sdw.h>
#include "lz77.h"

#include SDW_MSC_PUSH_PACKED
struct SGarcHeader
{
	u32 Signature;
	u32 HeaderSize;
	u16 ByteOrder;
	u16 Version;
	u16 DataBlocks;
	u16 Reserved;
	u32 DataOffset;
	u32 FileSize;
} SDW_GNUC_PACKED;

struct SGarcHeader0400 : public SGarcHeader
{
	u32 SubFileSizeMax;
} SDW_GNUC_PACKED;

struct SGarcHeader0600 : public SGarcHeader
{
	u32 SubFileSizeMaxAligned;
	u32 SubFileSizeMax;
	u32 Alignment;
} SDW_GNUC_PACKED;

struct SFatoHeader
{
	u32 Signature;
	u32 Size;
	u16 Count;
	u16 Unknown0xA;
} SDW_GNUC_PACKED;

struct SFatbHeader
{
	u32 Signature;
	u32 Size;
	u32 Count;
} SDW_GNUC_PACKED;

struct SFileRecord
{
	u32 BeginOffset;
	u32 EndOffset;
	u32 Size;
} SDW_GNUC_PACKED;

struct SFatbRecord
{
	u32 NumSet;
	SFileRecord FileRecord[1];
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

int unpackGarc(const UChar* a_pFileName, const UChar* a_pDirName)
{
	FILE* fp = UFopen(a_pFileName, USTR("rb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uGarcSize = ftell(fp);
	if (uGarcSize == 0)
	{
		fclose(fp);
		return 0;
	}
	fseek(fp, 0, SEEK_SET);
	SGarcHeader garcHeader;
	fread(&garcHeader, sizeof(SGarcHeader), 1, fp);
	SGarcHeader* pGarcHeader = reinterpret_cast<SGarcHeader*>(&garcHeader);
	if (pGarcHeader->Signature != 'GARC')
	{
		fclose(fp);
		return 1;
	}
	if (pGarcHeader->HeaderSize != sizeof(SGarcHeader0400) && pGarcHeader->HeaderSize != sizeof(SGarcHeader0600))
	{
		fclose(fp);
		return 1;
	}
	if (pGarcHeader->ByteOrder != 0xFEFF)
	{
		fclose(fp);
		return 1;
	}
	if (pGarcHeader->Version == 0x0400)
	{
		if (pGarcHeader->HeaderSize != sizeof(SGarcHeader0400))
		{
			fclose(fp);
			return 1;
		}
	}
	else if (pGarcHeader->Version == 0x0600)
	{
		if (pGarcHeader->HeaderSize != sizeof(SGarcHeader0600))
		{
			fclose(fp);
			return 1;
		}
	}
	else
	{
		fclose(fp);
		return 1;
	}
	if (pGarcHeader->DataBlocks != 4)
	{
		fclose(fp);
		return 1;
	}
	if (pGarcHeader->Reserved != 0)
	{
		fclose(fp);
		return 1;
	}
	if (pGarcHeader->FileSize != uGarcSize)
	{
		fclose(fp);
		return 1;
	}
	fseek(fp, 0, SEEK_SET);
	vector<u8> vGarcHeader(pGarcHeader->HeaderSize);
	fread(&*vGarcHeader.begin(), 1, pGarcHeader->HeaderSize, fp);
	pGarcHeader = reinterpret_cast<SGarcHeader*>(&*vGarcHeader.begin());
	u32 uAlignment = 0;
	u32 uSubFileSizeMaxAligned = 0;
	if (pGarcHeader->Version == 0x0400)
	{
		uAlignment = 4;
		uSubFileSizeMaxAligned = static_cast<u32>(Align(reinterpret_cast<SGarcHeader0400*>(pGarcHeader)->SubFileSizeMax, uAlignment));
	}
	else if (pGarcHeader->Version == 0x0600)
	{
		uAlignment = reinterpret_cast<SGarcHeader0600*>(pGarcHeader)->Alignment;
		if (uAlignment == 0)
		{
			fclose(fp);
			return 1;
		}
		uSubFileSizeMaxAligned = static_cast<u32>(Align(reinterpret_cast<SGarcHeader0600*>(pGarcHeader)->SubFileSizeMax, uAlignment));
		if (uSubFileSizeMaxAligned != reinterpret_cast<SGarcHeader0600*>(pGarcHeader)->SubFileSizeMaxAligned)
		{
			fclose(fp);
			return 1;
		}
	}
	SFatoHeader fatoHeader;
	fread(&fatoHeader, sizeof(SFatoHeader), 1, fp);
	SFatoHeader* pFatoHeader = &fatoHeader;
	if (pFatoHeader->Signature != 'FATO')
	{
		fclose(fp);
		return 1;
	}
	if (pFatoHeader->Size != sizeof(SFatoHeader) + pFatoHeader->Count * 4)
	{
		fclose(fp);
		return 1;
	}
	if (pFatoHeader->Unknown0xA != 0xFFFF)
	{
		fclose(fp);
		return 1;
	}
	fseek(fp, -static_cast<n32>(sizeof(SFatoHeader)), SEEK_CUR);
	vector<u8> vFato(pFatoHeader->Size);
	fread(&*vFato.begin(), 1, pFatoHeader->Size, fp);
	pFatoHeader = reinterpret_cast<SFatoHeader*>(&*vFato.begin());
	u32* pFatbOffset = reinterpret_cast<u32*>(pFatoHeader + 1);
	SFatbHeader fatbHeader;
	fread(&fatbHeader, sizeof(SFatbHeader), 1, fp);
	SFatbHeader* pFatbHeader = &fatbHeader;
	if (pFatbHeader->Signature != 'FATB')
	{
		fclose(fp);
		return 1;
	}
	fseek(fp, -static_cast<n32>(sizeof(SFatbHeader)), SEEK_CUR);
	vector<u8> vFatb(pFatbHeader->Size);
	fread(&*vFatb.begin(), 1, pFatbHeader->Size, fp);
	pFatbHeader = reinterpret_cast<SFatbHeader*>(&*vFatb.begin());
	SFimbHeader fimbHeader;
	fread(&fimbHeader, sizeof(SFimbHeader), 1, fp);
	SFimbHeader* pFimbHeader = &fimbHeader;
	if (pFimbHeader->Signature != 'FIMB')
	{
		fclose(fp);
		return 1;
	}
	if (pFimbHeader->Size != sizeof(SFimbHeader))
	{
		fclose(fp);
		return 1;
	}
	if (pGarcHeader->DataOffset + pFimbHeader->DataSize != uGarcSize)
	{
		fclose(fp);
		return 1;
	}
	// TODO: test begin
	UPrintf(USTR("%") PRIUS USTR("\n"), a_pFileName);
	// TODO: test end
	UMkdir(a_pDirName);
	u32 uSubFileSizeMax = 0;
	u32 uPrevOffset = 0;
	vector<u8> vData(uSubFileSizeMaxAligned);
	for (n32 i = 0; i < pFatoHeader->Count; i++)
	{
		SFatbRecord* pFatbRecord = reinterpret_cast<SFatbRecord*>(&*vFatb.begin() + sizeof(SFatbHeader) + pFatbOffset[i]);
		bitset<32> bsNum(pFatbRecord->NumSet);
		if (bsNum.none())
		{
			fclose(fp);
			return 1;
		}
		UString sDirName = a_pDirName;
		if (pFatbRecord->NumSet != 1)
		{
			sDirName += Format(USTR("/%d.dir"), i);
			UMkdir(sDirName.c_str());
		}
		n32 nIndex = 0;
		for (n32 j = 0; j < 32; j++)
		{
			if (bsNum[j] != 0)
			{
				SFileRecord* pFileRecord = &pFatbRecord->FileRecord[nIndex++];
				if (pFileRecord->Size == 0 && pFileRecord->EndOffset != pFileRecord->BeginOffset)
				{
					fclose(fp);
					return 1;
				}
				if (static_cast<u32>(Align(pFileRecord->BeginOffset + pFileRecord->Size, uAlignment)) != pFileRecord->EndOffset)
				{
					fclose(fp);
					return 1;
				}
				if (pFileRecord->BeginOffset != uPrevOffset)
				{
					fclose(fp);
					return 1;
				}
				uPrevOffset = pFileRecord->EndOffset;
				if (pFileRecord->Size > uSubFileSizeMax)
				{
					uSubFileSizeMax = pFileRecord->Size;
				}
				if (pFileRecord->Size != 0)
				{
					fseek(fp, pGarcHeader->DataOffset + pFileRecord->BeginOffset, SEEK_SET);
					fread(&*vData.begin(), 1, pFileRecord->Size, fp);
					u8* pData = &*vData.begin();
					u32 uDataSize = pFileRecord->Size;
					do
					{
						if (pData[0] == 0x11)
						{
							u32 uUncompressedSize = 0;
							if (!CLz77::GetUncompressedSize(pData, pFileRecord->Size, uUncompressedSize))
							{
								break;
							}
							u8* pUncompressed = nullptr;
							try
							{
								pUncompressed = new u8[uUncompressedSize];
							}
							catch (bad_alloc&)
							{
								break;
							}
							if (pUncompressed == nullptr)
							{
								break;
							}
							if (!CLz77::Uncompress(pData, pFileRecord->Size, pUncompressed, uUncompressedSize))
							{
								delete[] pUncompressed;
								break;
							}
							pData = pUncompressed;
							uDataSize = uUncompressedSize;
						}
					} while (false);
					UString sFileName = sDirName + USTR("/");
					if (pFatbRecord->NumSet == 1)
					{
						sFileName += Format(USTR("%d"), i);
					}
					else
					{
						sFileName += Format(USTR("%d"), j);
					}
					const char* pFileExt = getExt(sFileName, pData, uDataSize);
					// TODO: test begin
					if (strcmp(pFileExt, "bin") == 0 && uDataSize >= 16)
					{
						for (u32 k = 0; k < 16; k++)
						{
							UPrintf(USTR("%02X "), pData[k]);
						}
						for (u32 k = 0; k < 16; k++)
						{
							if (isprint(pData[k]))
							{
								printf("%c", pData[k]);
							}
							else
							{
								printf(" ");
							}
						}
						UPrintf(USTR("\n"));
					}
					// TODO: test end
					sFileName += USTR(".") + AToU(pFileExt);
					FILE* fpSub = UFopen(sFileName.c_str(), USTR("wb"), false);
					if (fpSub == nullptr)
					{
						if (pData != &*vData.begin())
						{
							delete[] pData;
						}
						fclose(fp);
						return 1;
					}
					fwrite(pData, 1, uDataSize, fpSub);
					fclose(fpSub);
					if (pData != &*vData.begin())
					{
						delete[] pData;
					}
				}
				else
				{
					UString sFileName = sDirName + USTR("/");
					if (pFatbRecord->NumSet == 1)
					{
						sFileName += Format(USTR("%d.bin"), i);
					}
					else
					{
						sFileName += Format(USTR("%d.bin"), j);
					}
					FILE* fpSub = UFopen(sFileName.c_str(), USTR("wb"), false);
					if (fpSub == nullptr)
					{
						fclose(fp);
						return 1;
					}
					fclose(fpSub);
				}
			}
		}
	}
	fclose(fp);
	if (pGarcHeader->Version == 0x0400)
	{
		if (reinterpret_cast<SGarcHeader0400*>(pGarcHeader)->SubFileSizeMax != uSubFileSizeMax)
		{
			return 1;
		}
	}
	else if (pGarcHeader->Version == 0x0600)
	{
		if (reinterpret_cast<SGarcHeader0600*>(pGarcHeader)->SubFileSizeMax != uSubFileSizeMax)
		{
			return 1;
		}
	}
	return 0;
}

int packGarc(const UChar* a_pFileName, const UChar* a_pDirName)
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
			return unpackGarc(argv[2], argv[3]);
		case USTR('P'):
		case USTR('p'):
			return packGarc(argv[2], argv[3]);
		default:
			break;
		}
	}
	return 1;
}
