#include <gtest/gtest.h>
#include "pdf_engine/pdf_engine.h"
#include "gerber/gerber.h"
#include "pdf/pdf.h"

TEST(ColorTest, TestCopyRgba) {
	Color a(1.0, 0.0, 0.0, 1.0);
	auto b = a;
	EXPECT_TRUE((b == a));

	Color c(1.0, 0.0, 0.0, 1.0, 0.5);
	auto d = c;
	EXPECT_TRUE((d == c));
}

std::string GetValue(pdfString& str) {
	std::vector<char> buf(str.GetLength() + 1);
	auto size = str.GetOutput(buf.data());
	return std::string(buf.begin(), size > 64 ? buf.begin() + 64 : buf.end() - 1);
}

std::string ToString(pdfIndirect& obj) {
	std::vector<char> buf(obj.GetBodyLength() + 1);
	auto size = obj.GetBody(buf.data());
	return std::string(buf.begin(), size > 512 ? buf.begin() + 512 : buf.end() - 1);
}

TEST(EngineTest, TestRun) {
	auto pdf = std::make_shared<Pdf>();
	PdfEngine engine(pdf);
	engine.light_ = Color(1.0, 0.0, 0.0, 1.0);
	engine.dark_ = Color(0.5, 0.0, 0.1, 0.8, 0.4);
	engine.mirror_ = true;

	std::string file_name(std::string(TestData) + "susb.gbr");
	engine.Run(file_name, "title");

	EXPECT_EQ(GetValue(pdf->Title), "()");
	EXPECT_EQ(GetValue(pdf->Author), "()");
	EXPECT_EQ(GetValue(pdf->Subject), "()");
	EXPECT_EQ(GetValue(pdf->Keywords), "()");
	EXPECT_EQ(GetValue(pdf->Creator), "()");
	EXPECT_EQ(GetValue(pdf->Producer), "()");
	EXPECT_EQ(GetValue(pdf->CreationDate), "()");

	EXPECT_EQ(pdf->Info.GetBodyLength(), 21);
	EXPECT_EQ(ToString(pdf->Info), "1 0 obj <<\n>> endobj\n");

	auto first = pdf->First;
	auto second = first->Next;
	auto third = second->Next;
	auto fourth = third->Next;
	auto fifth = fourth->Next;
	auto sixth = fifth->Next;
	auto seventh = sixth->Next;
	auto last = pdf->Last;

	EXPECT_EQ(first->Object->GetBodyLength(), 21);
	EXPECT_EQ(ToString(*first->Object), "1 0 obj <<\n>> endobj\n");

	EXPECT_EQ(second->Object->GetBodyLength(), 86);
	EXPECT_EQ(ToString(*second->Object), "2 0 obj <<\n /Type /Catalog\n /PageMode /UseOutlines\n /PageLayout /SinglePage\n>> endobj\n");

	EXPECT_EQ(third->Object->GetBodyLength(), 35);
	EXPECT_EQ(ToString(*third->Object), "3 0 obj <<\n /CA 1\n /ca 1\n>> endobj\n");

	EXPECT_EQ(fourth->Object->GetBodyLength(), 72);
	EXPECT_EQ(ToString(*fourth->Object), "4 0 obj <<\n /Dest [5 0 R /Fit]\n /Title (title)\n /Parent 0 0 R\n>> endobj\n");

	EXPECT_EQ(fifth->Object->GetBodyLength(), 258);
	EXPECT_EQ(ToString(*fifth->Object), "5 0 obj <<\n /Type /Page\n /Group <</S /Transparency /CS /DeviceRGB>>\n /Parent 0 0 R\n /MediaBox \
[-706.1867717 -61.86188976 106.6025197 598.1201575]\n /Contents 6 0 R\n /Resources <<\n  /XObject <</G1 7 0 R>>\n  /ExtGState <</Opaque 3 0 \
R /O2 8 0 R>>\n >>\n>> endobj\n");

	EXPECT_EQ(sixth->Object->GetBodyLength(), 232);
	EXPECT_EQ(ToString(*sixth->Object), "6 0 obj <<\n /Length 181\n>>\nstream\n1 J\n1 j\n2.834645669 0 0 2.834645669 0 0 cm\nq\n1 0 0 RG\n1 0 \
0 rg\n/Opaque gs\n-1000000 -1000000 2000000 2000000 re\nf\nQ\n0.1 0.2 0.18 RG\n0.1 0.2 0.18 rg\nq\n/O2 gs\n-1 0 0 1 0 0 cm\n/G1 Do\nQ\nendstream\nendobj\n");

	EXPECT_EQ(seventh->Object->GetBodyLength(), 6267);
	EXPECT_EQ(ToString(*seventh->Object), "7 0 obj <<\n /Type /XObject /Subtype /Form\n /BBox [-30.78 -16.28 242.3 205.46]\n /Group \
<</S /Transparency>>\n /Length 6046\n /Filter /FlateDecode\n /Resources <<\n  /ExtGState <</Opaque 3 0 R>>\n >>\n>>\nstream\nx\x9Cu\x95I\
\x92$\xBB\x8El\xE7w\x15" "9\xFE\"\xE9" "b\x8D[\xB7\x8C\xBF\x86'R\xA3\xFB\x6\xB5\x83\xAD\x17@\x10\xE0Q\x86\xC5(\x93\xCA" "0\x12\x80*\x8F\
\xFF\xEF?\x7F\xD7?\xCBg\xFB\xF3\xDF\x7F\xFEn\xFB\xE7<\xEF\xBD-\xFFm\xCB\xFB^\xBE\xE7" "7\x94\xBF\xDB\xF7\xB3\xDC\xFBsng\xFB\xE2\xFEn\xCFz\
\x9D\xCB\xD3v\xB6}\x7F\xEE\xE3\xBB\xD9\xCEw\xFF\xEE\xEB\xF7\xB8\x8E\xF5\xCF\x7F\xEC\x94\xEFg\xBF\xBF\xCBro_\xDB;\xEE\xEF\xBE_\xFB\xFD\xC4y\
\xDF\xB8\xEE\xBA\xAF{?\xAE" "e=\x86\x9A\x1F\xC7" "b\xFB<\xD7\xBE\xB6\xAAR\xDA?\xEB}l^\x9D\xDC" "a\xFA~\xDFW\xABR\n\xB3\x8D" "c\xB7\x8B\xFC\
\xFFqtuc[\xE7m\xBD\xFA\xF7l\xDB\xF4\xCB\xAF\xAD\xC9\xA4`\xDF\xB7\xAF\xDA\xC2\x8A\x92\x16\xEA+iW\xEE\x90!M\x85\xC9h\xA5\x1Bw\x81m\xB7u\x9FL\
|hSk\xD5\xB8\r\x1C\xAA\x9C\xA9.H)\xB3y\xF2\xA7\xEA\xB9\\\xD0\xF2\x11" "9\xFA\xCF?\xFF\xF3\xFF\xDAt\xAC\xB2\x83\xA9\nAs\x15\x9Az\xE1R\xBB\xE7\
\x9E\xFC\xB3\x8D\xABUw\xCE\xB9\xF2\xAD\xBB\x15\xEA\xFE" "e@lTK\x94\xA8\xE2\xAE\xC9:>\xDB\x1C-\xD3\xBE\xAF\xD9\xB2\r3");

	EXPECT_EQ(last->Object->GetBodyLength(), 39);
	EXPECT_EQ(ToString(*last->Object), "8 0 obj <<\n /CA 0.4\n /ca 0.4\n>> endobj\n");
}
