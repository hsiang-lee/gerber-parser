//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
// This file is part of a library
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//==============================================================================

// This is done manually because std:wstring_convert throws funny errors


#include "UTF_Converter.h"


UTF_CONVERTER UTF_Converter;


using namespace std;


UTF_CONVERTER::UTF_CONVERTER() {
}


UTF_CONVERTER::~UTF_CONVERTER() {
}


char32_t UTF_CONVERTER::GetUTF_32(const char* UTF_8, int* CodeLength) {
	uint32_t n;
	uint32_t Bits;   // Valid bits
	uint32_t UTF_32; // The result
	uint8_t  Lead;   // The leading byte
	const uint8_t* s = (const uint8_t*)UTF_8;

	if ((s[0] & 0xC0) == 0xC0) {
		n = 1;
		Lead = s[0] << 1;
		Bits = 0x3F; // 6 bits
		UTF_32 = s[0];

		while (Lead & 0x80) {
			if ((s[n] & 0xC0) != 0x80) { // Invalid code-word
				if (CodeLength) *CodeLength = 1;
				return s[0];
			}
			Bits = (Bits << 5) | 0x1F;
			UTF_32 = (UTF_32 << 6) | (s[n] & 0x3F);
			Lead <<= 1;
			n++;
		}
		if (CodeLength) *CodeLength = n;
		return UTF_32 & Bits;

	}
	else {
		if (CodeLength) *CodeLength = 1;
		return s[0];
	}
}


u32string& UTF_CONVERTER::UTF32(const char* UTF_8) {
	UTF_32.clear();

	int l;
	int n = 0;
	while (UTF_8[n]) {
		UTF_32.append(1, GetUTF_32(UTF_8 + n, &l));
		n += l;
	}

	return UTF_32;
}


u32string& UTF_CONVERTER::UTF32(const char16_t* UTF_16) {
	UTF_32.clear();

	const uint16_t* u = (const uint16_t*)UTF_16;

	char32_t c;

	for (int n = 0; u[n]; n++) {
		if (
			((u[n] & 0xFC00) == 0xD800) &&
			((u[n + 1] & 0xFC00) == 0xDC00)
			) {
			c = u[n++] & 0x3FF;
			c = (c << 10) | (u[n] & 0x3FF);
			c += 0x10000;
		}
		else {
			c = u[n];
		}
		UTF_32.append(1, c);
	}
	return UTF_32;
}


u32string& UTF_CONVERTER::UTF32(const string& UTF_8) {
	return UTF32(UTF_8.c_str());
}


u32string& UTF_CONVERTER::UTF32(const u16string& UTF_16) {
	return UTF32(UTF_16.c_str());
}


std::u16string& UTF_CONVERTER::UTF16(char32_t UTF_32) {
	char32_t s[2] = { UTF_32, 0 };
	return UTF16(s);
}


u16string& UTF_CONVERTER::UTF16(const char* UTF_8) {
	return UTF16(UTF32(UTF_8));
}


u16string& UTF_CONVERTER::UTF16(const char32_t* UTF_32) {
	UTF_16.clear();

	for (int n = 0; UTF_32[n]; n++) {
		if (UTF_32[n] > 0xFFFF) {
			uint32_t c = UTF_32[n] - 0x10000;
			UTF_16.append(1, (char16_t)(0xD800 | (c >> 10)));
			UTF_16.append(1, (char16_t)(0xDC00 | (c & 0x3FF)));
		}
		else {
			UTF_16.append(1, (char16_t)(UTF_32[n]));
		}
	}

	return UTF_16;
}


u16string& UTF_CONVERTER::UTF16(const string& UTF_8) {
	return UTF16(UTF_8.c_str());
}


u16string& UTF_CONVERTER::UTF16(const u32string& UTF_32) {
	return UTF16(UTF_32.c_str());
}


std::string& UTF_CONVERTER::UTF8(char32_t UTF_32) {
	char32_t s[2] = { UTF_32, 0 };
	return UTF8(s);
}


string& UTF_CONVERTER::UTF8(const char16_t* UTF_16) {
	return UTF8(UTF32(UTF_16));
}


string& UTF_CONVERTER::UTF8(const char32_t* UTF_32) {
	UTF_8.clear();

	int      n;
	uint32_t c;
	uint8_t  Head;
	uint8_t  Lead;
	uint8_t  Cont[6];

	for (int j = 0; UTF_32[j]; j++) {
		c = UTF_32[j];
		n = 0;
		Head = 0x3F; // Active bits in the leading byte
		Lead = 0x80; // Leading byte

		if (c < 0x80) {
			UTF_8.append(1, (char)c);

		}
		else {
			while (c > Head) { // Doesn't fit in the leading byte
				Cont[n] = 0x80 | (c & 0x3F);
				Lead = 0x80 | (Lead >> 1);
				Head >>= 1;
				c >>= 6;
				n++;
			}
			UTF_8.append(1, (char)(Lead | c));
			for (n; n >= 0; n) UTF_8.append(1, (char)(Cont[n]));
		}
	}

	return UTF_8;
}


string& UTF_CONVERTER::UTF8(const u16string& UTF_16) {
	return UTF8(UTF_16.c_str());
}


string& UTF_CONVERTER::UTF8(const u32string& UTF_32) {
	return UTF8(UTF_32.c_str());
}


std::string& UTF_CONVERTER::UpperCase(const std::string& String) {
	return UTF8(UpperCase(UTF32(String)));
}


std::u16string& UTF_CONVERTER::UpperCase(const std::u16string& String) {
	return UTF16(UpperCase(UTF32(String)));
}


void UTF_CONVERTER::ProcessEqual4(std::size_t n) {
	if (UTF_32[n] == 0x2CD9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CDB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CDD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CDF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CE1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CE3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CEC) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CEE) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA641) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA643) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA645) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA647) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA649) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA64B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA64D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA64F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA651) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA653) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA655) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA657) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA659) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA65B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA65D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA65F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA661) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA663) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA665) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA667) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA669) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA66B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA66D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA681) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA683) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA685) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA687) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA689) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA68B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA68D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA68F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA691) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA693) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA695) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA697) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA723) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA725) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA727) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA729) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA72B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA72D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA72F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA733) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA735) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA737) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA739) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA73B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA73D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA73F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA741) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA743) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA745) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA747) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA749) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA74B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA74D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA74F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA751) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA753) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA755) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA757) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA759) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA75B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA75D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA75F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA761) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA763) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA765) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA767) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA769) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA76B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA76D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA76F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA77A) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA77C) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA77F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA781) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA783) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA785) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA787) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA78C) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA791) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA7A1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA7A3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA7A5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA7A7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0xA7A9) UTF_32[n] -= 1;
}

void UTF_CONVERTER::ProcessEqual3(std::size_t n) {
	if (UTF_32[n] == 0x1E83) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E85) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E87) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E89) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E8B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E8D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E8F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E91) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E93) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E95) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E9B) UTF_32[n] -= 59;
	else if (UTF_32[n] == 0x1EA1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EA3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EA5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EA7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EA9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EAB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EAD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EAF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EB1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EB3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EB5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EB7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EB9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EBB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EBD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EBF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EC1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EC3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EC5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EC7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EC9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1ECB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1ECD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1ECF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1ED1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1ED3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1ED5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1ED7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1ED9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EDB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EDD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EDF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EE1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EE3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EE5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EE7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EE9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EEB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EED) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EEF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EF1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EF3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EF5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EF7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EF9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EFB) UTF_32[n] -= 4097;
	else if (UTF_32[n] == 0x1EFD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1EFF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1F51) UTF_32[n] += 8;
	else if (UTF_32[n] == 0x1F53) UTF_32[n] += 8;
	else if (UTF_32[n] == 0x1F55) UTF_32[n] += 8;
	else if (UTF_32[n] == 0x1F57) UTF_32[n] += 8;
	else if (UTF_32[n] == 0x1FB3) UTF_32[n] += 9;
	else if (UTF_32[n] == 0x1FBE) UTF_32[n] -= 7205;
	else if (UTF_32[n] == 0x1FC3) UTF_32[n] += 9;
	else if (UTF_32[n] == 0x1FE5) UTF_32[n] += 7;
	else if (UTF_32[n] == 0x1FF3) UTF_32[n] += 9;
	else if (UTF_32[n] == 0x214E) UTF_32[n] -= 28;
	else if (UTF_32[n] == 0x2184) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C61) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C65) UTF_32[n] -= 10795;
	else if (UTF_32[n] == 0x2C66) UTF_32[n] -= 10792;
	else if (UTF_32[n] == 0x2C68) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C6A) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C6C) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C73) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C76) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C81) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C83) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C85) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C87) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C89) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C8B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C8D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C8F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C91) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C93) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C95) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C97) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C99) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C9B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C9D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2C9F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CA1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CA3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CA5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CA7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CA9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CAB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CAD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CAF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CB1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CB3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CB5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CB7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CB9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CBB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CBD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CBF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CC1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CC3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CC5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CC7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CC9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CCB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CCD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CCF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CD1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CD3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CD5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x2CD7) UTF_32[n] -= 1;
	else {
		ProcessEqual4(n);
	}
}

void UTF_CONVERTER::ProcessEqual2(std::size_t n) {
	if (UTF_32[n] == 0x04BB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04BD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04BF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04C2) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04C4) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04C6) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04C8) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04CA) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04CC) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04CE) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04CF) UTF_32[n] -= 15;
	else if (UTF_32[n] == 0x04D1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04D3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04D5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04D7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04D9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04DB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04DD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04DF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04E1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04E3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04E5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04E7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04E9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04EB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04ED) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04EF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04F1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04F3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04F5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04F7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04F9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04FB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04FD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04FF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0501) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0503) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0505) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0507) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0509) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x050B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x050D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x050F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0511) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0513) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0515) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0517) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0519) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x051B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x051D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x051F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0521) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0523) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0525) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0527) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1D79) UTF_32[n] += 35332;
	else if (UTF_32[n] == 0x1D7D) UTF_32[n] += 3814;
	else if (UTF_32[n] == 0x1E01) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E03) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E05) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E07) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E09) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E0B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E0D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E0F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E11) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E13) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E15) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E17) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E19) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E1B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E1D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E1F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E21) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E23) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E25) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E27) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E29) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E2B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E2D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E2F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E31) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E33) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E35) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E37) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E39) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E3B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E3D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E3F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E41) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E43) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E45) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E47) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E49) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E4B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E4D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E4F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E51) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E53) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E55) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E57) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E59) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E5B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E5D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E5F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E61) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E63) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E65) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E67) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E69) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E6B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E6D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E6F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E71) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E73) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E75) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E77) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E79) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E7B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E7D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E7F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x1E81) UTF_32[n] -= 1;
	else {
		ProcessEqual3(n);
	}
}

void UTF_CONVERTER::ProcessEqual1(std::size_t n) {
	if (UTF_32[n] == 0x020B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x020D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x020F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0211) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0213) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0215) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0217) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0219) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x021B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x021D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x021F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0223) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0225) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0227) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0229) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x022B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x022D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x022F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0231) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0233) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x023C) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0242) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0247) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0249) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x024B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x024D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x024F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0250) UTF_32[n] += 10783;
	else if (UTF_32[n] == 0x0251) UTF_32[n] += 2588;
	else if (UTF_32[n] == 0x0252) UTF_32[n] += 10782;
	else if (UTF_32[n] == 0x0253) UTF_32[n] -= 210;
	else if (UTF_32[n] == 0x0254) UTF_32[n] -= 206;
	else if (UTF_32[n] == 0x0259) UTF_32[n] -= 202;
	else if (UTF_32[n] == 0x025B) UTF_32[n] -= 203;
	else if (UTF_32[n] == 0x0260) UTF_32[n] -= 205;
	else if (UTF_32[n] == 0x0263) UTF_32[n] -= 207;
	else if (UTF_32[n] == 0x0265) UTF_32[n] += 42280;
	else if (UTF_32[n] == 0x0268) UTF_32[n] -= 209;
	else if (UTF_32[n] == 0x0269) UTF_32[n] -= 211;
	else if (UTF_32[n] == 0x026B) UTF_32[n] += 10743;
	else if (UTF_32[n] == 0x026F) UTF_32[n] -= 211;
	else if (UTF_32[n] == 0x0271) UTF_32[n] += 10749;
	else if (UTF_32[n] == 0x0272) UTF_32[n] -= 213;
	else if (UTF_32[n] == 0x0275) UTF_32[n] -= 214;
	else if (UTF_32[n] == 0x027D) UTF_32[n] += 10727;
	else if (UTF_32[n] == 0x0283) UTF_32[n] -= 218;
	else if (UTF_32[n] == 0x0288) UTF_32[n] -= 218;
	else if (UTF_32[n] == 0x0289) UTF_32[n] -= 69;
	else if (UTF_32[n] == 0x028C) UTF_32[n] -= 71;
	else if (UTF_32[n] == 0x0292) UTF_32[n] -= 219;
	else if (UTF_32[n] == 0x0345) UTF_32[n] += 84;
	else if (UTF_32[n] == 0x0371) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0373) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0377) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03AC) UTF_32[n] -= 38;
	else if (UTF_32[n] == 0x03C2) UTF_32[n] -= 31;
	else if (UTF_32[n] == 0x03CC) UTF_32[n] -= 64;
	else if (UTF_32[n] == 0x03D0) UTF_32[n] -= 62;
	else if (UTF_32[n] == 0x03D1) UTF_32[n] -= 57;
	else if (UTF_32[n] == 0x03D5) UTF_32[n] -= 47;
	else if (UTF_32[n] == 0x03D6) UTF_32[n] -= 54;
	else if (UTF_32[n] == 0x03D7) UTF_32[n] -= 8;
	else if (UTF_32[n] == 0x03D9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03DB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03DD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03DF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03E1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03E3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03E5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03E7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03E9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03EB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03ED) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03EF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03F0) UTF_32[n] -= 86;
	else if (UTF_32[n] == 0x03F1) UTF_32[n] -= 80;
	else if (UTF_32[n] == 0x03F2) UTF_32[n] += 7;
	else if (UTF_32[n] == 0x03F5) UTF_32[n] -= 96;
	else if (UTF_32[n] == 0x03F8) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x03FB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0461) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0463) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0465) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0467) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0469) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x046B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x046D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x046F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0471) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0473) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0475) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0477) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0479) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x047B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x047D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x047F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0481) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x048B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x048D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x048F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0491) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0493) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0495) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0497) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0499) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x049B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x049D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x049F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04A1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04A3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04A5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04A7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04A9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04AB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04AD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04AF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04B1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04B3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04B5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04B7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x04B9) UTF_32[n] -= 1;
	else {
		ProcessEqual2(n);
	}
}

void UTF_CONVERTER::ProcessEqual(std::size_t n) {
	if (UTF_32[n] == 0x00B5) UTF_32[n] += 743;
	else if (UTF_32[n] == 0x00FF) UTF_32[n] += 121;
	else if (UTF_32[n] == 0x0101) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0103) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0105) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0107) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0109) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x010B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x010D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x010F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0111) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0113) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0115) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0117) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0119) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x011B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x011D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x011F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0121) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0123) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0125) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0127) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0129) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x012B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x012D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x012F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0131) UTF_32[n] -= 232;
	else if (UTF_32[n] == 0x0133) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0135) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0137) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x013A) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x013C) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x013E) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0140) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0142) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0144) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0146) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0148) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x014B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x014D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x014F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0151) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0153) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0155) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0157) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0159) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x015B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x015D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x015F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0161) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0163) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0165) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0167) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0169) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x016B) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x016D) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x016F) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0171) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0173) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0175) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0177) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x017A) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x017C) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x017E) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x017F) UTF_32[n] -= 300;
	else if (UTF_32[n] == 0x0180) UTF_32[n] += 195;
	else if (UTF_32[n] == 0x0183) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0185) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0188) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x018C) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0192) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0195) UTF_32[n] += 97;
	else if (UTF_32[n] == 0x0199) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x019A) UTF_32[n] += 163;
	else if (UTF_32[n] == 0x019E) UTF_32[n] += 130;
	else if (UTF_32[n] == 0x01A1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01A3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01A5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01A8) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01AD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01B0) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01B4) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01B6) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01B9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01BD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01BF) UTF_32[n] += 56;
	else if (UTF_32[n] == 0x01C5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01C6) UTF_32[n] -= 2;
	else if (UTF_32[n] == 0x01C8) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01C9) UTF_32[n] -= 2;
	else if (UTF_32[n] == 0x01CB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01CC) UTF_32[n] -= 2;
	else if (UTF_32[n] == 0x01CE) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01D0) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01D2) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01D4) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01D6) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01D8) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01DA) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01DC) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01DD) UTF_32[n] -= 79;
	else if (UTF_32[n] == 0x01DF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01E1) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01E3) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01E5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01E7) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01E9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01EB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01ED) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01EF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01F2) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01F3) UTF_32[n] -= 2;
	else if (UTF_32[n] == 0x01F5) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01F9) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01FB) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01FD) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x01FF) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0201) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0203) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0205) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0207) UTF_32[n] -= 1;
	else if (UTF_32[n] == 0x0209) UTF_32[n] -= 1;
	else {
		ProcessEqual1(n);
	}
}

std::u32string& UTF_CONVERTER::UpperCase(const std::u32string& String) {
	UTF_32 = String;

	for (size_t n = 0; UTF_32[n]; n++) {
		if (UTF_32[n] >= 0x0061 && UTF_32[n] <= 0x007A) UTF_32[n] -= 32;
		else if (UTF_32[n] >= 0x00E0 && UTF_32[n] <= 0x00F6) UTF_32[n] -= 32;
		else if (UTF_32[n] >= 0x00F8 && UTF_32[n] <= 0x00FE) UTF_32[n] -= 32;
		else if (UTF_32[n] >= 0x023F && UTF_32[n] <= 0x0240) UTF_32[n] += 10815;
		else if (UTF_32[n] >= 0x0256 && UTF_32[n] <= 0x0257) UTF_32[n] -= 205;
		else if (UTF_32[n] >= 0x028A && UTF_32[n] <= 0x028B) UTF_32[n] -= 217;
		else if (UTF_32[n] >= 0x037B && UTF_32[n] <= 0x037D) UTF_32[n] += 130;
		else if (UTF_32[n] >= 0x03AD && UTF_32[n] <= 0x03AF) UTF_32[n] -= 37;
		else if (UTF_32[n] >= 0x03B1 && UTF_32[n] <= 0x03C1) UTF_32[n] -= 32;
		else if (UTF_32[n] >= 0x03C3 && UTF_32[n] <= 0x03CB) UTF_32[n] -= 32;
		else if (UTF_32[n] >= 0x03CD && UTF_32[n] <= 0x03CE) UTF_32[n] -= 63;
		else if (UTF_32[n] >= 0x0430 && UTF_32[n] <= 0x044F) UTF_32[n] -= 32;
		else if (UTF_32[n] >= 0x0450 && UTF_32[n] <= 0x045F) UTF_32[n] -= 80;
		else if (UTF_32[n] >= 0x0561 && UTF_32[n] <= 0x0586) UTF_32[n] -= 48;
		else if (UTF_32[n] >= 0x1F00 && UTF_32[n] <= 0x1F07) UTF_32[n] += 8;
		else if (UTF_32[n] >= 0x1F10 && UTF_32[n] <= 0x1F15) UTF_32[n] += 8;
		else if (UTF_32[n] >= 0x1F20 && UTF_32[n] <= 0x1F27) UTF_32[n] += 8;
		else if (UTF_32[n] >= 0x1F30 && UTF_32[n] <= 0x1F37) UTF_32[n] += 8;
		else if (UTF_32[n] >= 0x1F40 && UTF_32[n] <= 0x1F45) UTF_32[n] += 8;
		else if (UTF_32[n] >= 0x1F60 && UTF_32[n] <= 0x1F67) UTF_32[n] += 8;
		else if (UTF_32[n] >= 0x1F70 && UTF_32[n] <= 0x1F71) UTF_32[n] += 74;
		else if (UTF_32[n] >= 0x1F72 && UTF_32[n] <= 0x1F75) UTF_32[n] += 86;
		else if (UTF_32[n] >= 0x1F76 && UTF_32[n] <= 0x1F77) UTF_32[n] += 100;
		else if (UTF_32[n] >= 0x1F78 && UTF_32[n] <= 0x1F79) UTF_32[n] += 128;
		else if (UTF_32[n] >= 0x1F7A && UTF_32[n] <= 0x1F7B) UTF_32[n] += 112;
		else if (UTF_32[n] >= 0x1F7C && UTF_32[n] <= 0x1F7D) UTF_32[n] += 126;
		else if (UTF_32[n] >= 0x1F80 && UTF_32[n] <= 0x1F87) UTF_32[n] += 8;
		else if (UTF_32[n] >= 0x1F90 && UTF_32[n] <= 0x1F97) UTF_32[n] += 8;
		else if (UTF_32[n] >= 0x1FA0 && UTF_32[n] <= 0x1FA7) UTF_32[n] += 8;
		else if (UTF_32[n] >= 0x1FB0 && UTF_32[n] <= 0x1FB1) UTF_32[n] += 8;
		else if (UTF_32[n] >= 0x1FD0 && UTF_32[n] <= 0x1FD1) UTF_32[n] += 8;
		else if (UTF_32[n] >= 0x1FE0 && UTF_32[n] <= 0x1FE1) UTF_32[n] += 8;
		else if (UTF_32[n] >= 0x2170 && UTF_32[n] <= 0x217F) UTF_32[n] -= 16;
		else if (UTF_32[n] >= 0x24D0 && UTF_32[n] <= 0x24E9) UTF_32[n] -= 26;
		else if (UTF_32[n] >= 0x2C30 && UTF_32[n] <= 0x2C5E) UTF_32[n] -= 48;
		else if (UTF_32[n] >= 0x2D00 && UTF_32[n] <= 0x2D25) UTF_32[n] -= 7264;
		else if (UTF_32[n] >= 0xFF41 && UTF_32[n] <= 0xFF5A) UTF_32[n] -= 32;
		else {
			ProcessEqual(n);
		}
	}
	return UTF_32;
}


