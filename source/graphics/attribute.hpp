#pragma once

#include <cstring>
#include <initializer_list>

#include <3ds.h>
#include <citro3d.h>

#include "core.hpp"

namespace GFX {

enum AttributeFormat : u32 {
	Byte = GPU_BYTE,
	UnsignedByte = GPU_UNSIGNED_BYTE,
	Short = GPU_SHORT,
	Float = GPU_FLOAT,
	Fixed = 255
};

struct Attribute {
	int reg;
	AttributeFormat format;
	int count;

	constexpr Attribute(int reg, AttributeFormat format, int count) : reg(reg) , format(format), count(count) {};
};

struct FixedAttribute : Attribute {
	constexpr FixedAttribute(int reg) : Attribute(reg, Fixed, 0) {};
};

// This *could* be a constexpr
// Edit: constexpr isn't as magical as I though it was
// Edit2: constexpr works, but memcpy doesnt :/
struct AttributeInfo : public C3D_AttrInfo {
	constexpr AttributeInfo(std::initializer_list<Attribute> list) : C3D_AttrInfo({0}) {
		// AttrInfo_Init(this);
		flags[1] = 0xFFF << 16;
		attrCount = list.size();

		unsigned int id = 0;
		for (const Attribute &attr : list) {
			int regId = attr.reg;
			int count = attr.count;

			if (regId < 0) regId = id;

			if (attr.format == Fixed) {
				// AttrInfo_AddFixed(this, attr.reg);

				flags[1] = (flags[1] &~ 0xF0000000) | (id << 28);
				permutation |= regId << (id*4);
			} else {
				GPU_FORMATS format = (GPU_FORMATS) attr.format;
				// AttrInfo_AddLoader(this, attr.reg, (GPU_FORMATS) attr.format, attr.count);

				if (id < 8)
					flags[0] |= GPU_ATTRIBFMT(id, count, format);
				else
					flags[1] |= GPU_ATTRIBFMT(id-8, count, format);

				flags[1] = (flags[1] &~ (0xF0000000 | (1 << (id+16)))) | (id << 28);
				permutation |= regId << (id*4);
			}

			id++;
		}

		/*AttrInfo_Init(this);
		for (Attribute attr : list) {
			if (attr.format == Fixed) {
				AttrInfo_AddFixed(this, attr.reg);
			} else {
				AttrInfo_AddLoader(this, attr.reg, (GPU_FORMATS) attr.format, attr.count);
			}
		}*/
	}

	void use() const {
		// Another instance where the argument is const in the function itself but not in the decl
		// TODO: Submit a PR for Citro3D with proper constness EVERYWHERE!
		//C3D_SetAttrInfo(const_cast<C3D_AttrInfo*>((const C3D_AttrInfo*) this));

		// Then again, memcpy is broken! Below does NOT work:
		//std::memcpy(C3D_GetAttrInfo(), (const C3D_AttrInfo*)this, sizeof(C3D_AttrInfo));

		// However, this works (????????????????????????????)
		auto attrInfo = C3D_GetAttrInfo();
		attrInfo->attrCount = attrCount;
		attrInfo->flags[0] = flags[0];
		attrInfo->flags[1] = flags[1];
		attrInfo->permutation = permutation;
	}
};

namespace Attr {
	static constexpr AttributeInfo PositionColor {
		GFX::Attribute(0, GFX::AttributeFormat::Float, 3),
		GFX::Attribute(1, GFX::AttributeFormat::UnsignedByte, 4)
	};

	static constexpr AttributeInfo PositionFixedColor {
		GFX::Attribute(0, GFX::AttributeFormat::Float, 3),
		GFX::FixedAttribute(1)
	};

	static_assert(PositionFixedColor.attrCount == 2, "invalid attribute count");
	static_assert(PositionFixedColor.permutation == 0x10, "invalid permutation");
	static_assert(PositionFixedColor.flags[0] == 0xB, "invalid flags[0]");
	static_assert(PositionFixedColor.flags[1] == 0x1FFE0000, "invalid flags[1]");

	static constexpr AttributeInfo PositionColorTexture {
		GFX::Attribute(0, GFX::AttributeFormat::Float, 3),
		GFX::Attribute(1, GFX::AttributeFormat::UnsignedByte, 4),
		GFX::Attribute(2, GFX::AttributeFormat::Float, 2)
	};

	static constexpr AttributeInfo PositionFixedColorTexture {
		GFX::Attribute(0, GFX::AttributeFormat::Float, 3),
		GFX::FixedAttribute(1),
		GFX::Attribute(2, GFX::AttributeFormat::Float, 2)
	};
}

}
