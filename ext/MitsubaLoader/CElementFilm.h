#ifndef __C_ELEMENT_FILM_H_INCLUDED__
#define __C_ELEMENT_FILM_H_INCLUDED__

#include "irr/macros.h"

#include "../../ext/MitsubaLoader/CElementRFilter.h"

namespace irr
{
namespace ext
{
namespace MitsubaLoader
{

class CGlobalMitsubaMetadata;

class CElementFilm : public IElement
{
	public:
		enum Type
		{
			INVALID,
			HDR_FILM,
			TILED_HDR,
			LDR_FILM,
			MFILM
		};
		enum PixelFormat
		{
			LUMINANCE,
			LUMINANCE_ALPHA,
			RGB,
			RGBA,
			XYZ,
			XYZA,
			SPECTRUM,
			SPECTRUM_ALPHA
		};
		enum FileFormat
		{
			OPENEXR,
			RGBE,
			PFM,
			PNG,
			JPEG,
			MATLAB,
			MATHEMATICA,
			NUMPY
		};
		enum ComponentFormat
		{
			FLOAT16,
			FLOAT32,
			UINT32
		};
		struct HDR
		{
			bool attachLog = true;
		};
		struct LDR
		{
			enum TonemapMethod
			{
				GAMMA,
				REINHARD
			};
			TonemapMethod tonemapMethod = GAMMA;
			float gamma = -1.f; // should really be an OETF choice
			float exposure = 0.f;
			float key = 0.18;
			float burn = 0.f;
		};
		struct M
		{
			M() : digits(4)
			{
				variable[0] = 'd';
				variable[1] = 'a';
				variable[2] = 't';
				variable[3] = 'a';
				variable[4] = 0;
			}
			int32_t digits;
			_IRR_STATIC_INLINE_CONSTEXPR size_t MaxVarNameLen = 63; // matlab
			char variable[MaxVarNameLen+1];
		};

		CElementFilm(const char* id) : IElement(id), type(Type::HDR_FILM),
			width(768), height(576), cropOffsetX(0), cropOffsetY(0), cropWidth(INT_MAX), cropHeight(INT_MAX),
			fileFormat(OPENEXR), pixelFormat(RGB), componentFormat(FLOAT16),
			banner(true), highQualityEdges(false), rfilter("")
		{
			hdrfilm = HDR();
		}
		virtual ~CElementFilm()
		{
		}

		bool addProperty(SNamedPropertyElement&& _property) override;
		bool onEndTag(asset::IAssetLoader::IAssetLoaderOverride* _override, CGlobalMitsubaMetadata* globalMetadata) override;
		IElement::Type getType() const override { return IElement::Type::FILM; }
		std::string getLogName() const override { return "film"; }

		inline bool processChildData(IElement* _child, const std::string& name) override
		{
			if (!_child)
				return true;
			if (_child->getType() != IElement::Type::RFILTER)
				return false;
			auto _rfilter = static_cast<CElementRFilter*>(_child);
			if (_rfilter->type == CElementRFilter::Type::INVALID)
				return false;
			rfilter = *_rfilter;
			return true;
		}

		// make these public
		Type			type;
		int32_t			width,height;
		int32_t			cropOffsetX,cropOffsetY,cropWidth,cropHeight;
		FileFormat		fileFormat;
		PixelFormat		pixelFormat;
		ComponentFormat	componentFormat;
		bool banner;
		bool highQualityEdges;
		CElementRFilter rfilter;
		union
		{
			HDR hdrfilm;
			LDR ldrfilm;
			M	mfilm;
		};
};


}
}
}

#endif