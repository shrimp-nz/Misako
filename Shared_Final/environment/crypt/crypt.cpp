#include <random>

#include <cryptopp/cryptlib.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/blowfish.h>
#include <cryptopp/base64.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <cryptopp/sha3.h>
#include <cryptopp/hex.h>
#include <cryptopp/gcm.h>
#include <cryptopp/eax.h>
#include <cryptopp/md5.h>

#include "crypt.hpp"

#include <dependencies/luau/VM/src/lfunc.h>

#include <utilities/tools/tools.hpp>
#include <utilities/console_debug/console_dbg.hpp>

#include <bypasses/callcheck/callcheck.hpp>


/*
* Note: Methods have been copied from Synapse to produce an exact replica of its environment.
*/

enum modes
{
	//AES
	AES_CBC,
	AES_CFB,
	AES_CTR,
	AES_OFB,
	AES_GCM,
	AES_EAX,

	//Blowfish
	BF_CBC,
	BF_CFB,
	BF_OFB
};

std::map<std::string, modes> translations =
{
	{ "aes-cbc", AES_CBC },
	{ "aes_cbc", AES_CBC },

	{ "aes-cfb", AES_CFB },
	{ "aes_cfb", AES_CFB },

	{ "aes-ctr", AES_CTR },
	{ "aes_ctr", AES_CTR },

	{ "aes-ofb", AES_OFB },
	{ "aes_ofb", AES_OFB },

	{ "aes-gcm", AES_GCM },
	{ "aes_gcm", AES_GCM },

	{ "aes-eax", AES_EAX },
	{ "aes_eax", AES_EAX },

	//Blowfish
	{ "blowfish-cbc", BF_CBC },
	{ "blowfish_cbc", BF_CBC },
	{ "bf-cbc", BF_CBC },
	{ "bf_cbc", BF_CBC },

	{ "blowfish-cfb", BF_CFB },
	{ "blowfish_cfb", BF_CFB },
	{ "bf-cfb", BF_CFB },
	{ "bf_cfb", BF_CFB },

	{ "blowfish-ofb", BF_OFB },
	{ "blowfish_ofb", BF_OFB },
	{ "bf-ofb", BF_OFB },
	{ "bf_ofb", BF_OFB },
};

enum hash_mode
{
	//MD5
	MD5,

	//SHA1
	SHA1,

	//SHA2
	SHA224,
	SHA256,
	SHA384,
	SHA512,

	//SHA3
	SHA3_256,
	SHA3_384,
	SHA3_512,
};

std::map<std::string, hash_mode> hash_translations =
{
	//MD5
	{ "md5", MD5 },

	//SHA1
	{ "sha1", SHA1 },

	//SHA2
	{ "sha224", SHA224 },
	{ "sha256", SHA256 },
	{ "sha384", SHA384 },
	{ "sha512", SHA512 },

	//SHA3
	{ "sha3-256", SHA3_256 },
	{ "sha3_256", SHA3_256 },
	{ "sha3-384", SHA3_384 },
	{ "sha3_384", SHA3_384 },
	{ "sha3-512", SHA3_512 },
	{ "sha3_512", SHA3_512 },
};

namespace helper 
{
	__forceinline std::vector<std::string> split(std::string data, char delimiter)
	{
		std::vector<std::string> ret;

		for (std::size_t position = data.find(delimiter); position != std::string::npos; position = data.find(delimiter))
		{
			ret.push_back(data.substr(0, position));
			data.erase(0, position + 1);
		}

		ret.push_back(data);

		return ret;
	}

	__forceinline std::string base64_encode(const char* data, std::size_t len)
	{
		std::string encoded;

		CryptoPP::Base64Encoder b64encoder(nullptr, false);
		b64encoder.Put(reinterpret_cast<const unsigned char*>(data), len);
		b64encoder.MessageEnd();

		encoded.resize(b64encoder.MaxRetrievable());
		b64encoder.Get(reinterpret_cast<unsigned char*>(&encoded[0]), encoded.size());

		return encoded;
	}

	__forceinline std::string base64_decode(const char* data, std::size_t len)
	{
		std::string decoded;

		CryptoPP::Base64Decoder b64decoder;
		b64decoder.Put(reinterpret_cast<const unsigned char*>(data), len);
		b64decoder.MessageEnd();

		decoded.resize(b64decoder.MaxRetrievable());
		b64decoder.Get(reinterpret_cast<unsigned char*>(&decoded[0]), decoded.size());

		return decoded;
	}

	template<typename algorithm>
	__forceinline std::string encrypt_data(misako_State* RL, const char* data, std::size_t data_len, const char* key, std::size_t key_len, const char* iv, std::size_t iv_len)
	{
		std::string output;

		algorithm encryptor;
		encryptor.SetKeyWithIV(
			reinterpret_cast<const unsigned char*>(key),
			key_len,
			reinterpret_cast<const unsigned char*>(iv),
			iv_len
		);

		CryptoPP::StringSource(std::string(data, data_len), true,
			new CryptoPP::StreamTransformationFilter(encryptor,
				new CryptoPP::StringSink(output)
			)
		);

		return base64_encode(output.c_str(), output.size());
	}

	template<typename algorithm>
	__forceinline std::string encrypt_data_aef(misako_State* RL, const char* data, std::size_t data_len, const char* key, std::size_t key_len, const char* iv, std::size_t iv_len)
	{
		std::string output;

		algorithm encryptor;
		encryptor.SetKeyWithIV(
			reinterpret_cast<const unsigned char*>(key),
			key_len,
			reinterpret_cast<const unsigned char*>(iv),
			iv_len
		);

		CryptoPP::AuthenticatedEncryptionFilter aef(encryptor,
			new CryptoPP::StringSink(output)
		);

		aef.Put(reinterpret_cast<const unsigned char*>(data), data_len);
		aef.MessageEnd();

		return base64_encode(output.c_str(), output.size());
	}

	template<typename algorithm>
	__forceinline std::string decrypt_data(misako_State* RL, const char* data, std::size_t data_len, const char* key, std::size_t key_len, const char* iv, std::size_t iv_len)
	{
		std::string output;

		algorithm decryptor;
		decryptor.SetKeyWithIV(
			reinterpret_cast<const unsigned char*>(key),
			key_len,
			reinterpret_cast<const unsigned char*>(iv),
			iv_len
		);

		std::string decoded = base64_decode(data, data_len);

		CryptoPP::StringSource(decoded, true,
			new CryptoPP::StreamTransformationFilter(decryptor,
				new CryptoPP::StringSink(output)
			)
		);

		return output;
	}

	template<typename algorithm>
	__forceinline std::string decrypt_data_adf(misako_State* RL, const char* data, std::size_t data_len, const char* key, std::size_t key_len, const char* iv, std::size_t iv_len)
	{
		std::string output;

		algorithm decryptor;
		decryptor.SetKeyWithIV(
			reinterpret_cast<const unsigned char*>(key),
			key_len,
			reinterpret_cast<const unsigned char*>(iv),
			iv_len
		);

		std::string decoded = base64_decode(data, data_len);

		CryptoPP::AuthenticatedDecryptionFilter adf(decryptor,
			new CryptoPP::StringSink(output)
		);

		adf.Put(reinterpret_cast<const unsigned char*>(decoded.c_str()), decoded.size());
		adf.MessageEnd();

		return output;
	}

	__forceinline std::string run_algo(misako_State* RL, std::string algo, const char* mode, const char* data, std::size_t data_len, const char* key, std::size_t key_len, const char* iv, std::size_t iv_len)
	{
		std::transform(algo.begin(), algo.end(), algo.begin(), tolower);

		switch (translations.find(algo.c_str())->second)
		{
		case AES_CBC:
			if (!strcmp(mode, "enc"))
				return encrypt_data<CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption>(RL, data, data_len, key, key_len, iv, iv_len);
			else
				return decrypt_data<CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption>(RL, data, data_len, key, key_len, iv, iv_len);
			break;
		case AES_CFB:
			if (!strcmp(mode, "enc"))
				return encrypt_data<CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption>(RL, data, data_len, key, key_len, iv, iv_len);
			else
				return decrypt_data<CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption>(RL, data, data_len, key, key_len, iv, iv_len);
			break;
		case AES_CTR:
			if (!strcmp(mode, "enc"))
				return encrypt_data<CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption>(RL, data, data_len, key, key_len, iv, iv_len);
			else
				return decrypt_data<CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption>(RL, data, data_len, key, key_len, iv, iv_len);
			break;
		case AES_OFB:
			if (!strcmp(mode, "enc"))
				return encrypt_data<CryptoPP::OFB_Mode<CryptoPP::AES>::Encryption>(RL, data, data_len, key, key_len, iv, iv_len);
			else
				return decrypt_data<CryptoPP::OFB_Mode<CryptoPP::AES>::Decryption>(RL, data, data_len, key, key_len, iv, iv_len);
			break;
		case AES_GCM:
			if (!strcmp(mode, "enc"))
				return encrypt_data_aef<CryptoPP::GCM<CryptoPP::AES>::Encryption>(RL, data, data_len, key, key_len, iv, iv_len);
			else
				return decrypt_data_adf<CryptoPP::GCM<CryptoPP::AES>::Decryption>(RL, data, data_len, key, key_len, iv, iv_len);
			break;
		case AES_EAX:
			if (!strcmp(mode, "enc"))
				return encrypt_data_aef<CryptoPP::EAX<CryptoPP::AES>::Encryption>(RL, data, data_len, key, key_len, iv, iv_len);
			else
				return decrypt_data_adf<CryptoPP::EAX<CryptoPP::AES>::Decryption>(RL, data, data_len, key, key_len, iv, iv_len);
			break;
		case BF_CBC:
			if (!strcmp(mode, "enc"))
				return encrypt_data<CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Encryption>(RL, data, data_len, key, key_len, iv, iv_len);
			else
				return decrypt_data<CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Decryption>(RL, data, data_len, key, key_len, iv, iv_len);
			break;
		case BF_CFB:
			if (!strcmp(mode, "enc"))
				return encrypt_data<CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Encryption>(RL, data, data_len, key, key_len, iv, iv_len);
			else
				return decrypt_data<CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Decryption>(RL, data, data_len, key, key_len, iv, iv_len);
			break;
		case BF_OFB:
			if (!strcmp(mode, "enc"))
				return encrypt_data<CryptoPP::OFB_Mode<CryptoPP::Blowfish>::Encryption>(RL, data, data_len, key, key_len, iv, iv_len);
			else
				return decrypt_data<CryptoPP::OFB_Mode<CryptoPP::Blowfish>::Decryption>(RL, data, data_len, key, key_len, iv, iv_len);
			break;
		default:
			luaL_error(RL, "algorithm not found");
			return "";
			break;
		}
	}

	template<typename algorithm>
	__forceinline std::string hash(const char* data, std::size_t data_len)
	{
		std::string output;
		algorithm hash;

		CryptoPP::StringSource(std::string(data, data_len), true,
			new CryptoPP::HashFilter(hash,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(output), false
				)
			)
		);

		return output;
	}

	__forceinline std::string run_hash(misako_State* RL, std::string algo, const char* data, std::size_t data_len)
	{
		std::transform(algo.begin(), algo.end(), algo.begin(), tolower);

		switch (hash_translations.find(algo.c_str())->second)
		{
		case MD5:
			return hash<CryptoPP::MD5>(data, data_len);
			break;
		case SHA1:
			return hash<CryptoPP::SHA1>(data, data_len);
			break;
		case SHA224:
			return hash<CryptoPP::SHA224>(data, data_len);
			break;
		case SHA256:
			return hash<CryptoPP::SHA256>(data, data_len);
			break;
		case SHA384:
			return hash<CryptoPP::SHA384>(data, data_len);
			break;
		case SHA512:
			return hash<CryptoPP::SHA512>(data, data_len);
			break;
		case SHA3_256:
			return hash<CryptoPP::SHA3_256>(data, data_len);
			break;
		case SHA3_384:
			return hash<CryptoPP::SHA3_384>(data, data_len);
			break;
		case SHA3_512:
			return hash<CryptoPP::SHA3_512>(data, data_len);
			break;
		default:
			luaL_error(RL, "algorithm not found");
			return "";
			break;
		}
	}
}

namespace crypt_environment 
{
	std::int32_t encrypt(misako_State* RL)
	{
		luaL_checktype(RL, 1, LUA_TSTRING);
		luaL_checktype(RL, 2, LUA_TSTRING);

		std::size_t data_len;
		const char* data = lua_tolstring(RL, 1, &data_len);

		std::size_t key_len;
		const char* key = lua_tolstring(RL, 2, &key_len);

		try
		{
			char iv[12];
			CryptoPP::AutoSeededRandomPool srp;
			srp.GenerateBlock(reinterpret_cast<unsigned char*>(&iv[0]), 12);

			char derived[32];
			CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA384> kdf;
			kdf.DeriveKey(reinterpret_cast<unsigned char*>(&derived[0]), 32, 0, reinterpret_cast<const unsigned char*>(key), key_len, NULL, 0, 10000);

			std::string output = helper::encrypt_data_aef<CryptoPP::GCM<CryptoPP::AES>::Encryption>(RL,
				data,
				data_len,
				derived,
				32,
				iv,
				12
				);

			output += "|" + helper::base64_encode(iv, 12);
			output = helper::base64_encode(output.c_str(), output.size());

			lua_pushlstring(RL, output.c_str(), output.size());
		}
		catch (CryptoPP::Exception& e)
		{
			luaL_error(RL, e.what());
		}

		return 1;
	}

	std::int32_t decrypt(misako_State* RL)
	{
		luaL_checktype(RL, 1, LUA_TSTRING);
		luaL_checktype(RL, 2, LUA_TSTRING);

		std::size_t data_len;
		const char* data = lua_tolstring(RL, 1, &data_len);

		std::size_t key_len;
		const char* key = lua_tolstring(RL, 2, &key_len);

		try
		{
			char derived[32];
			CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA384> kdf;
			kdf.DeriveKey(reinterpret_cast<unsigned char*>(&derived[0]), 32, 0, reinterpret_cast<const unsigned char*>(key), key_len, NULL, 0, 10000);

			std::vector<std::string> _split = helper::split(helper::base64_decode(data, data_len), '|');
			std::string data1 = _split.at(0);
			std::string iv = helper::base64_decode(_split.at(1).c_str(), _split.at(1).size());


			std::string output = helper::decrypt_data_adf<CryptoPP::GCM<CryptoPP::AES>::Decryption>(RL,
				data1.c_str(),
				data1.size(),
				reinterpret_cast<const char*>(derived),
				32,
				iv.c_str(),
				iv.size()
				);

			lua_pushlstring(RL, output.c_str(), output.size());
		}
		catch (CryptoPP::Exception& e)
		{
			luaL_error(RL, e.what());
		}

		return 1;
	}

	namespace aes
	{
		std::int32_t encrypt(misako_State* RL)
		{
			luaL_checktype(RL, 1, LUA_TSTRING);
			luaL_checktype(RL, 2, LUA_TSTRING);
			luaL_checktype(RL, 3, LUA_TSTRING);
			luaL_checktype(RL, 4, LUA_TSTRING);

			const char* algo = lua_tostring(RL, 1);

			std::size_t data_len;
			const char* data = lua_tolstring(RL, 1, &data_len);

			std::size_t key_len;
			const char* key = lua_tolstring(RL, 2, &key_len);

			std::size_t iv_len;
			const char* iv = lua_tolstring(RL, 3, &iv_len);

			std::string output;

			try
			{
				output = helper::run_algo(RL, "aes-cbc", "enc", data, data_len, key, key_len, iv, iv_len);
			}
			catch (CryptoPP::Exception& e)
			{
				luaL_error(RL, e.what());
			}

			lua_pushlstring(RL, output.c_str(), output.size());
			return 1;
		}

		std::int32_t decrypt(misako_State* RL)
		{
			luaL_checktype(RL, 1, LUA_TSTRING);
			luaL_checktype(RL, 2, LUA_TSTRING);
			luaL_checktype(RL, 3, LUA_TSTRING);
			luaL_checktype(RL, 4, LUA_TSTRING);

			const char* algo = lua_tostring(RL, 1);

			std::size_t data_len;
			const char* data = lua_tolstring(RL, 1, &data_len);

			std::size_t key_len;
			const char* key = lua_tolstring(RL, 2, &key_len);

			std::size_t iv_len;
			const char* iv = lua_tolstring(RL, 3, &iv_len);

			std::string output;

			try
			{
				output = helper::run_algo(RL, "aes-cbc", "dec", data, data_len, key, key_len, iv, iv_len);
			}
			catch (CryptoPP::Exception& e)
			{
				luaL_error(RL, e.what());
			}

			lua_pushlstring(RL, output.c_str(), output.size());
			return 1;
		}
	}

	namespace base64 
	{
		std::int32_t encode(misako_State* RL)
		{
			luaL_checktype(RL, 1, LUA_TSTRING);

			std::size_t len = 0;
			const char* data = lua_tolstring(RL, 1, &len);

			try
			{
				std::string encoded;
				CryptoPP::Base64Encoder b64encoder(nullptr, false);
				b64encoder.Put(reinterpret_cast<const unsigned char*>(data), len);
				b64encoder.MessageEnd();

				encoded.resize(b64encoder.MaxRetrievable());
				b64encoder.Get(reinterpret_cast<unsigned char*>(&encoded[0]), encoded.size());

				lua_pushlstring(RL, encoded.c_str(), encoded.size());
			}
			catch (CryptoPP::Exception& e)
			{
				luaL_error(RL, e.what());
			}

			return 1;
		}

		std::int32_t decode(misako_State* RL)
		{
			luaL_checktype(RL, 1, LUA_TSTRING);

			std::size_t len = 0;
			const char* data = lua_tolstring(RL, 1, &len);

			try
			{
				std::string decoded;
				CryptoPP::Base64Decoder b64decoder;
				b64decoder.Put(reinterpret_cast<const unsigned char*>(data), len);
				b64decoder.MessageEnd();

				decoded.resize(b64decoder.MaxRetrievable());
				b64decoder.Get(reinterpret_cast<unsigned char*>(&decoded[0]), decoded.size());

				lua_pushlstring(RL, decoded.c_str(), decoded.size());
			}
			catch (CryptoPP::Exception& e)
			{
				luaL_error(RL, e.what());
			}

			return 1;
		}
	}

	std::int32_t hash(misako_State* RL)
	{
		luaL_checktype(RL, 1, LUA_TSTRING);

		std::size_t len = 0;
		const char* data = lua_tolstring(RL, 1, &len);

		std::string hashed;
		std::string digest;

		try
		{
			CryptoPP::SHA384 hash;
			CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hashed), false);
			hash.Update(reinterpret_cast<const unsigned char*>(data), len);
			digest.resize(hash.DigestSize());
			hash.Final(reinterpret_cast<unsigned char*>(&digest[0]));

			CryptoPP::StringSource _unused(digest, true, new CryptoPP::Redirector(encoder));

			lua_pushlstring(RL, hashed.c_str(), hashed.size());
		}
		catch (CryptoPP::Exception& e)
		{
			luaL_error(RL, e.what());
		}

		return 1;
	}

	std::int32_t derive(misako_State* RL)
	{
		luaL_checktype(RL, 1, LUA_TSTRING);
		luaL_checktype(RL, 2, LUA_TNUMBER);

		std::size_t secret_len = 0;
		const char* secret = lua_tolstring(RL, 1, &secret_len);

		unsigned int derive_len = lua_tointeger(RL, 2);
		char derived[1024]{};

		if (derive_len > 1024)
			luaL_error(RL, "exceeded maximum size (1024)");

		try
		{
			CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA384> kdf;
			kdf.DeriveKey(reinterpret_cast<unsigned char*>(&derived[0]), derive_len, 0, reinterpret_cast<const unsigned char*>(secret), secret_len, NULL, 0, 10000);

			lua_pushlstring(RL, derived, derive_len);
		}
		catch (CryptoPP::Exception& e)
		{
			luaL_error(RL, e.what());
		}

		return 1;
	}

	std::int32_t random(misako_State* RL)
	{
		luaL_checktype(RL, 1, LUA_TNUMBER);

		unsigned int len = lua_tointeger(RL, 1);
		char output[1024]{};

		if (len > 1024)
			luaL_error(RL, "exceeded maximum size (1024)");

		try
		{
			CryptoPP::AutoSeededRandomPool srp;
			srp.GenerateBlock(reinterpret_cast<unsigned char*>(&output[0]), len);

			lua_pushlstring(RL, output, len);
		}
		catch (CryptoPP::Exception& e)
		{
			luaL_error(RL, e.what());
		}

		return 1;
	}

	namespace custom 
	{
		std::int32_t encrypt(misako_State* RL)
		{
			luaL_checktype(RL, 1, LUA_TSTRING);
			luaL_checktype(RL, 2, LUA_TSTRING);
			luaL_checktype(RL, 3, LUA_TSTRING);
			luaL_checktype(RL, 4, LUA_TSTRING);

			const char* algo = lua_tostring(RL, 1);

			std::size_t data_len;
			const char* data = lua_tolstring(RL, 2, &data_len);

			std::size_t key_len;
			const char* key = lua_tolstring(RL, 3, &key_len);

			std::size_t iv_len;
			const char* iv = lua_tolstring(RL, 4, &iv_len);

			std::string output;

			try
			{
				output = helper::run_algo(RL, algo, "enc", data, data_len, key, key_len, iv, iv_len);
			}
			catch (CryptoPP::Exception& e)
			{
				luaL_error(RL, e.what());
			}

			lua_pushlstring(RL, output.c_str(), output.size());
			return 1;
		}

		std::int32_t decrypt(misako_State* RL)
		{
			luaL_checktype(RL, 1, LUA_TSTRING);
			luaL_checktype(RL, 2, LUA_TSTRING);
			luaL_checktype(RL, 3, LUA_TSTRING);
			luaL_checktype(RL, 4, LUA_TSTRING);

			const char* algo = lua_tostring(RL, 1);

			std::size_t data_len;
			const char* data = lua_tolstring(RL, 2, &data_len);

			std::size_t key_len;
			const char* key = lua_tolstring(RL, 3, &key_len);

			std::size_t iv_len;
			const char* iv = lua_tolstring(RL, 4, &iv_len);

			std::string output;

			try
			{ 
				output = helper::run_algo(RL, algo, "dec", data, data_len, key, key_len, iv, iv_len); 
			}
			catch (CryptoPP::Exception& e)
			{
				luaL_error(RL, e.what()); 
			}

			lua_pushlstring(RL, output.c_str(), output.size());

			return 1;
		}

		std::int32_t hash(misako_State* RL)
		{
			luaL_checktype(RL, 1, LUA_TSTRING);
			luaL_checktype(RL, 2, LUA_TSTRING);

			const char* algo = lua_tostring(RL, 1);

			std::size_t data_len;
			const char* data = lua_tolstring(RL, 2, &data_len);

			std::string output;

			try
			{
				output = helper::run_hash(RL, algo, data, data_len);
			}
			catch (CryptoPP::Exception& e)
			{
				luaL_error(RL, e.what());
			}

			lua_pushlstring(RL, output.c_str(), output.size());
			return 1;
		}
	}

	void setup_crypt_environment(misako_State* state)
	{
		lua_newtable(state);
		{
			misako_register_table(state, crypt_environment::encrypt, "encrypt");
			misako_register_table(state, crypt_environment::decrypt, "decrypt");

			misako_register_table(state, crypt_environment::base64::encode, "base64encode");
			misako_register_table(state, crypt_environment::base64::decode, "base64decode");

			misako_register_table(state, crypt_environment::base64::encode, "base64_encode");
			misako_register_table(state, crypt_environment::base64::decode, "base64_decode");

			lua_newtable(state);
			{
				misako_register_table(state, crypt_environment::base64::encode, "encode");
				misako_register_table(state, crypt_environment::base64::decode, "decode");
			}
			lua_setfield(state, -2, "base64");

			misako_register_table(state, crypt_environment::hash, "hash");
			misako_register_table(state, crypt_environment::derive, "derive");
			misako_register_table(state, crypt_environment::random, "random");

			lua_newtable(state);
			{
				misako_register_table(state, crypt_environment::custom::encrypt, "encrypt");
				misako_register_table(state, crypt_environment::custom::decrypt, "decrypt");
				misako_register_table(state, crypt_environment::custom::hash, "hash");
			}
			lua_setfield(state, -2, "custom");
		}
		lua_setglobal(state, "crypt");

		/* above but without crypt. table, script developers can be retarded sometimes */
		lua_newtable(state);
		{
			misako_register_table(state, crypt_environment::base64::encode, "encode");
			misako_register_table(state, crypt_environment::base64::decode, "decode");
		}
		lua_setglobal(state, "base64");

		/* the one above but without base64. table, script developers can be retarded sometimes */
		misako_register(state, crypt_environment::base64::encode, "base64_encode");
		misako_register(state, crypt_environment::base64::decode, "base64_decode");
	}
}