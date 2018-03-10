#include "CatSandbox.h"
#include "base64.h"

FString UBase64Codec::Base64UrlEncode(std::vector<BYTE>& input)
{
  auto encoded = UBase64Codec::base64UrlEncode(input);
  return FString(encoded.c_str());
}

FString UBase64Codec::Base64UrlEncode(FString input)
{
  auto basic = UBase64Codec::prepareFStringForDecode(input);
  std::vector<BYTE> vec(basic.begin(), basic.end());
  return UBase64Codec::Base64UrlEncode(vec);
}

FString UBase64Codec::Base64UrlEncodeNoPad(FString Input)
{
  return UBase64Codec::Base64UrlEncode(Input).Replace(TEXT("="), TEXT(""));
}

FString UBase64Codec::Base64UrlEncodeNoPad(std::vector<BYTE> & Input)
{
  auto encoded = UBase64Codec::base64UrlEncode(Input);
  return FString(encoded.c_str()).Replace(TEXT("="), TEXT(""));
}

std::vector<BYTE> UBase64Codec::base64UrlDecode(const std::basic_string<TCHAR>& input)
{
	if (input.length() % 4) //Sanity check
		throw std::runtime_error(std::string(input.begin(), input.end()));
	size_t padding = 0;
	if (input.length())
	{
		if (input[input.length()-1] == padCharacter)
			padding++;
		if (input[input.length()-2] == padCharacter)
			padding++;
	}
	//Setup a vector to hold the result
	std::vector<BYTE> decodedBytes;
	decodedBytes.reserve(((input.length()/4)*3) - padding);
	DWORD temp=0; //Holds decoded quanta
	std::basic_string<TCHAR>::const_iterator cursor = input.begin();
	while (cursor < input.end())
	{
		for (size_t quantumPosition = 0; quantumPosition < 4; quantumPosition++)
		{
			temp <<= 6;
			if       (*cursor >= 0x41 && *cursor <= 0x5A) // This area will need tweaking if
				temp |= *cursor - 0x41;		              // you are using an alternate alphabet
			else if  (*cursor >= 0x61 && *cursor <= 0x7A)
				temp |= *cursor - 0x47;
			else if  (*cursor >= 0x30 && *cursor <= 0x39)
				temp |= *cursor + 0x04;
			else if  (*cursor == 0x2B)
				temp |= 0x2D; //change to 0x2D for URL alphabet
			else if  (*cursor == 0x2F)
				temp |= 0x5F; //change to 0x5F for URL alphabet
			else if  (*cursor == padCharacter) //pad
			{
				switch( input.end() - cursor )
				{
				case 1: //One pad character
					decodedBytes.push_back((temp >> 16) & 0x000000FF);
					decodedBytes.push_back((temp >> 8 ) & 0x000000FF);
					return decodedBytes;
				case 2: //Two pad characters
					decodedBytes.push_back((temp >> 10) & 0x000000FF);
					return decodedBytes;
				default:
					throw std::runtime_error("Invalid Padding in Base 64!");
				}
			}  else
				throw std::runtime_error("Non-Valid Character in Base 64!");
			cursor++;
		}
		decodedBytes.push_back((temp >> 16) & 0x000000FF);
		decodedBytes.push_back((temp >> 8 ) & 0x000000FF);
		decodedBytes.push_back((temp      ) & 0x000000FF);
	}
	return decodedBytes;
}

std::basic_string<TCHAR> UBase64Codec::base64UrlEncode(std::vector<BYTE>& inputBuffer)
{
	std::basic_string<TCHAR> encodedString;
	encodedString.reserve(((inputBuffer.size()/3) + (inputBuffer.size() % 3 > 0)) * 4);
	DWORD temp;
	std::vector<BYTE>::iterator cursor = inputBuffer.begin();
	for(size_t idx = 0; idx < inputBuffer.size()/3; idx++)
	{
		temp  = (*cursor++) << 16; //Convert to big endian
		temp += (*cursor++) << 8;
		temp += (*cursor++);
		encodedString.append(1,encodeLookup[(temp & 0x00FC0000) >> 18]);
		encodedString.append(1,encodeLookup[(temp & 0x0003F000) >> 12]);
		encodedString.append(1,encodeLookup[(temp & 0x00000FC0) >> 6 ]);
		encodedString.append(1,encodeLookup[(temp & 0x0000003F)      ]);
	}
	switch(inputBuffer.size() % 3)
	{
	case 1:
		temp  = (*cursor++) << 16; //Convert to big endian
		encodedString.append(1,encodeLookup[(temp & 0x00FC0000) >> 18]);
		encodedString.append(1,encodeLookup[(temp & 0x0003F000) >> 12]);
		encodedString.append(2,padCharacter);
		break;
	case 2:
		temp  = (*cursor++) << 16; //Convert to big endian
		temp += (*cursor++) << 8;
		encodedString.append(1,encodeLookup[(temp & 0x00FC0000) >> 18]);
		encodedString.append(1,encodeLookup[(temp & 0x0003F000) >> 12]);
		encodedString.append(1,encodeLookup[(temp & 0x00000FC0) >> 6 ]);
		encodedString.append(1,padCharacter);
		break;
	}
	return encodedString;
}

bool UBase64Codec::Base64UrlDecode(FString input, FString & out)
{
  auto basicString = UBase64Codec::prepareFStringForDecode(input);
  try {
    auto result = UBase64Codec::base64UrlDecode(basicString);
    out = UBase64Codec::bytesToString(result);
    return true;
  } catch (std::runtime_error & e) {
    out = FString(e.what());
    return false;
  }
}

FString UBase64Codec::bytesToString(std::vector<BYTE>& bytes) 
{
  std::vector<char> signedVec(bytes.begin(), bytes.end());
  std::string data(signedVec.data(), signedVec.size());
  return FString(data.c_str());
}

std::basic_string<TCHAR> UBase64Codec::prepareFStringForDecode(const FString & string)
{
  FString prepped = string;
  if(string.Len() % 4) {
    auto neededPadding = 4 - (string.Len() % 4);
    for(int i = 0; i < neededPadding; i++) {
      prepped += padCharacter;
    }
  }
  std::wstring stdString(*prepped);
  return stdString;
}
