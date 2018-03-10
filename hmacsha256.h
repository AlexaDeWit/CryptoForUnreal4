#pragma once

#include "Object.h"
#include "base64.h"
#include <string>
#include <cstring>
#include <vector>
#include "hmacsha256.generated.h"

#define SBYTE char
#define UBYTE unsigned char
#define SHA2_BLOCK_SIZE 64
#define SHA2_OUTPUT_SIZE 32
#define OPAD_CONST 0x5c
#define IPAD_CONST 0x36


UCLASS(BlueprintType, Blueprintable)
class CATSANDBOX_API UHmacSha256 : public UObject
{

	GENERATED_BODY()
  public:
    UFUNCTION(BlueprintCallable, Category = "Crypto")
    static bool ValidateSignature(FString messageToValidate, FString key, FString expected);
    static std::vector<UBYTE> ComputeSignature(FString message, FString key);

    UFUNCTION(BlueprintCallable, Category = "Crypto")
    static FString ComputeBase64Signature(FString message, FString key);

    UFUNCTION(BlueprintCallable, Category = "Crypto")
    static FString Sha256EncodeHash(FString StringToHash);

    static std::pair<const unsigned char*, int> PrepareMessageBuffer(FString & message);

};
