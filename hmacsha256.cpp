#include "CatSandbox.h"
#include "hmacsha256.h"
#include "picosha2.h"

/*
    H is a cryptographic hash function,
    K is the secret key,
    m is the message to be authenticated,
    K' is another secret key, derived from the original key K (by padding K to the right
      with extra zeroes to the input block size of the hash function, or by hashing K if
      it is longer than that block size),
    || denotes concatenation,
    ⊕ denotes exclusive or (XOR),
    opad is the outer padding (0x5c5c5c…5c5c, one-block-long hexadecimal constant),

*/

bool UHmacSha256::ValidateSignature(FString messageToValidate, FString key, FString expected)
{
  auto encoded = UHmacSha256::ComputeBase64Signature(messageToValidate, key);
  return encoded == expected.Replace(TEXT("="), TEXT(""));
}

FString UHmacSha256::ComputeBase64Signature(FString message, FString key)
{
  std::vector<UBYTE> result(SHA2_OUTPUT_SIZE);
  result = UHmacSha256::ComputeSignature(message, key);
  return UBase64Codec::Base64UrlEncodeNoPad(result);
}

std::vector<UBYTE> UHmacSha256::ComputeSignature(FString message, FString key)
{
  std::string keyString(TCHAR_TO_UTF8(*key));
  std::string messageString(TCHAR_TO_UTF8(*message));
  std::vector<UBYTE> keyToUse(SHA2_BLOCK_SIZE);
  std::vector<UBYTE> messageVec(messageString.begin(), messageString.end());
  if(keyString.length() > SHA2_BLOCK_SIZE)
  {
    picosha2::hash256(keyString.begin(), keyString.end(), keyToUse.begin(), keyToUse.end());
  }
  else
  {
    keyToUse.assign(keyString.begin(), keyString.end());
  }
  for(int i = keyToUse.size(); i < SHA2_BLOCK_SIZE; i++)
  {
    keyToUse.reserve(SHA2_BLOCK_SIZE);
	keyToUse.push_back(0x00);
  }
  std::vector<UBYTE> o_key_pad(SHA2_BLOCK_SIZE);
  std::vector<UBYTE> i_key_pad(SHA2_BLOCK_SIZE);
  for(int i = 0; i < SHA2_OUTPUT_SIZE; i++)
  {
    o_key_pad[i] = keyToUse[i] ^ OPAD_CONST;
    i_key_pad[i] = keyToUse[i] ^ IPAD_CONST;
  }
  for (int i = SHA2_OUTPUT_SIZE; i < SHA2_BLOCK_SIZE; i++)
  {
	  o_key_pad[i] = OPAD_CONST;
	  i_key_pad[i] = IPAD_CONST;
  }
  std::vector<UBYTE> innerMessage;
  innerMessage.insert(innerMessage.begin(), i_key_pad.begin(), i_key_pad.end());
  innerMessage.insert(innerMessage.end(), messageVec.begin(), messageVec.end());
  //Complete the inner hash
  std::vector<UBYTE> innerHash(picosha2::k_digest_size);
  picosha2::hash256(innerMessage.begin(), innerMessage.end(), innerHash.begin(), innerHash.end());
  //Begin outer hash
  std::vector<UBYTE> outerMessage;
  outerMessage.insert(outerMessage.begin(), o_key_pad.begin(), o_key_pad.end());
  outerMessage.insert(outerMessage.end(), innerHash.begin(), innerHash.end());
  //Complete the outer hash
  std::vector<UBYTE> result(picosha2::k_digest_size);
  picosha2::hash256(outerMessage.begin(), outerMessage.end(), result.begin(), result.end());
  return result; 
}

FString UHmacSha256::Sha256EncodeHash(FString StringToHash)
{
  std::string src_str(TCHAR_TO_UTF8(*StringToHash));
  std::string hex_string;
  picosha2::hash256_hex_string(src_str, hex_string);
  return FString(hex_string.c_str());
}

std::pair<const unsigned char*, int> UHmacSha256::PrepareMessageBuffer(FString & message)
{
  const char* signedBuffer = (TCHAR_TO_UTF8(*message));
  int bufferLen = strlen(signedBuffer);
  const unsigned char* buffer = reinterpret_cast<const unsigned char*>(signedBuffer);
  return std::make_pair(buffer, bufferLen);
}
