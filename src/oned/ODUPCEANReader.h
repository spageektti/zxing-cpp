#pragma once
/*
* Copyright 2016 ZXing authors
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "oned/ODReader.h"

#include <string>
#include <array>

namespace ZXing {

enum class BarcodeFormat;
enum class ErrorStatus;

namespace OneD {

/**
* <p>Encapsulates functionality and implementation that is common to UPC and EAN families
* of one-dimensional barcodes.</p>
*
* @author dswitkin@google.com (Daniel Switkin)
* @author Sean Owen
* @author alasdair@google.com (Alasdair Mackintosh)
*/
class UPCEANReader : public Reader
{
public:
	virtual Result decodeRow(int rowNumber, const BitArray& row, const DecodeHints* hints) override;

	/**
	* <p>Like {@link #decodeRow(int, BitArray, java.util.Map)}, but
	* allows caller to inform method about where the UPC/EAN start pattern is
	* found. This allows this to be computed once and reused across many implementations.</p>
	*
	* @param rowNumber row index into the image
	* @param row encoding of the row of the barcode image
	* @param startGuardRange start/end column where the opening start pattern was found
	* @param hints optional hints that influence decoding
	* @return {@link Result} encapsulating the result of decoding a barcode in the row
	* @throws NotFoundException if no potential barcode is found
	* @throws ChecksumException if a potential barcode is found but does not pass its checksum
	* @throws FormatException if a potential barcode is found but format is invalid
	*/
	virtual Result decodeRow(int rowNumber, const BitArray& row, int startGuardBegin, int startGuardEnd, const DecodeHints* hints);

protected:

	/**
	* Get the format of this decoder.
	*/
	virtual BarcodeFormat expectedFormat() const = 0;

	/**
	* Subclasses override this to decode the portion of a barcode between the start
	* and end guard patterns.
	*
	* @param row row of black/white values to search
	* @param rowOffset on input, end offset of start guard pattern, and on output: horizontal offset of first pixel after the "middle" that was decoded
	* @param resultString {@link StringBuilder} to append decoded chars to
	* @throws NotFoundException if decoding could not complete successfully
	*/
	virtual ErrorStatus decodeMiddle(const BitArray& row, int &rowOffset, std::string& resultString) = 0;

	/**
	* @param s string of digits to check
	* @return {@link #checkStandardUPCEANChecksum(CharSequence)}
	* @throws FormatException if the string does not contain only digits
	*/
	virtual	ErrorStatus checkChecksum(const std::string& s);


	virtual ErrorStatus decodeEnd(const BitArray& row, int endStart, int& begin, int& end);

public:
	static ErrorStatus FindStartGuardPattern(const BitArray& row, int& begin, int& end);

	template <typename Container>
	static ErrorStatus FindGuardPattern(const BitArray& row, int rowOffset, bool whiteFirst, const Container& pattern, int& begin, int& end) {
		return FindGuardPattern(row, rowOffset, whiteFirst, pattern.data(), pattern.size(), begin, end);
	}

	/**
	* Computes the UPC/EAN checksum on a string of digits, and reports
	* whether the checksum is correct or not.
	*
	* @param s string of digits to check
	* @return true iff string of digits passes the UPC/EAN checksum algorithm
	* @throws FormatException if the string does not contain only digits
	*/
	static ErrorStatus CheckStandardUPCEANChecksum(const std::string& s);

	/**
	* Attempts to decode a single UPC/EAN-encoded digit.
	*
	* @param row row of black/white values to decode
	* @param counters the counts of runs of observed black/white/black/... values
	* @param rowOffset horizontal offset to start decoding from
	* @param patterns the set of patterns to use to decode -- sometimes different encodings
	* for the digits 0-9 are used, and this indicates the encodings for 0 to 9 that should
	* be used
	* @return horizontal offset of first pixel beyond the decoded digit
	* @throws NotFoundException if digit cannot be decoded
	*/
	template <typename Container>
	static ErrorStatus DecodeDigit(const BitArray& row, int rowOffset, const Container& patterns, std::array<int, 4>& counters, int &resultOffset) {
		return DecodeDigit(row, rowOffset, patterns.data(), patterns.size(), counters, resultOffset);
	}
	
	/**
	* Pattern marking the middle of a UPC/EAN pattern, separating the two halves.
	*/
	static const std::array<int, 5> MIDDLE_PATTERN;
	
	/**
	* "Odd", or "L" patterns used to encode UPC/EAN digits.
	*/
	static const std::array<std::array<int, 4>, 10> L_PATTERNS;

	/**
	* As above but also including the "even", or "G" patterns used to encode UPC/EAN digits.
	*/
	static const std::array<std::array<int, 4>, 20> L_AND_G_PATTERNS;

private:
	static ErrorStatus FindGuardPattern(const BitArray& row, int rowOffset, bool whiteFirst, const int* pattern, size_t length, int& begin, int& end);
	static ErrorStatus DecodeDigit(const BitArray& row, int rowOffset, const std::array<int, 4>* patterns, size_t patternCount, std::array<int, 4>& counters, int &resultOffset);
	static ErrorStatus DoFindGuardPattern(const BitArray& row, int rowOffset, bool whiteFirst, const int* pattern, int* counters, size_t length, int& begin, int& end);
};

} // OneD
} // ZXing
