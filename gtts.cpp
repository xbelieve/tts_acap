#include "./gtts.h"

namespace gtts {

const char SAFE[256] =
	{
			/*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
			/* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
			/* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
			/* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
			/* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
			
			/* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
			/* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
			/* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
			/* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
			
			/* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
			/* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
			/* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
			/* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
			
			/* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
			/* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
			/* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
			/* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
	};

	std::string URIEncode(const std::string & sSrc)
	{
		const char DEC2HEX[16 + 1] = "0123456789abcdef";
		const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
		const int SRC_LEN = sSrc.length();
		unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
		unsigned char * pEnd = pStart;
		const unsigned char * const SRC_END = pSrc + SRC_LEN;

		for (; pSrc < SRC_END; ++pSrc)
		{
			if (SAFE[*pSrc]) {
				*pEnd++ = *pSrc;
			}	else if (*pSrc == ' ' || *pSrc == '-' || *pSrc == '_' || *pSrc == '.' || *pSrc == '!' || *pSrc == '~' || *pSrc == '*' || *pSrc == '\'' || *pSrc == '(' || *pSrc == ')' || *pSrc == ':' || *pSrc == '/'|| *pSrc == '\\') {
				*pEnd++ = *pSrc;
			} else {
				// escape this char
				*pEnd++ = '%';
				*pEnd++ = DEC2HEX[*pSrc >> 4];
				*pEnd++ = DEC2HEX[*pSrc & 0x0F];
			}
		}

		std::string sResult((char *)pStart, (char *)pEnd);
		delete [] pStart;
		return sResult;
	}


GoogleTTS::GoogleTTS(string msg, string lang, string speed) {
    _speed += speed;
    _lang += lang;
    if (msg.length() > 200) {
        std::vector<string> msgs = this->split(msg);
        this->parse(msgs);
    } else {
        _text = msg;
        this->parse();
    }
}

std::vector<string> GoogleTTS::split(string& msg) {
    std::vector<string> vec;
    std::istringstream iss(msg);
    std::vector<string> words(std::istream_iterator<string>{iss},
            std::istream_iterator<string>());
    string part = "";
    for (string s : words) {
        if(part.size()-1 + s.size() <= 200) {
            part += s + " ";
        } else {
            vec.push_back(part);
            part = s + " ";
        }
    }
    if(part != "") vec.push_back(part);

    return vec;
}

void GoogleTTS::unite() {
    system(_cat.c_str());
    system(_rm.c_str());
}

void GoogleTTS::execute() {
    if (_cmds.size() == 1) {
        std::system(_cmds[0].c_str());
        LOG("%s\n", _cmds[0].c_str());
    } else {
        for(string cmd : _cmds) {
            std::system(cmd.c_str());
            LOG("%s\n", cmd.c_str());
        }
        this->unite();
    }
    _mpv += _play;
    std::system(_mpv.c_str());
    LOG("%s\n", _mpv.c_str());
}

void GoogleTTS::replace(string& text) {
    text = URIEncode(text);
    /*size_t start_pos = 0;
    while ((start_pos = text.find(" ", start_pos)) != string::npos) {
       text.replace(start_pos, 1, "%20");
       start_pos += 3; // Handles case where 'to' is a substring of 'from'
    }*/
}

void GoogleTTS::parse() {
    this->replace(_text);
    string cmd = _curl + _text + _lang + _client + "-H" + _ref + "-H";
    cmd += _agent + _out; 
    _cmds.push_back(cmd);
}

void GoogleTTS::parse(std::vector<string>& vec) {
    string cmd = "";
    int i = 0;
    for (string msg : vec) {
        this->replace(msg);
        cmd = _curl + msg + _lang + _client + "-H" + _ref + "-H";
        cmd += _agent + _outv + std::to_string(i) + ".mp3" + " 2>/dev/null"; 
        _cmds.push_back(cmd);
        i++;
    }
}

void GoogleTTS::help() {
    std::cout << "gtts: plays Google Text-to-Speech speech synthesis with " << 
        "help of Google Translate voice" << std::endl;
    std::cout << "Usage: gtts [language] \"[message]\" ([speed])" << std::endl;
    std::cout << "Example: gtts us \"hello world\" 1.5" << std::endl;
    std::cout << "standard speed is 1.0" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;    
    std::cout << "-h\t\tshows this help" << std::endl;
    std::cout << "-v\t\tshows program version" << std::endl;
    std::cout << "-l\t\tshows all available languages" << std::endl;
    std::cout << std::endl;
    std::cout << "To speak from text files use: gtts [lang] \"$(cat file.txt)\""
       " ([speed])" << std::endl;
    std::cout << std::endl;
}

void GoogleTTS::languages() {
    std::cout << "Supported languages:" << std::endl;
    int counter = 0;
    for(std::pair<string, string> l : lang_codes) {
        std::cout << l.first  << "\t:\t"<< l.second << std::endl;
    }    
    std::cout << std::endl;
}

void GoogleTTS::version() {
    std::cout << "gtts version: 0.3" << std::endl;
}

}
