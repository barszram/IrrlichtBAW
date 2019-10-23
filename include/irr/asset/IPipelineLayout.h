#ifndef __IRR_I_PIPELINE_LAYOUT_H_INCLUDED__
#define __IRR_I_PIPELINE_LAYOUT_H_INCLUDED__

#include "irr/core/IReferenceCounted.h"
#include "irr/macros.h"
#include "irr/asset/ShaderCommons.h"
#include "irr/core/memory/refctd_dynamic_array.h"
#include "irr/core/SRange.h"
#include <algorithm>
#include <array>

namespace irr {
namespace asset
{

struct SPushConstantRange
{
    E_SHADER_STAGE stageFlags;
    uint32_t offset;
    uint32_t size;

    inline bool operator==(const SPushConstantRange& _rhs) const
    {
        if (stageFlags != _rhs.stageFlags)
            return false;
        if (offset != _rhs.offset)
            return false;
        return (size == _rhs.size);
    }
    inline bool operator!=(const SPushConstantRange& _rhs) const
    {
        return !((*this)==_rhs);
    }
};

template<typename DescLayoutType>
class IPipelineLayout
{
public:
    _IRR_STATIC_INLINE_CONSTEXPR uint32_t DESCRIPTOR_SET_COUNT = 4u;

    const DescLayoutType* getDescriptorSetLayout(uint32_t _set) const { return m_descSetLayouts[_set].get(); }
    core::SRange<const SPushConstantRange> getPushConstantRanges() const 
    {
        if (m_pushConstantRanges)
            return {m_pushConstantRanges->data(), m_pushConstantRanges->data()+m_pushConstantRanges->size()};
        else 
            return {nullptr, nullptr};
    }

    bool isCompatibleForPushConstants(const IPipelineLayout<DescLayoutType>* _other) const
    {
        if (getPushConstantRanges().length() != _other->getPushConstantRanges().length())
            return false;

        const size_t cnt = getPushConstantRanges().length();
        const SPushConstantRange* lhs = getPushConstantRanges().begin();
        const SPushConstantRange* rhs = _other->getPushConstantRanges().begin();
        for (size_t i = 0ull; i < cnt; ++i)
            if (lhs[i] != rhs[i])
                return false;

        return true;
    }

    //! Checks if `this` and `_other` are compatible for set `_setNum`. See https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#descriptorsets-compatibility for compatiblity rules.
    /**
    @returns Max value of `_setNum` for which the two pipeline layouts are compatible or DESCRIPTOR_SET_COUNT if they're not compatible at all.
    */
    uint32_t isCompatibleForSet(const uint32_t _setNum, const IPipelineLayout<DescLayoutType>* _other) const
    {
        if ((_setNum >= DESCRIPTOR_SET_COUNT)) //vulkan would also care about push constant ranges compatibility here
            return DESCRIPTOR_SET_COUNT;

        for (uint32_t i = 0u; i <= _setNum; ++i)
        {
            const DescLayoutType* lhs = m_descSetLayouts[i].get();
            const DescLayoutType* rhs = _other->getDescriptorSetLayout();

            const bool compatible = (lhs == rhs) || (lhs->isIdenticallyDefined(rhs));
            if (!compatible)
                return (i == 0u) ? DESCRIPTOR_SET_COUNT : (i - 1u);
        }
        return _setNum;
    }

protected:
    virtual ~IPipelineLayout() = default;

    IPipelineLayout(
        const SPushConstantRange* const _pcRangesBegin = nullptr, const SPushConstantRange* const _pcRangesEnd = nullptr,
        core::smart_refctd_ptr<DescLayoutType>&& _layout0 = nullptr, core::smart_refctd_ptr<DescLayoutType>&& _layout1 = nullptr,
        core::smart_refctd_ptr<DescLayoutType>&& _layout2 = nullptr, core::smart_refctd_ptr<DescLayoutType>&& _layout3 = nullptr
    ) : m_descSetLayouts{{std::move(_layout0), std::move(_layout1), std::move(_layout2), std::move(_layout3)}},
        m_pushConstantRanges(_pcRangesBegin==_pcRangesEnd ? nullptr : core::make_refctd_dynamic_array<core::smart_refctd_dynamic_array<SPushConstantRange>>(_pcRangesEnd-_pcRangesBegin))
    {
        if (m_pushConstantRanges)
            std::copy(_pcRangesBegin, _pcRangesEnd, m_pushConstantRanges->begin());
    }


    std::array<core::smart_refctd_ptr<DescLayoutType>, DESCRIPTOR_SET_COUNT> m_descSetLayouts;
    core::smart_refctd_dynamic_array<SPushConstantRange> m_pushConstantRanges;
};

}
}

#endif