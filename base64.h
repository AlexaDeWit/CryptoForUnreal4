#pragma once

#include "Object.h"
#include <string>
#include <vector>
#include "base64.generated.h"

#define BYTE unsigned char
#define DWORD long
// Prototype
// std::basic_string<TCHAR> base64Encode(std::vector<BYTE> inputBuffer);
// This line goes in header file

/* Define these if they aren't already in your environment
 * #define TEXT(x) Lx    //Unicode
 * #define TCHAR wchar_t //Unicode
 * #define TCHAR char    //Not unicode
 * #define TEXT(x) x     //Not unicode
 * They are defined by default in Windows.h
 */

//Lookup table for encoding
//If you want to use an alternate alphabet, change the characters here
const static TCHAR encodeLookup[] = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_");
const static TCHAR padCharacter = TEXT('=');

UCLASS(BlueprintType, Blueprintable)
  class CATSANDBOX_API UBase64Codec : public UObject
{
  GENERATED_BODY()
  public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Crypto")
      static bool Base64UrlDecode(FString input, FString & out);
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Crypto")
      static FString Base64UrlEncode(FString input);
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Crypto")
      static FString Base64UrlEncodeNoPad(FString Input);

    static FString Base64UrlEncode(std::vector<BYTE>& input);
    static FString Base64UrlEncodeNoPad(std::vector<BYTE> & Input);
    static std::basic_string<TCHAR> base64UrlEncode(std::vector<BYTE>& inputBuffer);
    static std::vector<BYTE> base64UrlDecode(const std::basic_string<TCHAR>& input);
    static FString bytesToString(std::vector<BYTE>& bytes);
    static std::basic_string<TCHAR> prepareFStringForDecode(const FString & string);
};
