#include <sdw.h>

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
	u32 Unknown0x18;
} SDW_GNUC_PACKED;

struct SGarcHeader0600 : public SGarcHeader
{
	u32 Unknown0x18;
	u32 Unknown0x1C;
	u32 Unknown0x20;
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
	u32 IndexSet;
	SFileRecord FileRecord[1];
} SDW_GNUC_PACKED;

struct SFimbHeader
{
	u32 Signature;
	u32 Size;
	u32 DataSize;
} SDW_GNUC_PACKED;
#include SDW_MSC_POP_PACKED

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
	u32* pFatoOffset = reinterpret_cast<u32*>(pFatoHeader + 1);
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
	fclose(fp);
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
