#include <sdw.h>
#include <png.h>
#include <PVRTextureUtilities.h>

#include SDW_MSC_PUSH_PACKED
struct ST4Header0
{
	u32 Unknown0x0;
	u32 Unknown0x4;
	char Unknown0x8[8];
	u32 DataSize;
	u32 Unknown0x14;
} SDW_GNUC_PACKED;

struct ST4Header1
{
	u32 ImageSize;
	u32 Unknown0x4;
	u32 Unknown0x8;
	u32 Unknown0xC;
	char FileName[0x40];
	u16 Width;
	u16 Height;
	u16 Format;
	u16 MipmapLevel;
	u32 Unknown0x58[4];
} SDW_GNUC_PACKED;
#include SDW_MSC_POP_PACKED

enum ETextureFormat
{
	kTextureFormatRGBA8888 = 4,
	kTextureFormatRGB888 = -1,
	kTextureFormatRGBA5551 = 23,
	kTextureFormatRGB565 = -3,
	kTextureFormatRGBA4444 = 22,
	kTextureFormatLA88 = -5,
	kTextureFormatHL8 = -6,
	kTextureFormatL8 = -7,
	kTextureFormatA8 = 38,
	kTextureFormatLA44 = -9,
	kTextureFormatL4 = -10,
	kTextureFormatA4 = -11,
	kTextureFormatETC1 = -12,
	kTextureFormatETC1_A4 = 43
};

static const n32 s_nDecodeTransByte[64] =
{
	 0,  1,  4,  5, 16, 17, 20, 21,
	 2,  3,  6,  7, 18, 19, 22, 23,
	 8,  9, 12, 13, 24, 25, 28, 29,
	10, 11, 14, 15, 26, 27, 30, 31,
	32, 33, 36, 37, 48, 49, 52, 53,
	34, 35, 38, 39, 50, 51, 54, 55,
	40, 41, 44, 45, 56, 57, 60, 61,
	42, 43, 46, 47, 58, 59, 62, 63
};

int decode(u8* a_pBuffer, n32 a_nWidth, n32 a_nHeight, n32 a_nFormat, pvrtexture::CPVRTexture** a_pPVRTexture)
{
	u8* pRGBA = nullptr;
	u8* pAlpha = nullptr;
	switch (a_nFormat)
	{
	case kTextureFormatRGBA8888:
		{
			u8* pTemp = new u8[a_nWidth * a_nHeight * 4];
			for (n32 i = 0; i < a_nWidth * a_nHeight / 64; i++)
			{
				for (n32 j = 0; j < 64; j++)
				{
					for (n32 k = 0; k < 4; k++)
					{
						pTemp[(i * 64 + j) * 4 + k] = a_pBuffer[(i * 64 + s_nDecodeTransByte[j]) * 4 + 3 - k];
					}
				}
			}
			pRGBA = new u8[a_nWidth * a_nHeight * 4];
			for (n32 i = 0; i < a_nHeight; i++)
			{
				for (n32 j = 0; j < a_nWidth; j++)
				{
					for (n32 k = 0; k < 4; k++)
					{
						pRGBA[(i * a_nWidth + j) * 4 + k] = pTemp[(((i / 8) * (a_nWidth / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 4 + k];
					}
				}
			}
			delete[] pTemp;
		}
		break;
	case kTextureFormatRGB888:
		{
			u8* pTemp = new u8[a_nWidth * a_nHeight * 3];
			for (n32 i = 0; i < a_nWidth * a_nHeight / 64; i++)
			{
				for (n32 j = 0; j < 64; j++)
				{
					for (n32 k = 0; k < 3; k++)
					{
						pTemp[(i * 64 + j) * 3 + k] = a_pBuffer[(i * 64 + s_nDecodeTransByte[j]) * 3 + 2 - k];
					}
				}
			}
			pRGBA = new u8[a_nWidth * a_nHeight * 3];
			for (n32 i = 0; i < a_nHeight; i++)
			{
				for (n32 j = 0; j < a_nWidth; j++)
				{
					for (n32 k = 0; k < 3; k++)
					{
						pRGBA[(i * a_nWidth + j) * 3 + k] = pTemp[(((i / 8) * (a_nWidth / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 3 + k];
					}
				}
			}
			delete[] pTemp;
		}
		break;
	case kTextureFormatRGBA5551:
	case kTextureFormatRGB565:
	case kTextureFormatRGBA4444:
		{
			u8* pTemp = new u8[a_nWidth * a_nHeight * 2];
			for (n32 i = 0; i < a_nWidth * a_nHeight / 64; i++)
			{
				for (n32 j = 0; j < 64; j++)
				{
					for (n32 k = 0; k < 2; k++)
					{
						pTemp[(i * 64 + j) * 2 + k] = a_pBuffer[(i * 64 + s_nDecodeTransByte[j]) * 2 + k];
					}
				}
			}
			pRGBA = new u8[a_nWidth * a_nHeight * 2];
			for (n32 i = 0; i < a_nHeight; i++)
			{
				for (n32 j = 0; j < a_nWidth; j++)
				{
					for (n32 k = 0; k < 2; k++)
					{
						pRGBA[(i * a_nWidth + j) * 2 + k] = pTemp[(((i / 8) * (a_nWidth / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 2 + k];
					}
				}
			}
			delete[] pTemp;
		}
		break;
	case kTextureFormatLA88:
	case kTextureFormatHL8:
		{
			u8* pTemp = new u8[a_nWidth * a_nHeight * 2];
			for (n32 i = 0; i < a_nWidth * a_nHeight / 64; i++)
			{
				for (n32 j = 0; j < 64; j++)
				{
					for (n32 k = 0; k < 2; k++)
					{
						pTemp[(i * 64 + j) * 2 + k] = a_pBuffer[(i * 64 + s_nDecodeTransByte[j]) * 2 + 1 - k];
					}
				}
			}
			pRGBA = new u8[a_nWidth * a_nHeight * 2];
			for (n32 i = 0; i < a_nHeight; i++)
			{
				for (n32 j = 0; j < a_nWidth; j++)
				{
					for (n32 k = 0; k < 2; k++)
					{
						pRGBA[(i * a_nWidth + j) * 2 + k] = pTemp[(((i / 8) * (a_nWidth / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 2 + k];
					}
				}
			}
			delete[] pTemp;
		}
		break;
	case kTextureFormatL8:
	case kTextureFormatA8:
	case kTextureFormatLA44:
		{
			u8* pTemp = new u8[a_nWidth * a_nHeight];
			for (n32 i = 0; i < a_nWidth * a_nHeight / 64; i++)
			{
				for (n32 j = 0; j < 64; j++)
				{
					pTemp[i * 64 + j] = a_pBuffer[i * 64 + s_nDecodeTransByte[j]];
				}
			}
			pRGBA = new u8[a_nWidth * a_nHeight];
			for (n32 i = 0; i < a_nHeight; i++)
			{
				for (n32 j = 0; j < a_nWidth; j++)
				{
					pRGBA[i * a_nWidth + j] = pTemp[((i / 8) * (a_nWidth / 8) + j / 8) * 64 + i % 8 * 8 + j % 8];
				}
			}
			delete[] pTemp;
		}
		break;
	case kTextureFormatL4:
	case kTextureFormatA4:
		{
			u8* pTemp = new u8[a_nWidth * a_nHeight];
			for (n32 i = 0; i < a_nWidth * a_nHeight / 64; i++)
			{
				for (n32 j = 0; j < 64; j += 2)
				{
					pTemp[i * 64 + j] = (a_pBuffer[i * 32 + s_nDecodeTransByte[j] / 2] & 0xF) * 0x11;
					pTemp[i * 64 + j + 1] = (a_pBuffer[i * 32 + s_nDecodeTransByte[j] / 2] >> 4 & 0xF) * 0x11;
				}
			}
			pRGBA = new u8[a_nWidth * a_nHeight];
			for (n32 i = 0; i < a_nHeight; i++)
			{
				for (n32 j = 0; j < a_nWidth; j++)
				{
					pRGBA[i * a_nWidth + j] = pTemp[((i / 8) * (a_nWidth / 8) + j / 8) * 64 + i % 8 * 8 + j % 8];
				}
			}
			delete[] pTemp;
		}
		break;
	case kTextureFormatETC1:
		{
			u8* pTemp = new u8[a_nWidth * a_nHeight / 2];
			for (n32 i = 0; i < a_nWidth * a_nHeight / 2 / 8; i++)
			{
				for (n32 j = 0; j < 8; j++)
				{
					pTemp[i * 8 + j] = a_pBuffer[i * 8 + 7 - j];
				}
			}
			pRGBA = new u8[a_nWidth * a_nHeight / 2];
			for (n32 i = 0; i < a_nHeight; i += 4)
			{
				for (n32 j = 0; j < a_nWidth; j += 4)
				{
					memcpy(pRGBA + ((i / 4) * (a_nWidth / 4) + j / 4) * 8, pTemp + (((i / 8) * (a_nWidth / 8) + j / 8) * 4 + (i % 8 / 4 * 2 + j % 8 / 4)) * 8, 8);
				}
			}
			delete[] pTemp;
		}
		break;
	case kTextureFormatETC1_A4:
		{
			u8* pTemp = new u8[a_nWidth * a_nHeight / 2];
			for (n32 i = 0; i < a_nWidth * a_nHeight / 2 / 8; i++)
			{
				for (n32 j = 0; j < 8; j++)
				{
					pTemp[i * 8 + j] = a_pBuffer[8 + i * 16 + 7 - j];
				}
			}
			pRGBA = new u8[a_nWidth * a_nHeight / 2];
			for (n32 i = 0; i < a_nHeight; i += 4)
			{
				for (n32 j = 0; j < a_nWidth; j += 4)
				{
					memcpy(pRGBA + ((i / 4) * (a_nWidth / 4) + j / 4) * 8, pTemp + (((i / 8) * (a_nWidth / 8) + j / 8) * 4 + (i % 8 / 4 * 2 + j % 8 / 4)) * 8, 8);
				}
			}
			delete[] pTemp;
			pTemp = new u8[a_nWidth * a_nHeight];
			for (n32 i = 0; i < a_nWidth * a_nHeight / 16; i++)
			{
				for (n32 j = 0; j < 4; j++)
				{
					pTemp[i * 16 + j] = (a_pBuffer[i * 16 + j * 2] & 0x0F) * 0x11;
					pTemp[i * 16 + j + 4] = (a_pBuffer[i * 16 + j * 2] >> 4 & 0x0F) * 0x11;
					pTemp[i * 16 + j + 8] = (a_pBuffer[i * 16 + j * 2 + 1] & 0x0F) * 0x11;
					pTemp[i * 16 + j + 12] = (a_pBuffer[i * 16 + j * 2 + 1] >> 4 & 0x0F) * 0x11;
				}
			}
			pAlpha = new u8[a_nWidth * a_nHeight];
			for (n32 i = 0; i < a_nHeight; i++)
			{
				for (n32 j = 0; j < a_nWidth; j++)
				{
					pAlpha[i * a_nWidth + j] = pTemp[(((i / 8) * (a_nWidth / 8) + j / 8) * 4 + i % 8 / 4 * 2 + j % 8 / 4) * 16 + i % 4 * 4 + j % 4];
				}
			}
			delete[] pTemp;
		}
		break;
	}
	PVRTextureHeaderV3 pvrTextureHeaderV3;
	switch (a_nFormat)
	{
	case kTextureFormatRGBA8888:
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('r', 'g', 'b', 'a', 8, 8, 8, 8).PixelTypeID;
		break;
	case kTextureFormatRGB888:
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('r', 'g', 'b', 0, 8, 8, 8, 0).PixelTypeID;
		break;
	case kTextureFormatRGBA5551:
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('r', 'g', 'b', 'a', 5, 5, 5, 1).PixelTypeID;
		break;
	case kTextureFormatRGB565:
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('r', 'g', 'b', 0, 5, 6, 5, 0).PixelTypeID;
		break;
	case kTextureFormatRGBA4444:
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('r', 'g', 'b', 'a', 4, 4, 4, 4).PixelTypeID;
		break;
	case kTextureFormatLA88:
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('l', 'a', 0, 0, 8, 8, 0, 0).PixelTypeID;
		break;
	case kTextureFormatHL8:
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('r', 'g', 0, 0, 8, 8, 0, 0).PixelTypeID;
		break;
	case kTextureFormatL8:
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('l', 0, 0, 0, 8, 0, 0, 0).PixelTypeID;
		break;
	case kTextureFormatA8:
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('a', 0, 0, 0, 8, 0, 0, 0).PixelTypeID;
		break;
	case kTextureFormatLA44:
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('l', 'a', 0, 0, 4, 4, 0, 0).PixelTypeID;
		break;
	case kTextureFormatL4:
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('l', 0, 0, 0, 8, 0, 0, 0).PixelTypeID;
		break;
	case kTextureFormatA4:
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('a', 0, 0, 0, 8, 0, 0, 0).PixelTypeID;
		break;
	case kTextureFormatETC1:
		pvrTextureHeaderV3.u64PixelFormat = ePVRTPF_ETC1;
		break;
	case kTextureFormatETC1_A4:
		pvrTextureHeaderV3.u64PixelFormat = ePVRTPF_ETC1;
		break;
	}
	pvrTextureHeaderV3.u32Height = a_nHeight;
	pvrTextureHeaderV3.u32Width = a_nWidth;
	MetaDataBlock metaDataBlock;
	metaDataBlock.DevFOURCC = PVRTEX3_IDENT;
	metaDataBlock.u32Key = ePVRTMetaDataTextureOrientation;
	metaDataBlock.u32DataSize = 3;
	metaDataBlock.Data = new PVRTuint8[metaDataBlock.u32DataSize];
	metaDataBlock.Data[0] = ePVRTOrientRight;
	metaDataBlock.Data[1] = ePVRTOrientUp;
	metaDataBlock.Data[2] = ePVRTOrientIn;
	pvrtexture::CPVRTextureHeader pvrTextureHeader(pvrTextureHeaderV3, 1, &metaDataBlock);
	*a_pPVRTexture = new pvrtexture::CPVRTexture(pvrTextureHeader, pRGBA);
	delete[] pRGBA;
	pvrtexture::Transcode(**a_pPVRTexture, pvrtexture::PVRStandard8PixelType, ePVRTVarTypeUnsignedByteNorm, ePVRTCSpacelRGB);
	if (a_nFormat == kTextureFormatETC1_A4)
	{
		u8* pData = static_cast<u8*>((*a_pPVRTexture)->getDataPtr());
		for (n32 i = 0; i < a_nHeight; i++)
		{
			for (n32 j = 0; j < a_nWidth; j++)
			{
				pData[(i * a_nWidth + j) * 4 + 3] = pAlpha[i * a_nWidth + j];
			}
		}
		delete[] pAlpha;
	}
	return 0;
}

void encode(u8* a_pData, n32 a_nWidth, n32 a_nHeight, n32 a_nFormat, n32 a_nMipmapLevel, n32 a_nBPP, u8** a_pBuffer)
{
	PVRTextureHeaderV3 pvrTextureHeaderV3;
	pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PVRStandard8PixelType.PixelTypeID;
	pvrTextureHeaderV3.u32Height = a_nHeight;
	pvrTextureHeaderV3.u32Width = a_nWidth;
	MetaDataBlock metaDataBlock;
	metaDataBlock.DevFOURCC = PVRTEX3_IDENT;
	metaDataBlock.u32Key = ePVRTMetaDataTextureOrientation;
	metaDataBlock.u32DataSize = 3;
	metaDataBlock.Data = new PVRTuint8[metaDataBlock.u32DataSize];
	metaDataBlock.Data[0] = ePVRTOrientRight;
	metaDataBlock.Data[1] = ePVRTOrientUp;
	metaDataBlock.Data[2] = ePVRTOrientIn;
	pvrtexture::CPVRTextureHeader pvrTextureHeader(pvrTextureHeaderV3, 1, &metaDataBlock);
	pvrtexture::CPVRTexture* pPVRTexture = nullptr;
	pvrtexture::CPVRTexture* pPVRTextureAlpha = nullptr;
	if (a_nFormat != kTextureFormatETC1_A4)
	{
		pPVRTexture = new pvrtexture::CPVRTexture(pvrTextureHeader, a_pData);
	}
	else
	{
		u8* pRGBAData = new u8[a_nWidth * a_nHeight * 4];
		memcpy(pRGBAData, a_pData, a_nWidth * a_nHeight * 4);
		u8* pAlphaData = new u8[a_nWidth * a_nHeight * 4];
		memcpy(pAlphaData, a_pData, a_nWidth * a_nHeight * 4);
		for (n32 i = 0; i < a_nWidth * a_nHeight; i++)
		{
			pRGBAData[i * 4 + 3] = 0xFF;
			pAlphaData[i * 4] = 0;
			pAlphaData[i * 4 + 1] = 0;
			pAlphaData[i * 4 + 2] = 0;
		}
		pPVRTexture = new pvrtexture::CPVRTexture(pvrTextureHeader, pRGBAData);
		pPVRTextureAlpha = new pvrtexture::CPVRTexture(pvrTextureHeader, pAlphaData);
		delete[] pRGBAData;
		delete[] pAlphaData;
	}
	if (a_nMipmapLevel != 1)
	{
		pvrtexture::GenerateMIPMaps(*pPVRTexture, pvrtexture::eResizeNearest, a_nMipmapLevel);
		if (a_nFormat == kTextureFormatETC1_A4)
		{
			pvrtexture::GenerateMIPMaps(*pPVRTextureAlpha, pvrtexture::eResizeNearest, a_nMipmapLevel);
		}
	}
	pvrtexture::uint64 uPixelFormat = 0;
	pvrtexture::ECompressorQuality eCompressorQuality = pvrtexture::ePVRTCBest;
	switch (a_nFormat)
	{
	case kTextureFormatRGBA8888:
		uPixelFormat = pvrtexture::PixelType('r', 'g', 'b', 'a', 8, 8, 8, 8).PixelTypeID;
		break;
	case kTextureFormatRGB888:
		uPixelFormat = pvrtexture::PixelType('r', 'g', 'b', 0, 8, 8, 8, 0).PixelTypeID;
		break;
	case kTextureFormatRGBA5551:
		uPixelFormat = pvrtexture::PixelType('r', 'g', 'b', 'a', 5, 5, 5, 1).PixelTypeID;
		break;
	case kTextureFormatRGB565:
		uPixelFormat = pvrtexture::PixelType('r', 'g', 'b', 0, 5, 6, 5, 0).PixelTypeID;
		break;
	case kTextureFormatRGBA4444:
		uPixelFormat = pvrtexture::PixelType('r', 'g', 'b', 'a', 4, 4, 4, 4).PixelTypeID;
		break;
	case kTextureFormatLA88:
		uPixelFormat = pvrtexture::PixelType('l', 'a', 0, 0, 8, 8, 0, 0).PixelTypeID;
		break;
	case kTextureFormatHL8:
		uPixelFormat = pvrtexture::PixelType('r', 'g', 0, 0, 8, 8, 0, 0).PixelTypeID;
		break;
	case kTextureFormatL8:
		uPixelFormat = pvrtexture::PixelType('l', 0, 0, 0, 8, 0, 0, 0).PixelTypeID;
		break;
	case kTextureFormatA8:
		uPixelFormat = pvrtexture::PixelType('a', 0, 0, 0, 8, 0, 0, 0).PixelTypeID;
		break;
	case kTextureFormatLA44:
		uPixelFormat = pvrtexture::PixelType('l', 'a', 0, 0, 4, 4, 0, 0).PixelTypeID;
		break;
	case kTextureFormatL4:
		uPixelFormat = pvrtexture::PixelType('l', 0, 0, 0, 8, 0, 0, 0).PixelTypeID;
		break;
	case kTextureFormatA4:
		uPixelFormat = pvrtexture::PixelType('a', 0, 0, 0, 8, 0, 0, 0).PixelTypeID;
		break;
	case kTextureFormatETC1:
		uPixelFormat = ePVRTPF_ETC1;
		eCompressorQuality = pvrtexture::eETCSlowPerceptual;
		break;
	case kTextureFormatETC1_A4:
		uPixelFormat = ePVRTPF_ETC1;
		eCompressorQuality = pvrtexture::eETCSlowPerceptual;
		break;
	}
	pvrtexture::Transcode(*pPVRTexture, uPixelFormat, ePVRTVarTypeUnsignedByteNorm, ePVRTCSpacelRGB, eCompressorQuality);
	if (a_nFormat == kTextureFormatETC1_A4)
	{
		pvrtexture::Transcode(*pPVRTextureAlpha, pvrtexture::PixelType('a', 0, 0, 0, 8, 0, 0, 0).PixelTypeID, ePVRTVarTypeUnsignedByteNorm, ePVRTCSpacelRGB, pvrtexture::ePVRTCBest);
	}
	n32 nTotalSize = 0;
	n32 nCurrentSize = 0;
	for (n32 l = 0; l < a_nMipmapLevel; l++)
	{
		nTotalSize += (a_nWidth >> l) * (a_nHeight >> l) * a_nBPP / 8;
	}
	*a_pBuffer = new u8[nTotalSize];
	for (n32 l = 0; l < a_nMipmapLevel; l++)
	{
		n32 nMipmapWidth = a_nWidth >> l;
		n32 nMipmapHeight = a_nHeight >> l;
		u8* pRGBA = static_cast<u8*>(pPVRTexture->getDataPtr(l));
		u8* pAlpha = nullptr;
		if (a_nFormat == kTextureFormatETC1_A4)
		{
			pAlpha = static_cast<u8*>(pPVRTextureAlpha->getDataPtr(l));
		}
		switch (a_nFormat)
		{
		case kTextureFormatRGBA8888:
			{
				u8* pTemp = new u8[nMipmapWidth * nMipmapHeight * 4];
				for (n32 i = 0; i < nMipmapHeight; i++)
				{
					for (n32 j = 0; j < nMipmapWidth; j++)
					{
						for (n32 k = 0; k < 4; k++)
						{
							pTemp[(((i / 8) * (nMipmapWidth / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 4 + k] = pRGBA[(i * nMipmapWidth + j) * 4 + k];
						}
					}
				}
				u8* pMipmapBuffer = *a_pBuffer + nCurrentSize;
				for (n32 i = 0; i < nMipmapWidth * nMipmapHeight / 64; i++)
				{
					for (n32 j = 0; j < 64; j++)
					{
						for (n32 k = 0; k < 4; k++)
						{
							pMipmapBuffer[(i * 64 + s_nDecodeTransByte[j]) * 4 + 3 - k] = pTemp[(i * 64 + j) * 4 + k];
						}
					}
				}
				delete[] pTemp;
			}
			break;
		case kTextureFormatRGB888:
			{
				u8* pTemp = new u8[nMipmapWidth * nMipmapHeight * 3];
				for (n32 i = 0; i < nMipmapHeight; i++)
				{
					for (n32 j = 0; j < nMipmapWidth; j++)
					{
						for (n32 k = 0; k < 3; k++)
						{
							pTemp[(((i / 8) * (nMipmapWidth / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 3 + k] = pRGBA[(i * nMipmapWidth + j) * 3 + k];
						}
					}
				}
				u8* pMipmapBuffer = *a_pBuffer + nCurrentSize;
				for (n32 i = 0; i < nMipmapWidth * nMipmapHeight / 64; i++)
				{
					for (n32 j = 0; j < 64; j++)
					{
						for (n32 k = 0; k < 3; k++)
						{
							pMipmapBuffer[(i * 64 + s_nDecodeTransByte[j]) * 3 + 2 - k] = pTemp[(i * 64 + j) * 3 + k];
						}
					}
				}
				delete[] pTemp;
			}
			break;
		case kTextureFormatRGBA5551:
		case kTextureFormatRGB565:
		case kTextureFormatRGBA4444:
			{
				u8* pTemp = new u8[nMipmapWidth * nMipmapHeight * 2];
				for (n32 i = 0; i < nMipmapHeight; i++)
				{
					for (n32 j = 0; j < nMipmapWidth; j++)
					{
						for (n32 k = 0; k < 2; k++)
						{
							pTemp[(((i / 8) * (nMipmapWidth / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 2 + k] = pRGBA[(i * nMipmapWidth + j) * 2 + k];
						}
					}
				}
				u8* pMipmapBuffer = *a_pBuffer + nCurrentSize;
				for (n32 i = 0; i < nMipmapWidth * nMipmapHeight / 64; i++)
				{
					for (n32 j = 0; j < 64; j++)
					{
						for (n32 k = 0; k < 2; k++)
						{
							pMipmapBuffer[(i * 64 + s_nDecodeTransByte[j]) * 2 + k] = pTemp[(i * 64 + j) * 2 + k];
						}
					}
				}
				delete[] pTemp;
			}
			break;
		case kTextureFormatLA88:
		case kTextureFormatHL8:
			{
				u8* pTemp = new u8[nMipmapWidth * nMipmapHeight * 2];
				for (n32 i = 0; i < nMipmapHeight; i++)
				{
					for (n32 j = 0; j < nMipmapWidth; j++)
					{
						for (n32 k = 0; k < 2; k++)
						{
							pTemp[(((i / 8) * (nMipmapWidth / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 2 + k] = pRGBA[(i * nMipmapWidth + j) * 2 + k];
						}
					}
				}
				u8* pMipmapBuffer = *a_pBuffer + nCurrentSize;
				for (n32 i = 0; i < nMipmapWidth * nMipmapHeight / 64; i++)
				{
					for (n32 j = 0; j < 64; j++)
					{
						for (n32 k = 0; k < 2; k++)
						{
							pMipmapBuffer[(i * 64 + s_nDecodeTransByte[j]) * 2 + 1 - k] = pTemp[(i * 64 + j) * 2 + k];
						}
					}
				}
				delete[] pTemp;
			}
			break;
		case kTextureFormatL8:
		case kTextureFormatA8:
		case kTextureFormatLA44:
			{
				u8* pTemp = new u8[nMipmapWidth * nMipmapHeight];
				for (n32 i = 0; i < nMipmapHeight; i++)
				{
					for (n32 j = 0; j < nMipmapWidth; j++)
					{
						pTemp[((i / 8) * (nMipmapWidth / 8) + j / 8) * 64 + i % 8 * 8 + j % 8] = pRGBA[i * nMipmapWidth + j];
					}
				}
				u8* pMipmapBuffer = *a_pBuffer + nCurrentSize;
				for (n32 i = 0; i < nMipmapWidth * nMipmapHeight / 64; i++)
				{
					for (n32 j = 0; j < 64; j++)
					{
						pMipmapBuffer[i * 64 + s_nDecodeTransByte[j]] = pTemp[i * 64 + j];
					}
				}
				delete[] pTemp;
			}
			break;
		case kTextureFormatL4:
		case kTextureFormatA4:
			{
				u8* pTemp = new u8[nMipmapWidth * nMipmapHeight];
				for (n32 i = 0; i < nMipmapHeight; i++)
				{
					for (n32 j = 0; j < nMipmapWidth; j++)
					{
						pTemp[((i / 8) * (nMipmapWidth / 8) + j / 8) * 64 + i % 8 * 8 + j % 8] = pRGBA[i * nMipmapWidth + j];
					}
				}
				u8* pMipmapBuffer = *a_pBuffer + nCurrentSize;
				for (n32 i = 0; i < nMipmapWidth * nMipmapHeight / 64; i++)
				{
					for (n32 j = 0; j < 64; j += 2)
					{
						pMipmapBuffer[i * 32 + s_nDecodeTransByte[j] / 2] = ((pTemp[i * 64 + j] / 0x11) & 0x0F) | ((pTemp[i * 64 + j + 1] / 0x11) << 4 & 0xF0);
					}
				}
				delete[] pTemp;
			}
			break;
		case kTextureFormatETC1:
			{
				u8* pTemp = new u8[nMipmapWidth * nMipmapHeight / 2];
				for (n32 i = 0; i < nMipmapHeight; i += 4)
				{
					for (n32 j = 0; j < nMipmapWidth; j += 4)
					{
						memcpy(pTemp + (((i / 8) * (nMipmapWidth / 8) + j / 8) * 4 + (i % 8 / 4 * 2 + j % 8 / 4)) * 8, pRGBA + ((i / 4) * (nMipmapWidth / 4) + j / 4) * 8, 8);
					}
				}
				u8* pMipmapBuffer = *a_pBuffer + nCurrentSize;
				for (n32 i = 0; i < nMipmapWidth * nMipmapHeight / 2 / 8; i++)
				{
					for (n32 j = 0; j < 8; j++)
					{
						pMipmapBuffer[i * 8 + 7 - j] = pTemp[i * 8 + j];
					}
				}
				delete[] pTemp;
			}
			break;
		case kTextureFormatETC1_A4:
			{
				u8* pTemp = new u8[nMipmapWidth * nMipmapHeight / 2];
				for (n32 i = 0; i < nMipmapHeight; i += 4)
				{
					for (n32 j = 0; j < nMipmapWidth; j += 4)
					{
						memcpy(pTemp + (((i / 8) * (nMipmapWidth / 8) + j / 8) * 4 + (i % 8 / 4 * 2 + j % 8 / 4)) * 8, pRGBA + ((i / 4) * (nMipmapWidth / 4) + j / 4) * 8, 8);
					}
				}
				u8* pMipmapBuffer = *a_pBuffer + nCurrentSize;
				for (n32 i = 0; i < nMipmapWidth * nMipmapHeight / 2 / 8; i++)
				{
					for (n32 j = 0; j < 8; j++)
					{
						pMipmapBuffer[8 + i * 16 + 7 - j] = pTemp[i * 8 + j];
					}
				}
				delete[] pTemp;
				pTemp = new u8[nMipmapWidth * nMipmapHeight];
				for (n32 i = 0; i < nMipmapHeight; i++)
				{
					for (n32 j = 0; j < nMipmapWidth; j++)
					{
						pTemp[(((i / 8) * (nMipmapWidth / 8) + j / 8) * 4 + i % 8 / 4 * 2 + j % 8 / 4) * 16 + i % 4 * 4 + j % 4] = pAlpha[i * nMipmapWidth + j];
					}
				}
				for (n32 i = 0; i < nMipmapWidth * nMipmapHeight / 16; i++)
				{
					for (n32 j = 0; j < 4; j++)
					{
						pMipmapBuffer[i * 16 + j * 2] = ((pTemp[i * 16 + j] / 0x11) & 0x0F) | ((pTemp[i * 16 + j + 4] / 0x11) << 4 & 0xF0);
						pMipmapBuffer[i * 16 + j * 2 + 1] = ((pTemp[i * 16 + j + 8] / 0x11) & 0x0F) | ((pTemp[i * 16 + j + 12] / 0x11) << 4 & 0xF0);
					}
				}
				delete[] pTemp;
			}
			break;
		}
		nCurrentSize += nMipmapWidth * nMipmapHeight * a_nBPP / 8;
	}
	delete pPVRTexture;
	if (a_nFormat == kTextureFormatETC1_A4)
	{
		delete pPVRTextureAlpha;
	}
}

int decodeT4(const UChar* a_pT4FileName, const UChar* a_pPngFileName)
{
	FILE* fp = UFopen(a_pT4FileName, USTR("rb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uT4Size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	u8* pT4 = new u8[uT4Size];
	fread(pT4, 1, uT4Size, fp);
	fclose(fp);
	ST4Header0* pT4Header0 = reinterpret_cast<ST4Header0*>(pT4);
	if (pT4Header0->Unknown0x0 != 0x15041213)
	{
		delete[] pT4;
		return 1;
	}
	if (pT4Header0->Unknown0x4 != 1)
	{
		delete[] pT4;
		return 1;
	}
	if (memcmp(pT4Header0->Unknown0x8, "texture", 8) != 0)
	{
		delete[] pT4;
		return 1;
	}
	if (pT4Header0->DataSize != uT4Size - sizeof(ST4Header0))
	{
		delete[] pT4;
		return 1;
	}
	if (pT4Header0->Unknown0x14 != 0xFFFFFFFF)
	{
		delete[] pT4;
		return 1;
	}
	ST4Header1* pT4Header1 = reinterpret_cast<ST4Header1*>(pT4Header0 + 1);
	if (pT4Header1->ImageSize + sizeof(ST4Header1) != pT4Header0->DataSize)
	{
		delete[] pT4;
		return 1;
	}
	if (pT4Header1->Unknown0x4 != 0)
	{
		delete[] pT4;
		return 1;
	}
	if (pT4Header1->Unknown0x8 != 0)
	{
		delete[] pT4;
		return 1;
	}
	if (pT4Header1->Unknown0xC != 0)
	{
		delete[] pT4;
		return 1;
	}
	if (pT4Header1->Format != kTextureFormatRGBA8888 && pT4Header1->Format != kTextureFormatRGBA4444 && pT4Header1->Format != kTextureFormatRGBA5551 && pT4Header1->Format != kTextureFormatA8 && pT4Header1->Format != kTextureFormatETC1_A4)
	{
		delete[] pT4;
		return 1;
	}
	if (pT4Header1->MipmapLevel != 1)
	{
		delete[] pT4;
		return 1;
	}
	for (n32 i = 0; i < SDW_ARRAY_COUNT(pT4Header1->Unknown0x58); i++)
	{
		if (pT4Header1->Unknown0x58[i] != 0xFFFFFFFF)
		{
			delete[] pT4;
			return 1;
		}
	}
	u32 uOffset = sizeof(ST4Header0) + sizeof(ST4Header1);
	pvrtexture::CPVRTexture* pPVRTexture = nullptr;
	if (decode(pT4 + uOffset, pT4Header1->Width, pT4Header1->Height, pT4Header1->Format, &pPVRTexture) == 0)
	{
		FILE* fpSub = UFopen(a_pPngFileName, USTR("wb"), false);
		if (fpSub == nullptr)
		{
			delete pPVRTexture;
			delete[] pT4;
			return 1;
		}
		png_structp pPng = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		if (pPng == nullptr)
		{
			fclose(fpSub);
			delete pPVRTexture;
			delete[] pT4;
			return 1;
		}
		png_infop pInfo = png_create_info_struct(pPng);
		if (pInfo == nullptr)
		{
			png_destroy_write_struct(&pPng, nullptr);
			fclose(fpSub);
			delete pPVRTexture;
			delete[] pT4;
			return 1;
		}
		if (setjmp(png_jmpbuf(pPng)) != 0)
		{
			png_destroy_write_struct(&pPng, &pInfo);
			fclose(fpSub);
			delete pPVRTexture;
			delete[] pT4;
			return 1;
		}
		png_init_io(pPng, fpSub);
		png_set_IHDR(pPng, pInfo, pT4Header1->Width, pT4Header1->Height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		u8* pData = static_cast<u8*>(pPVRTexture->getDataPtr());
		png_bytepp pRowPointers = new png_bytep[pT4Header1->Height];
		for (u32 j = 0; j < pT4Header1->Height; j++)
		{
			pRowPointers[j] = pData + j * pT4Header1->Width * 4;
		}
		png_set_rows(pPng, pInfo, pRowPointers);
		png_write_png(pPng, pInfo, PNG_TRANSFORM_IDENTITY, nullptr);
		png_destroy_write_struct(&pPng, &pInfo);
		delete[] pRowPointers;
		fclose(fpSub);
		delete pPVRTexture;
	}
	else
	{
		delete pPVRTexture;
		delete[] pT4;
		return 1;
	}
	delete[] pT4;
	return 0;
}

int encodeT4(const UChar* a_pT4FileName, const UChar* a_pPngFileName)
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
		case USTR('D'):
		case USTR('d'):
			return decodeT4(argv[2], argv[3]);
		case USTR('E'):
		case USTR('e'):
			return encodeT4(argv[2], argv[3]);
		default:
			break;
		}
	}
	return 1;
}
