/*******************************************************************************
* symbiote "lightweight security protocol" Library
* Version: 0.1
* Date: 12/01/2018
* Author: Unidata
* Company: Unidata
*
* Library to handle security negotiation protocol for agent
*
*
* Revision  Description
* ========  ===========
*
* 0.1      Initial release
*******************************************************************************/
#include <iostream>

#include "lsp.h"

/*
lsp::lsp(char* cp, char* kdf, uint8_t* psk, uint8_t psk_len) {
	_kdf = std::string(kdf);
	_cp = std::string(cp);
	_psk = (uint8_t*)malloc(psk_len);
	memcpy(_psk, psk, psk_len);
	_psk_len = psk_len;
	_SDEVNonce = 0;
	_GWNonce = 0;
	_sn = 0;
	_needInitVector = false;
	_iv = "";
	_sessionId = "";
	_lastSSPId = "";
	memset(_dk1, 0, sizeof(_dk1));
	memset(_prevDk1, 0, sizeof(_prevDk1));
	memset(_dk2, 0, sizeof(_dk2));
	memset(_SDEVmac, 0, sizeof(_SDEVmac));
}*/
        
uint8_t ppsk[HMAC_DIGEST_SIZE] = {0x46, 0x72, 0x31, 0x73, 0x80, 0x52, 0x78, 0x92, 0x52, 0x81, 0xad, 0xd7, 0x57, 0x2c, 0x04, 0xa5, 0xdd, 0x84, 0x16, 0x68};

extern "C" void PBKDF2function( uint8_t *pass, uint32_t pass_len, uint8_t *salt, uint32_t salt_len,uint8_t *output, uint32_t key_len, uint32_t rounds )

{

	register int ret,j;
	register uint32_t i;
	register uint8_t md1[HMAC_DIGEST_SIZE],work[HMAC_DIGEST_SIZE];
	register size_t use_len;
	register uint8_t *out_p = output;
	register uint8_t counter[4];

	for ( i = 0 ; i < sizeof ( counter ) ; i++ )

		counter[i] = 0;

	counter[3] = 1;

	while (key_len)

	{

		sha1.initHmac(pass,pass_len);
		sha1.write(salt,salt_len);
		sha1.write(counter,4);
		hmac = sha1.resultHmac();

		for ( i = 0 ; i < HMAC_DIGEST_SIZE ; i++ )

			work[i] = md1[i] = hmac[i];

		for ( i = 1 ; i < rounds ; i++ )

		{

			sha1.initHmac(pass,pass_len);
			sha1.write(md1,HMAC_DIGEST_SIZE);
			hmac = sha1.resultHmac();

			for ( j = 0 ; j < HMAC_DIGEST_SIZE ; j++ )
			{
				md1[j] = hmac[j];
				work[j] ^= md1[j];
			}
		}

		use_len = (key_len < HMAC_DIGEST_SIZE ) ? key_len : HMAC_DIGEST_SIZE;

		for ( i = 0 ; i < use_len ; i++ )
			out_p[i] = work[i];

		key_len -= use_len;
		out_p += use_len;

		for ( i = 4 ; i > 0 ; i-- )

			if ( ++counter[i-1] != 0 )

				break;
	}

}

void printBuffer(uint8_t* buff, uint8_t len, std::string label) {
	//std::cout << label;
	//std::cout <<  "\t= {";
        printf("%s \t = {", label.c_str());
	for (uint8_t j = 0; j < len - 1; j++) {
		printf("%2x, ", buff[j]);
		//std::cout <<  ", ";
	}
	printf("%2x }\n", buff[len - 1]);
	//std::cout <<  "}" std::endl;
}


extern "C" void calculateDK1(uint8_t num_iterations) {
	P("\n\nStart calculating DK1 key....");
	// search for a valid key in flash, if not found _dk1 and _prevDk1 are not populated,
	// otherwise get _dk1 and
	if (_kdf == "PBKDF2") {
		if (_cp == TLS_PSK_WITH_AES_128_CBC_SHA) {
			printBuffer(_psk, _psk_len, "PSK");
			uint8_t salt[8];
			memset(salt, 0, sizeof(salt));
			//uint32_t tmpnonce = ENDIAN_SWAP_32(0x98ec4);
			uint32_t tmpnonce = ENDIAN_SWAP_32(_SDEVNonce);
			memcpy(salt, (uint8_t*)&tmpnonce, 4);
			tmpnonce = ENDIAN_SWAP_32(_GWNonce);
			memcpy(salt+4, (uint8_t*)&tmpnonce, 4);

			printBuffer(salt, 8, "DK1salt");

			PBKDF2function( _psk, _psk_len, salt, sizeof(salt) ,_dk1, sizeof(_dk1), num_iterations );
#ifdef DEBUG_SYM_CLASS
			printBuffer(_dk1, AES_KEY_LENGTH, "DK1");
#endif
		} else {
			//TBD
			P("DK1: CRYPTO SUITE NOT IMPLEMENTED ");
		}
	} else {
		//TBD
		P("HKDF already not implemented!");
	}

}

extern "C" void calculateDK2(uint8_t num_iterations) {
	P("\nStart calculating DK2 key....");
	if (_kdf == "PBKDF2") {
		if (_cp == TLS_PSK_WITH_AES_128_CBC_SHA) {
			uint8_t dk2Password[8+(_psk_len/2)];

			uint8_t salt[8];
			memset(salt, 0, sizeof(salt));
			memset(dk2Password, 0, sizeof(dk2Password));

			// the new password is: firstpart(PSK/2)||SDEVnonce||GW_INKnonce
			memcpy(dk2Password, (uint8_t*)_psk, (_psk_len/2));

			uint32_t tmpnonce = ENDIAN_SWAP_32(_SDEVNonce);
			memcpy(dk2Password+10, (uint8_t*)&tmpnonce, 4);
			memcpy(salt, (uint8_t*)&tmpnonce, 4);

			tmpnonce = ENDIAN_SWAP_32(_GWNonce);
			memcpy(dk2Password+14, (uint8_t*)&tmpnonce, 4);
			memcpy(salt+4, (uint8_t*)&tmpnonce, 4);

			printBuffer(dk2Password, sizeof(dk2Password), "DK2password");
			printBuffer(salt, sizeof(salt), "DK2salt");

			PBKDF2function( dk2Password, sizeof(dk2Password), salt, sizeof(salt), _dk2, sizeof(_dk2), num_iterations );
#ifdef DEBUG_SYM_CLASS
			printBuffer(_dk2, AES_KEY_LENGTH, "DK2");
#endif
		} else {
			//TBD
			P("DK2: CRYPTO SUITE NOT IMPLEMENTED ");
		}

	} else {
		//TBD
		P("HKDF already not implemented!");
	}

}


//extern "C" uint8_t elaborateInnkResp(std::string& resp) {
extern "C" uint8_t elaborateInnkResp(char* resp) {
	_jsonBuff.clear();
	JsonObject& _root = _jsonBuff.parseObject(resp);
	if (!_root.success()) {
		P("parseObject() failed");
		return JSON_PARSE_ERR;
	}
	P("\nGOT this response from INNK:");
        std::string tmpjsonString = "";
	_root.prettyPrintTo(tmpjsonString);
        P(tmpjsonString.c_str());
	std::string mti = _root["mti"];
        //std::string mti = _root["mti"].as<String>();
	if (mti == STRING_MTI_GW_INK_HELLO) {
		// GWInnkeeperHello code, everything ok
		// get the crypto choice
		/*
		This is a GW_INKK_HELLO RESPONSE:
			{
				"mti": "0x20",
				"cc": "0x00a8",
				"iv": "<16_characters>",
				"nonce": "<GWnonce>",
				"sessionId": <abCD123a>
			}
		*/
		P("\nGOT MTI GW INK HELLO");
		std::string _cc = _root["cc"];
                //String _cc = _root["cc"].as<String>();
		if (_cc != _cp) {
			P("Crypto Choice different from Crypto Proposal, process degraded, I continue usign CP");
		}
                std::string ivTmp = _root["iv"];
		_iv = ivTmp;
                //++_iv = _root["iv"].as<String>();
                std::string tmpConvString = _root["nonce"];
		//++String tmpConvString = _root["nonce"].as<String>();
		PI("DEBUG: GWnonce(STRING)=");
		printf("%s\n", tmpConvString.c_str());
		_GWNonce = HEX2Int(tmpConvString);
		PI("DEBUG: GWnonce=");
		printf("%x\n", _GWNonce);
		PI("DEBUG: iv=");
		P("%s\n", _iv.c_str());
		//_GWNonce = _root["nonce"].as<String>().toInt();
		if (_iv != "") {
			// we need to use the init vector for the key calculation
			P("Init vector found");
			_needInitVector == true;
		} else {
			P("Init vector not found");
			_needInitVector == false;
		}
                std::string sessionIdTmp = _root["sessionId"];
		_sessionId = sessionIdTmp;
                //++_sessionId = _root["sessionId"].as<String>();
		return COMMUNICATION_OK;
	} else if (mti == STRING_MTI_GW_INK_AUTHN) {
		P("\nGOT MTI GW INK AUTHN");
/*
		This is a GW INK AUTHN RESPONSE:
			{
				"mti": "0x40",
				"sn": <HEX(SDEVsn+1)>,
				"nonce": "<GWnonce2>",
				"sessionId": <abCD123a>,
				"authn": "<b64(ENC_dk1( ( SHA-1(HEX_STRING(SDEVnonce2)||HEX_STRING(GWnonce2))  || HEX_STRING(sn) )>",
				"sign": "<b64(SHA-1-HMAC_dk2([ENC_dk1(SHA-1(SDEVnonce2||GWnonce2))]))>"
			}
*/
		std::string sn = _root["sn"];
                //++String sn = _root["sn"].as<String>();
		P("SEQUENCE NUMBER GOT(String):");
		P("%s", sn.c_str());
		PI("SEQUENCE NUMBER GOT(INT):");
		printf("%x\n", HEX2Int(sn));
		if (HEX2Int(sn) == (_sn+1)) {
			// everything ok
			// Increment the _sn index of 1 unit to match with the
			// _sn value used by the innkeeeper
			_sn = _sn+1;
			std::string sessionId = _root["sessionId"];
                        //++String sessionId = _root["sessionId"].as<String>();
			if (sessionId != _sessionId) {
				// Wrong session id
				P("ERR: wrong session ID");
				return COMMUNICATION_ERROR;
			}
			//String gwnonce = _root["nonce"].as<String>();
			// save the new GWnonce
			//_GWNonce = gwnonce.toInt();
			std::string tmpConvString = _root["nonce"];
                        //++String tmpConvString = _root["nonce"].as<String>();
			PI("DEBUG: GWnonce(STRING)=");
			P("%s\n", tmpConvString.c_str());
			_GWNonce = HEX2Int(tmpConvString);
			PI("DEBUG: GWnonce=");
			printf("%x", _GWNonce);
                        std::string authn = _root["authn"];
			std::string sign = _root["sign"];
			//++String authn = _root["authn"].as<String>();
			//++String sign = _root["sign"].as<String>();
			std::string decrypted;
			if (decryptAndVerify(authn, decrypted, sign)) {
				P("decryptAndVerify OK");
			} else {
				P("decryptAndVerify KO");
			}
			_sn = _sn+1;
			return COMMUNICATION_OK;
		} else {
			P("ERR: sequence wrong sequence number");
			return COMMUNICATION_ERROR;
		}
	} else {
		P("ERR: wrong mti code from INNK");
		return COMMUNICATION_ERROR;
	}
}

uint32_t HEX2Int(std::string in) {
	uint32_t ret = 0;
	for (uint8_t i = 1; i <= in.length(); i++) {
		switch (in.at(i-1)){
			case '0':
				break;
			case '1':
				ret += 1<<((in.length()-i)*4);
				break;
			case '2':
				ret += 2<<((in.length()-i)*4);
				break;
			case '3':
				ret += 3<<((in.length()-i)*4);
				break;
			case '4':
				ret += 4<<((in.length()-i)*4);
				break;
			case '5':
				ret += 5<<((in.length()-i)*4);
				break;
			case '6':
				ret += 6<<((in.length()-i)*4);
				break;
			case '7':
				ret += 7<<((in.length()-i)*4);
				break;
			case '8':
				ret += 8<<((in.length()-i)*4);
				break;
			case '9':
				ret += 9<<((in.length()-i)*4);
				break;
			case 'a':
				ret += 10<<((in.length()-i)*4);
				break;
			case 'b':
				ret += 11<<((in.length()-i)*4);
				break;
			case 'c':
				ret += 12<<((in.length()-i)*4);
				break;
			case 'd':
				ret += 13<<((in.length()-i)*4);
				break;
			case 'e':
				ret += 14<<((in.length()-i)*4);
				break;
			case 'f':
				ret += 15<<((in.length()-i)*4);
				break;
			default:
				P("ERROR HEX2INT");
				break;
		}


	}
	return ret;
}

//extern "C" void begin(std::string SSPId) {
extern "C" void begin(char* SSPId) {
        // init rand generator
        _kdf = std::string("PBKDF2");
	_cp = std::string(TLS_PSK_WITH_AES_128_CBC_SHA);
	_psk = (uint8_t*)malloc(HMAC_DIGEST_SIZE);
	memcpy(_psk, ppsk, HMAC_DIGEST_SIZE);
	_psk_len = HMAC_DIGEST_SIZE;
	_SDEVNonce = 0;
	_GWNonce = 0;
	_sn = 0;
	_needInitVector = false;
	_iv = "";
	_sessionId = "";
	_lastSSPId = "";
        memset(_dk1, 0, sizeof(_dk1));
	memset(_prevDk1, 0, sizeof(_prevDk1));
	memset(_dk2, 0, sizeof(_dk2));
	memset(_SDEVmac, 0, sizeof(_SDEVmac));
        srand (time(NULL));
	_currentSSPId = SSPId;
	// if first time ever connected to SSP do nothing
	// otherwise retrive:
	// - _prevDk1
	// - _lastSSPId
	getContext();
}

extern "C" void getContext() {
	P("GETCONTEXTFROMFLASH");
	std::string tmpString = "";
	char c;
	// Stream class to read from files
        
        const char *homedir;
        if ((homedir = getenv("HOME")) == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }
        printf("Get this home directory: %s\n", homedir);
        
	std::ifstream contextFile;
	//contextFile.open ("context.txt");
        std::string contextFilePath = homedir;
        contextFilePath += "/.sdev-agent/";
        printf("Get this path: %s\n", contextFilePath.c_str());
        
        std::string contextFileName = contextFilePath;
        contextFileName += "context.txt";
        printf("Search this file: %s\n", contextFileName.c_str());
        
	contextFile.open (contextFileName.c_str());
	if (contextFile.is_open()) {
		// file exist, so a context is been created
		while (!contextFile.eof()) {
			contextFile.get(c);
			tmpString += c;
		}
                P("Read this: %s\n", tmpString.c_str());
	}
	else {
            P("No context file found, create file");
            contextFile.close();
            saveContext("");
        } 
            
	_jsonBuff.clear();
	JsonObject& _root = _jsonBuff.parseObject(tmpString.c_str());
	if (!_root.success()) {
		P("parseObject() failed");
	} else {
            tmpString = "";
            _root.prettyPrintTo(tmpString);
            //char _prevDk1[12];
            memset(_prevDk1, 0, sizeof(_prevDk1));
            std::string tmpPrevDk1String = _root["prev_dk1"];
            _lastSSPId = _root["sspWiFiId"].as<std::string>();
            _symId = _root["symId"].as<std::string>();
            PI("sspWiFiId = ");
            P(_lastSSPId.c_str());
            PI("symId = ");
            P(_symId.c_str());
            if (tmpPrevDk1String != "") {
                for (uint8_t i = 0; i < sizeof(_prevDk1); i++) {
                        if (i < tmpPrevDk1String.length()) _prevDk1[i] = tmpPrevDk1String.at(i);
                }
                for (uint8_t i = 0; i < sizeof(_prevDk1); i++) std::cout << _prevDk1[i] << std::endl;
                std::cout << "\n" << std::endl;
                for (uint8_t i = 0; i < sizeof(_prevDk1); i++) printf("%x,", _prevDk1[i]);
                std::cout << "\n" << std::endl;
                std::cout << tmpString << std::endl;
            } else P("DK1 empty");
        }
	contextFile.close();
	return;
}

extern "C" void saveContext(char* symId) {
	P("SAVECONTEXTINFLASH");
	std::ofstream contextFile;
        struct stat st = {0};

        const char *homedir;
        if ((homedir = getenv("HOME")) == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }
        printf("Get this home directory: %s\n", homedir);
        std::string contextFilePath = homedir;
        contextFilePath += "/.sdev-agent/";
        printf("Get this path: %s\n", contextFilePath.c_str());
        
        if (stat(contextFilePath.c_str(), &st) == -1) {
            P("No directory found, creating...");
            mkdir(contextFilePath.c_str(), 0700);
        }
        
        std::string contextFileName = contextFilePath;
        contextFileName += "context.txt";
        printf("Search this file: %s\n", contextFileName.c_str());
        
	contextFile.open (contextFileName.c_str());
	std::string jsonData = "";
	//string _currentSSPId = "sym-1234567890abcdef";
	//char _dk1[11] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x70, 0x70, 0x10};
	_jsonBuff.clear();
	JsonObject& _root = _jsonBuff.createObject();
	_root["sspWiFiId"] = _currentSSPId.c_str();
	_root["prev_dk1"] = _dk1;
        _root["symId"] = symId;
        _symId = symId;
	_root.printTo(jsonData);
	if (contextFile.is_open()) {
		// file exist, so a context is been created
		contextFile << jsonData;
	}
	else P("Unable to open file(2)");
	contextFile.close();
}


/*
	Return a SHA1(symbiote-id||prevDK1). IN/OUT data should be intended as ascii hex rapresentation
*/
extern "C" const char* getHashOfIdentity() {
	// return all zeros if first time connect to a SSP
        P("GETHASHOFIDENTITY");
	if (_lastSSPId == "") return "00000000000000000000";
	else {
                P("-");
		std::string tmpString = _symId;
                P("-");
                char tmpDk[(2*AES_KEY_LENGTH) + 1];
                P("-");
                memset (tmpDk, 0, sizeof(tmpDk));
                P("-");
                sprintf(tmpDk, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", _dk1[0], _dk1[1], _dk1[2], _dk1[3], _dk1[4], _dk1[5], _dk1[6], _dk1[7], _dk1[8], 
                _dk1[9], _dk1[10], _dk1[11], _dk1[12],_dk1[13], _dk1[14], _dk1[15]);
                P("-");
                std::string DkString = tmpString;
                for (uint8_t i = 0; i < (2*AES_KEY_LENGTH); i++) DkString += tmpDk[i];
                P("-");
                PI("Got this symId|DK1 string: ");
                P("%s\n", DkString.c_str());
                
		//for (uint8_t i = 0; i < AES_KEY_LENGTH; i++) tmpString = std::string(_prevDk1[i], HEX);
		sha1.init();
		sha1.print(DkString);
                
		uint8_t dataout[SHA1_KEY_SIZE];
		memcpy(dataout, sha1.result(), SHA1_KEY_SIZE);
                
                char tmpResult[(2*SHA1_KEY_SIZE) + 1];
                memset (tmpResult, 0, sizeof(tmpResult));
                //for (uint8_t i = 0; i < SHA1_KEY_SIZE; i++) sprintf(tmpResult, "%x", dataout[i]);
                sprintf(tmpResult, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", dataout[0], dataout[1], dataout[2], dataout[3], dataout[4], dataout[5], dataout[6], dataout[7], dataout[8], 
                dataout[9], dataout[10], dataout[11], dataout[12],dataout[13], dataout[14], dataout[15], dataout[16], dataout[17], dataout[18], dataout[19]);
       
                std::string resString = "";
                for (uint8_t i = 0; i < (2*SHA1_KEY_SIZE); i++) resString += tmpResult[i];
                
		//std::string retString = "";
		//for (uint8_t i = 0; i < SHA1_KEY_SIZE; i++) retString = std::string(dataout[i], HEX);
		PI("Got this SHA-1(sym-id||prevDK1): ");
		P("%s\n", resString.c_str());
		//return resString.c_str();
                
                char* retStr = (char*) malloc(resString.length()+1);
                strcpy(retStr, resString.c_str());
                return retStr;
	}
}

extern "C" char* sendSDEVHelloToGW(char* mac) {
	P("Enter sendSDEVHelloToGW\n");
	
	_jsonBuff.clear();
	JsonObject& _root = _jsonBuff.createObject();
	std::string mac_string;
        char c;
	//for (int j = 0; j < 6; j++) {
	//	if (_SDEVmac[j] < 16) {
	///		mac_string += "0";
	//	}
	//	mac_string += String(_SDEVmac[j], HEX);
	//	if (j!=5) mac_string += ":";
	//}
        //std::ifstream macAddrFile;
        //macAddrFile.open ("/sys/class/net/wlan0/address");
        //if (macAddrFile.is_open()) {
            // file exist, so a context is been created
        //    while (!macAddrFile.eof()) {
         //       macAddrFile.get(c);
         //       if (c != '\n') mac_string += c;
        //        }
        //    P("Read this from addresses: %s", mac_string.c_str());
         //   }
        //else P("Unable to open file");
            // FIXME, decomment
        //P("a");
	_SDEVNonce = rand() % 0xFFFFFFFF;
	//std::string nonce = std::string(_SDEVNonce, HEX);
        char tmpNonce[9];
        memset (tmpNonce, 0, sizeof(tmpNonce));
        sprintf(tmpNonce, "%x", _SDEVNonce);
        //P("a");
	std::string nonce = std::string(tmpNonce);
       // P("a");
	_root["mti"] = STRING_MTI_SDEV_HELLO;
       // P("b");
	//_root["SDEVmac"] = mac_string.c_str();
	_root["SDEVmac"] = mac;
        //P("a");
	_root["cp"] = _cp.c_str();
       // P("c");
	_root["kdf"] = _kdf.c_str();
       // P("a");
	_root["nonce"] = nonce.c_str();
       // P("d");
	//std::string temp = "";
        //P("e");
	P("Return this JSON:");
        std::string resp;
	_root.printTo(resp);
        printf("%s\n", resp.c_str());
        char* retStr = (char*) malloc(resp.length()+1);
        strcpy(retStr, resp.c_str());
        //printf("fatto\n");
	// add this to respect the HTTP RFC
	//temp = "\r\n" + temp;
        return retStr;
}

extern "C" void createAuthNPacket(uint8_t* dataout) {
        char tmpGWNonce[9];
        memset (tmpGWNonce, 0, sizeof(tmpGWNonce));
        sprintf(tmpGWNonce, "%x", _GWNonce);
	std::string gwNonceString = std::string(tmpGWNonce);
    
    
	//std::string gwNonceString = std::string(_GWNonce, HEX);
	while (gwNonceString.length() < 8) {
		// we need to add '0'
		gwNonceString = '0' + gwNonceString;

	}
        
        char tmpSDEVNonce[9];
        memset (tmpSDEVNonce, 0, sizeof(tmpSDEVNonce));
        sprintf(tmpSDEVNonce, "%x", _SDEVNonce);
	std::string SDEVNonceString = std::string(tmpSDEVNonce);
        
	//std::string SDEVNonceString = std::string(_SDEVNonce, HEX);
	while (SDEVNonceString.length() < 8) {
		// we need to add '0'
		SDEVNonceString = '0' + SDEVNonceString;

	}
	std::string dataToHash = SDEVNonceString + gwNonceString;
	PI("\n**********\nSHA1(");
	printf("%s", dataToHash.c_str());
	PI(")");
	sha1.init();
	sha1.print(dataToHash);
	memcpy(dataout, sha1.result(), 20);
}

//extern "C" std::string sendAuthN() {
extern "C" char* sendAuthN() {
	/* in this message SDEv should send (if TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA):
	{
		"mti": "0x30",
		"sn": "<HEX(sequence_number)>",
		"nonce": "<SDEVnonce2>",
		"sessionId": "abcdefgh",
		"authn": "<			      b64 ( ENC_dk1 ( SHA-1(HEX_STRING(SDEVnonce)||HEX_STRING(GWnonce))  || HEX_STRING(sn) ) )  >"
		"sign":  "<b64( SHA-1-HMAC_dk2( ENC_dk1 ( SHA-1(HEX_STRING(SDEVnonce)||HEX_STRING(GWnonce))  || HEX_STRING(sn) ) ) )>"
	}
	 */
	//"authn": <			     b64 ( ENC_dk1 ( SHA-1(HEX_STRING(SDEVnonce)||HEX_STRING(GWnonce))  || HEX_STRING(sn) ) )  >
	//"sign":  <b64( SHA-1-HMAC_dk2( ENC_dk1 ( SHA-1(HEX_STRING(SDEVnonce)||HEX_STRING(GWnonce))  || HEX_STRING(sn) ) ) )>

	P("\nEnter sendAuthN\n*********************\n");
	std::string resp = "";
	_jsonBuff.clear();
	JsonObject& _root = _jsonBuff.createObject();
	//initialize the sequence number
	_sn = rand() % 1000000;
	// use DK1 to encrypt
	std::string outdata;
	std::string signedData;
	uint8_t authNPacket[SHA1_KEY_SIZE];
	memset(authNPacket, 0, SHA1_KEY_SIZE);
	// crypt and sign data with the old SDEVnonce
	// authNPacket should be SHA1(SDEVnonce||GWnonce)
	createAuthNPacket(authNPacket);
        //P("ok");
	printBuffer(authNPacket, 20, "");
        //P("ok2");
	encryptDataAndSign((char*)authNPacket, outdata, signedData);
        //P("ok3");
	//PI("B64 encoded data(outside): ");
	//P(outdata);
	// create the new SDEVnonce
	_SDEVNonce = rand() % 0xFFFFFFFF;
        
        char tmpNonce[9];
        memset (tmpNonce, 0, sizeof(tmpNonce));
        sprintf(tmpNonce, "%x", _SDEVNonce);
	std::string nonce = std::string(tmpNonce);
        
        char tmpSn[9];
        memset (tmpSn, 0, sizeof(tmpSn));
        sprintf(tmpSn, "%x", _sn);
	std::string sn = std::string(tmpSn);
        
	_root["mti"] = STRING_MTI_SDEV_AUTHN;
	//_root["sn"] = std::string(_sn, HEX);
	_root["sn"] = sn;
	//_root["nonce"] = std::string(_SDEVNonce, HEX);
	_root["nonce"] = nonce;
	_root["sessionId"] = _sessionId;
	_root["authn"] = outdata;
	// use DK2 to sign
	_root["sign"] = signedData;
	std::string temp = "";
	P("\n*********************\nSend this JSON:");
        
	_root.printTo(resp);
        printf("%s\n", resp.c_str());
        
        char* retStr = (char*) malloc(resp.length()+1);
        strcpy(retStr, resp.c_str());
        return retStr;
}


extern "C" std::string getSessionId() {
	return _sessionId;
}

extern "C" char* getDK1() {
        P("GET DK1");
        //char tmpDk[AES_KEY_LENGTH];
        char* tmpDk = (char*) malloc((2*AES_KEY_LENGTH) + 1);
        memset (tmpDk, 0, ((2*AES_KEY_LENGTH) + 1));
        sprintf(tmpDk, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x", _dk1[0], _dk1[1], _dk1[2], _dk1[3], _dk1[4], _dk1[5], _dk1[6], _dk1[7], _dk1[8], 
                _dk1[9], _dk1[10], _dk1[11], _dk1[12],_dk1[13], _dk1[14], _dk1[15]);
        
        PI("tmpDk: ");
        P("%s", tmpDk);
        
        return tmpDk;

}

void bufferSize(char* text, int &length) {
	int i = strlen(text);
	int buf = round(i / BLOCK_SIZE) * BLOCK_SIZE;
	length = (buf < i) ? buf + BLOCK_SIZE : length = buf;
}

void bufferSize(char* text, int text_len, int &length) {
	int i = text_len;
	int buf = round(i / BLOCK_SIZE) * BLOCK_SIZE;
	length = (buf < i) ? buf + BLOCK_SIZE : length = buf;
}

void bufferSize(unsigned char* text, int &length) {
	int i = strlen((const char*)text);
	int buf = round(i / BLOCK_SIZE) * BLOCK_SIZE;
	length = (buf < i) ? buf + BLOCK_SIZE : length = buf;
}

extern "C" void signData(uint8_t* data, uint8_t data_len, std::string& output) {

	uint8_t dataToSign[data_len+4];
	memcpy(dataToSign, data, data_len);
	uint32_t tmpSn = ENDIAN_SWAP_32(_sn);
	memcpy(dataToSign+data_len, (uint8_t*)&tmpSn, 4);

	printBuffer(dataToSign, data_len+4, "DataToSign");

	sha1.initHmac(_dk2, DK2_KEY_LENGTH);
	sha1.write(dataToSign, (data_len+4));

	uint8_t* ret_data = sha1.resultHmac();

	printBuffer(ret_data, SHA1_KEY_SIZE, "BinarySign");
	base64 b64enc;
	std::string encoded = b64enc.encode(ret_data, SHA1_KEY_SIZE, false);
	output = encoded;
}

void encryptAndSign(char* plain_text, std::string& output, int length, std::string& signature) {

	byte enciphered[length];
	uint8_t iv[16];
	for (uint8_t k = 0; k < 16; k++) iv[k] = _iv.at(k);
	printBuffer(iv, 16, "IV\t");
	AES aesEncryptor(_dk1, iv, AES::AES_MODE_128, AES::CIPHER_ENCRYPT);
	//aesEncryptor.process((uint8_t*)plain_text, enciphered, length);
	aesEncryptor.processNoPad((uint8_t*)plain_text, enciphered, length);
	int encrypted_size = sizeof(enciphered);
	printBuffer((uint8_t*)enciphered, encrypted_size, "EncrypData");

	std::string signedData;
	signData(enciphered, encrypted_size, signedData);
	signature = signedData;

	base64 b64enc;
	std::string encoded = b64enc.encode(enciphered, encrypted_size, false);
	output = encoded;
}



/*
	Encrypts with DK1 (data || <sequence_number>) and returns
	the base64 rapresentation of the encrypted data and the b64 rapresentation of the signed
	SHA1-HMAC_dk2(ENC_DATA|| <sequence_number>)
*/
extern "C" void encryptDataAndSign(char* plain_text, std::string& output, std::string& signature) {
	int length = 0;
	unsigned int tmpLen = 0;
	// todo fixme
        char tmpSn[9];
        memset (tmpSn, 0, sizeof(tmpSn));
        sprintf(tmpSn, "%x", _sn);
	std::string sn = std::string(tmpSn);
        
	//tmpLen = SHA1_KEY_SIZE + std::string(_sn, HEX).length();
	tmpLen = SHA1_KEY_SIZE + sn.length();
	//String dataToEncrypt = String(plain_text) + String(_sn, HEX);
	PI("ADD this SN to encrypt:\t= ");
	P("%x\n", _sn);
        char tmpChar;
        
	uint8_t arrayOfDataToEncrypt[tmpLen];
	memset(arrayOfDataToEncrypt, 0, tmpLen);
	//dataToEncrypt.getBytes((byte*)arrayOfDataToEncrypt, (unsigned int)tmpLen);
	memcpy(arrayOfDataToEncrypt, plain_text, SHA1_KEY_SIZE);
	//for (uint8_t i = SHA1_KEY_SIZE; i < tmpLen; i++) arrayOfDataToEncrypt[i] = std::string(_sn, HEX).at(i-SHA1_KEY_SIZE);
	for (uint8_t i = SHA1_KEY_SIZE; i < tmpLen; i++) arrayOfDataToEncrypt[i] = sn.at(i-SHA1_KEY_SIZE);

	printBuffer((uint8_t*)arrayOfDataToEncrypt, tmpLen,"Data2Encrypt(array)");
	bufferSize((char*)arrayOfDataToEncrypt, tmpLen, length);;

	std::string encrypted;
	std::string tmpSign;
	uint8_t arrayOfDataToEncrypt_padded[length];
	memcpy(arrayOfDataToEncrypt_padded, arrayOfDataToEncrypt, tmpLen);
	if (tmpLen < length) {
		// we need to pad the data
		for (uint8_t i = tmpLen; i < length; i++) arrayOfDataToEncrypt_padded[i] = 0x55;
	}

	/// add 0x55 as pad if needed
	//for (uint8_t i = dataToEncrypt.length(); i < length; i++) dataToEncrypt.concat('U');
	//for (uint8_t i = tmpLen; i < length; i++) dataToEncrypt.concat('U');
	///printBuffer((uint8_t*)dataToEncrypt.c_str(), dataToEncrypt.length(),"DATA2ENCRYPT");
	printBuffer(arrayOfDataToEncrypt_padded, length,"DATA2ENCRYPT(padded)");
	PI("Lenght of the data without padding:\t");
	P("%d\n", tmpLen);
	PI("Lenght of the data with padding:\t");
	P("%d\n", length);

	//encryptAndSign((char*)dataToEncrypt.c_str(), encrypted, length, tmpSign);
	encryptAndSign((char*)arrayOfDataToEncrypt_padded, encrypted, length, tmpSign);
	output = encrypted;
	signature = tmpSign;
}

/* TMP
		This is a GW INK AUTHN RESPONSE:
			{
				"mti": "0x40",
				"sn": <HEX(SDEVsn+1)>,
				"nonce": "<GWnonce2>",
				"sessionId": <abCD123a>,
				"authn": "<b64(ENC_dk1(SHA-1(SDEVnonce2||GWnonce2)))>",
				"sign": "<b64(SHA-1-HMAC_dk2([ENC_dk1(SHA-1(SDEVnonce2||GWnonce2))]))>"
			}
*/
extern "C" bool decryptAndVerify(std::string authn, std::string& decrypted, std::string GWsigned) {
	// Please note that you should invoke this method after SDEV and GW nonce are updated with
	// SDEVnonce2 and GWnonce2

	// calculate the new ENC_dk1(SHA-1(SDEVnonce2||GWnonce2))
	std::string GWoutdata;
	std::string signedData;
	uint8_t GWauthNPacket[SHA1_KEY_SIZE];
	memset(GWauthNPacket, 0, SHA1_KEY_SIZE);
	// it creates the packet using the new SDEV and GW nonce savend in library
	P(" ");
	createAuthNPacket(GWauthNPacket);
	printBuffer(GWauthNPacket, 20, " = CalculatedGWauthNPacket");
	encryptDataAndSign((char*)GWauthNPacket, GWoutdata, signedData);
	PI("Got this sign from INNK:\t");
	P("%s", GWsigned.c_str());
	PI("Calculated sign:\t\t\t");
	P("%s", signedData.c_str());
	// FIXME: uncomment
	if (signedData == GWsigned) {
		unsigned int binaryLength = decode_base64_length((unsigned char*)authn.c_str());
		unsigned char decodedb64[binaryLength];
		memset(decodedb64, 0, binaryLength);
		P("Sign match found!");
		decode_base64((unsigned char*)authn.c_str(), decodedb64);
		printBuffer(decodedb64, binaryLength, "AUTHN(binary)");
		std::string plainHex;
		decrypt(decodedb64, binaryLength, plainHex);
		PI("PlainHex decrypted: ");
		P("%s\n", plainHex.c_str());
		return true;
	}
}

/*
	Convert plain text to b64 of encrypted data
	NO use of sequence number
*/
/*extern "C" void cryptData(std::string in, std::string& out) {
	P("CRYPTDATA");
	int length = 0;
	bufferSize((char*)in.c_str(), length);
	byte enciphered[length];
	uint8_t iv[16];
	for (uint8_t k = 0; k < 16; k++) iv[k] = _iv.at(k);
	//printBuffer(iv, 16, "IV\t");

	AES aesEncryptor(_dk1, iv, AES::AES_MODE_128, AES::CIPHER_ENCRYPT);
	aesEncryptor.process((uint8_t*)in.c_str(), enciphered, length);
	int encrypted_size = sizeof(enciphered);
	//printBuffer((uint8_t*)enciphered, encrypted_size, "EncrypData");
	base64 b64enc;
	std::string encoded = b64enc.encode(enciphered, encrypted_size, false);
	out = encoded.c_str();
}*/

extern "C" char* cryptData(std::string in) {
	P("CRYPTDATA");
	int length = 0;
        //P("1");
	bufferSize((char*)in.c_str(), length);
        //P("2");
	byte enciphered[length];
        //P("3");
	uint8_t iv[16];
        //P("4");
	for (uint8_t k = 0; k < 16; k++) iv[k] = _iv.at(k);
        //P("5");
	//printBuffer(iv, 16, "IV\t");
	AES aesEncryptor(_dk1, iv, AES::AES_MODE_128, AES::CIPHER_ENCRYPT);
        //P("6");
	aesEncryptor.process((uint8_t*)in.c_str(), enciphered, length);
        //P("7");
	int encrypted_size = sizeof(enciphered);
        //P("8");
	//printBuffer((uint8_t*)enciphered, encrypted_size, "EncrypData");
	base64 b64enc;
        //P("9");
	std::string encoded = b64enc.encode(enciphered, encrypted_size, false);
	//P("10");
        //P("encoded: %s", encoded.c_str());
	//out = encoded;	
        //P("11");
        int len = encoded.length()+1;
        //P("len: %d", len);
        // freed in caller method
        char* retStr = (char*) malloc(len);
        strcpy(retStr, encoded.c_str());
        return retStr;
	//return encoded.c_str();
}


extern "C" const char* preparePacket(char* semantic) {
    P("PREPARE PACKET FOR INNK");
    //std::string tempCryptData = "";
    std::string tempJsonPacket = "";
    std::string tempClearData(semantic);
    //cryptData(tempClearData, tempCryptData);
    char* tmpCC = cryptData(tempClearData);
    std::string tempCryptData(tmpCC);
    //P("crypt: %s", tempCryptData.c_str());
    _jsonBuff.clear();
    JsonObject& jsonCrypt = _jsonBuff.createObject();
    jsonCrypt["mti"] = STRING_MTI_SDEV_DATA_UPLINK;
    jsonCrypt["sessionId"] = getSessionId();
    jsonCrypt["data"] = tempCryptData;
    jsonCrypt.printTo(tempJsonPacket);
    P("Prepare this JSON:");
    P(tempJsonPacket.c_str());
   // return tempJsonPacket.c_str();
    free(tmpCC);
    char* retStr = (char*) malloc(tempJsonPacket.length()+1);
    strcpy(retStr, tempJsonPacket.c_str());
    return retStr;

}

extern "C" const char* decryptPacketFromInnk(char* data) {
    P("DECRYPT DATA FROM INNK");
    //std::string in (data);
    std::string out = "";
    //P("Got this:");
    //P(in.c_str());
    
    _jsonBuff.clear();
    JsonObject& _root = _jsonBuff.parseObject(data);
    if (!_root.success()) {
	P("parseObject() failed");
	//return JSON_PARSE_ERR;
    }
    
    P("\nGOT this response from INNK:");
    std::string tmpjsonString = "";
    _root.prettyPrintTo(tmpjsonString);
    P(tmpjsonString.c_str());
    std::string in = _root["data"];
    
    decryptData(in, out);
    P("Send back this:");
    P(out.c_str());
    
    //return out.c_str();
    char* retStr = (char*) malloc(out.length()+1);
    strcpy(retStr, out.c_str());
    return retStr;
    
}

/*
	Convert data b64 crypted data to plain text
	NO use of sequence number
*/
void decryptData(std::string in, std::string& out) {
	P("DECRYPTDATA\nB64in:\t");
        P(in.c_str());
	unsigned int binaryLength = decode_base64_length((unsigned char*)in.c_str());
	unsigned char decodedb64[binaryLength];
	memset(decodedb64, 0, binaryLength);
	decode_base64((unsigned char*)in.c_str(), decodedb64);
	printBuffer(decodedb64, binaryLength, "B64out:");
	std::string plainHex;
	decrypt(decodedb64, binaryLength, plainHex);
	out = plainHex;
}

extern "C" void decrypt(unsigned char* crypted, uint8_t cryptedSize, std::string& output) {

	// TODO FIXME iv must be the same accorded with INNKEEPER
	int length = 0;
        //P("Ok qui");
	bufferSize((char*)crypted, cryptedSize, length);
        //P("Ok qui 2");
	byte deciphered[length];

	uint8_t iv[16];
	for (uint8_t k = 0; k < 16; k++) iv[k] = _iv.at(k);
	printBuffer(iv, 16, "IV\t");
        //P("Ok qui3");
	AES aesDencryptor(_dk1, iv, AES::AES_MODE_128, AES::CIPHER_DECRYPT);
        //P("Ok qui4");
	aesDencryptor.process((uint8_t*)crypted, deciphered, length);
        //P("Ok qui5");
	printBuffer(deciphered, length, "DECRYPT(BINARY)");
	for (uint8_t i = 0; i< length; i++) {
		output += (char)deciphered[i];
	}
        ///P("Ok qui6");
	//output = output.substring(0,output.lastIndexOf("}")+1);
        //size_t position = output.find_last_of('}', output.length());
        //PI("Size of the output string: ");
        //P("%d", (int)output.length());
        //PI("Position found: ");
        //P("%d", (int)position);
        //P("Ok qui7");
        ////TODO FIXME
	////output.resize(position);
        //P("Ok qui8");
	PI("String decrypted: ");
	P(output.c_str());

}


/* base64_to_binary:
 *   Description:
 *     Converts a single byte from a base64 character to the corresponding binary value
 *   Parameters:
 *     c - Base64 character (as ascii code)
 *   Returns:
 *     6-bit binary value
 */
unsigned char base64_to_binary(unsigned char c) {
	// Capital letters - 'A' is ascii 65 and base64 0
	if('A' <= c && c <= 'Z') return c - 'A';

	// Lowercase letters - 'a' is ascii 97 and base64 26
	if('a' <= c && c <= 'z') return c - 71;

	// Digits - '0' is ascii 48 and base64 52
	if('0' <= c && c <= '9') return c + 4;

	// '+' is ascii 43 and base64 62
	if(c == '+') return 62;

	// '/' is ascii 47 and base64 63
	if(c == '/') return 63;

	return 255;
}

unsigned int decode_base64_length(unsigned char input[]) {

	unsigned char *start = input;
	while(base64_to_binary(input[0]) < 64) {
		++input;
	}
	unsigned int input_length = input - start;
	unsigned int output_length = input_length/4*3;
	switch(input_length % 4) {

		default: return output_length;

		case 2: return output_length + 1;

		case 3: return output_length + 2;
	}
}


/* decode_base64:
 *   Description:
 *     Converts a base64 null-terminated string to an array of bytes
 *   Parameters:
 *     input - Pointer to input string
 *     output - Pointer to output array
 *   Returns:
 *     Number of bytes in the decoded binary
 */
extern "C" unsigned int decode_base64(unsigned char input[], unsigned char output[]) {

	unsigned int output_length = decode_base64_length(input);
	// While there are still full sets of 24 bits...
	for(unsigned int i = 2; i < output_length; i += 3) {

		output[0] = base64_to_binary(input[0]) << 2 | base64_to_binary(input[1]) >> 4;
		output[1] = base64_to_binary(input[1]) << 4 | base64_to_binary(input[2]) >> 2;
		output[2] = base64_to_binary(input[2]) << 6 | base64_to_binary(input[3]);

		input += 4;
		output += 3;

	}
	switch(output_length % 3) {
		case 1:
			output[0] = base64_to_binary(input[0]) << 2 | base64_to_binary(input[1]) >> 4;
			break;

		case 2:
			output[0] = base64_to_binary(input[0]) << 2 | base64_to_binary(input[1]) >> 4;
			output[1] = base64_to_binary(input[1]) << 4 | base64_to_binary(input[2]) >> 2;
			break;
	}
	return output_length;
}