#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <wchar.h>
#include <boost/filesystem.hpp>
#include <vector>
#include <ctime>
#include <new>
#include <stdio.h>
#include <stdlib.h>

namespace fs = boost::filesystem;

#define ae (char)0xE4
#define oe (char)0xF6
#define ue (char)0xFC

#define MAX_N (10)

int itoindex(int i, char c[], int n);
void swap(char s[]);
int indextoi(char s[]);

typedef struct{
	std::string m_str;
	unsigned int m_wert;
}lstPraefix;

std::string FindeDatum(void);
int ObsoletenListeEinlesen(std::vector<std::string>& strListeObsolet);
void EntferneObsoletes(std::string& strFileName, std::vector<std::string>& vObsoletenContainer);
void EntferneLeerzeichen(std::string& strFileName);
void printLicense(void);

std::ofstream logBuch;

int main(int argc, char** argv)
{
	system("chcp 1252");
	system("cls");

	logBuch.open("log.txt", std::ios::out|std::ios::app);
	if(logBuch.bad())
	{
		std::cout<<"Logbuch konnte nicht ge"<<oe<<"ffnet werden\n";
		system("PAUSE");
		return 1;
	}
	std::cout<<"\n.: (c)R"<<ue<<"bensoft 2023 - DatInkNmbV2 :.\n\n";

	if(argc <2)
	{
		std::cout<<"\nVerwendung: InkDatNmb.exe Datei_1 Datei_2 ...\n\n";
		system("PAUSE");
		return 1;
	}

	std::vector<std::string> vObsoletenContainer;
	ObsoletenListeEinlesen(vObsoletenContainer);

	std::string stEingabe;
	
	std::cout<<"\nBitte Pr"<<ae<<"fix eingeben\n['a' = an \\ 'aa' = an-ALDI \\ 'v' = von \\ 'vv' = von-ALDI \\ 'k' = konv \\ '*...' = beliebig]: ";
	std::getline(std::cin, stEingabe);
	switch(stEingabe[0])
	{
		case 'a':
		case 'A':
			switch(stEingabe[1])
			{
				case 'a':
				case 'A':
					stEingabe = std::string("an-ALDI");
					break;
			
				case '\0':
					stEingabe = std::string("an");
					break;
				default:
					break;
			}
			break;
		case 'v':
		case 'V':
			switch(stEingabe[1])
			{
				case 'v':
				case 'V':
					stEingabe = std::string("von-ALDI");
					break;
				case '\0':
					stEingabe = std::string("von");
					break;
				default:
					break;
			}
			break;
		case 'k':
		case 'K':
			stEingabe = std::string("konv");
			break;
		case '*':
			stEingabe = stEingabe.substr(1, std::string::npos);
			break;
		default:
			break;
	}

	lstPraefix lstPrFx;
	lstPrFx.m_str = stEingabe;
	lstPrFx.m_wert = 0;

	std::string strExt(lstPrFx.m_str);
	std::string strDatum = FindeDatum();
	
	std::cout<<"\nDatumsangabe erstellt\n"<<std::flush;
	
	char c, s[MAX_N];
	int maxIndex = -1;
	
	for(int datNr = 1; datNr < argc; datNr++)
	{
		fs::path pfad(argv[datNr]);
		if(!fs::exists(pfad))continue;
		if(!fs::is_regular_file(pfad))continue;
		
		std::string strPfad;
		fs::path subPfad = pfad.parent_path();

		for(auto&& x : fs::directory_iterator(subPfad))
		{
			fs::path pfad = x.path();
			if(fs::is_regular_file(pfad))
			{
				strPfad = pfad.filename().generic_string();
				size_t fundStelle = strPfad.find(strDatum, 0);
				if(fundStelle != 0)continue; /*strDatum steht nicht am Anfang des Dateinamens*/
				
				int i = 0;
				int datLen = strDatum.length();
				while ((c = strPfad[i + datLen]) != ' ' && i < MAX_N - 1) {
					s[i++] = c;
				}
				s[i] = '\0';
				i = indextoi(s);
				if (maxIndex < i) {
					maxIndex = i;
				}
			}
		}
	}

	if (maxIndex > -1) {
		std::string stIndizieren;
		std::cout<<"\nSoll der Index erh"<<oe<<"ht werden? [j/n] ";
		std::getline(std::cin, stIndizieren);
		if(stIndizieren[0] != 'n' && stIndizieren[0] != 'N') {
			maxIndex++;
		}

		itoindex(maxIndex, s, MAX_N);
		strDatum += s;
	}
	if (strExt.length() > 0) strDatum += " ";
	
	for (int datNr = 1; datNr < argc; datNr++) {
		fs::path pfad(argv[datNr]);
		if(fs::exists(pfad))
		{
			if(fs::is_regular_file(pfad))
			{
				std::string strDir = pfad.parent_path().generic_string();
				std::string strFileName = pfad.filename().generic_string();
				EntferneObsoletes(strFileName, vObsoletenContainer);

				EntferneLeerzeichen(strFileName);
				std::string neuerPfad = strDir + "/" + strDatum + strExt + "  -  " + strFileName;
				if(neuerPfad.length() > 255)
				{
					std::cout<<"Der generierte Pfad ist zu lang (> 256 Zeichen)\nBitte "<<neuerPfad.length() - 255<<" Zeichen aus dem Dateinamen";
					std::cout<<strDir<<"/"<<strFileName<<" entfernen, dann umbenennen neu starten\n";
					logBuch<<"Der generierte Pfad ist zu lang (> 256 Zeichen)\nBitte "<<neuerPfad.length() - 255<<" Zeichen aus dem Dateinamen";
					logBuch<<strDir<<"/"<<strFileName<<" entfernen, dann umbenennen neu starten\n";
					system("PAUSE");
					return 1;
				}
				fs::rename(pfad, neuerPfad);
			}
		}
	}

	return 0;
}

std::string FindeDatum(void)
{
	std::string strDatum;
	char chDatum[11];
	char revDatum[11];
	do{
		std::cout<<"Zu verwendendes Datum [tt.mm.yyyy \\ h \\ g \\ -[Anzahl Tage]]: ";
		std::getline(std::cin, strDatum);
		for (int i = 0; i < 11; i++) {
			chDatum[i] = strDatum[i];
		}
		if((chDatum[0] == 'h')||(chDatum[0] == 'g')||(chDatum[0] == 'H')||(chDatum[0] == 'G')||(chDatum[0] == '-'))
		{
			time_t jetzt;
			time(&jetzt);
			if((chDatum[0] == 'g')||(chDatum[0] == 'G')) jetzt -= 86400;
			if(chDatum[0] == '-')
			{
				int tageOffset = atoi(chDatum);
				std::cout<<"tageOffset: "<<tageOffset<<"\n";
				jetzt += tageOffset * 86400;
			}
			struct tm * jetztStruct = localtime(&jetzt);
			strftime (revDatum, 11, "%Y-%m-%d", jetztStruct);
			return revDatum;
		}
		if(strlen(chDatum) != 10)
		{
			std::cout<<"\nDatum weist falsches Format auf!\n";
			logBuch<<"\nDatum weist falsches Format auf!\n";
			continue;
		}
		if(chDatum[2] != '.')
		{
			std::cout<<"\nDatum weist falsches Format auf!\n";
			logBuch<<"\nDatum weist falsches Format auf!\n";
			continue;
		}
		if(chDatum[5] != '.')
		{
			std::cout<<"\nDatum weist falsches Format auf!\n";
			logBuch<<"\nDatum weist falsches Format auf!\n";
			continue;
		}
		revDatum[0] = chDatum[6];
		revDatum[1] = chDatum[7];
		revDatum[2] = chDatum[8];
		revDatum[3] = chDatum[9];
		revDatum[4] = '-';
		revDatum[5] = chDatum[3];
		revDatum[6] = chDatum[4];
		revDatum[7] = '-';
		revDatum[8] = chDatum[0];
		revDatum[9] = chDatum[1];
		revDatum[10] = '\0';
		std::cout<<"gedrehtes Datum: "<<revDatum<<'\n';
		break;
		std::cout<<"Das d"<<ue<<"rfte eigentlich nie zu sehen sein (Schleife in Datumssuche)\n";
		logBuch<<"Das d"<<ue<<"rfte eigentlich nie zu sehen sein (Schleife in Datumssuche)\n";
	}while(1);
	return revDatum;
}

int ObsoletenListeEinlesen(std::vector<std::string>& strListeObsolet)
{
	int dGroesseListe = 0;

	std::fstream fObsoletenDatei;
	fObsoletenDatei.open("Obsoletenliste.txt", std::ifstream::in);

	if(!fObsoletenDatei.good())
	{
		std::cout<<"Die Datei 'Obsoletenliste.txt' konnte nicht ge"<<oe<<"ffnet werden!\n\n";
		logBuch<<"Die Datei 'Obsoletenliste.txt' konnte nicht ge"<<oe<<"ffnet werden!\n\n";
		return -1;
	}

	char c[256];
	std::string str;
	while(!fObsoletenDatei.eof())
	{
		fObsoletenDatei.getline(c, 256);
		str = c;
		strListeObsolet.push_back(str);
		dGroesseListe++;
	}

	return dGroesseListe;
}

void EntferneObsoletes(std::string& strFileName, std::vector<std::string>& vObsoletenContainer)
{
	std::string strNewFileName;
	size_t dStelle;
	for(auto&& strObsoletes : vObsoletenContainer)
	{
		if(strObsoletes.empty())continue;
		do
		{
			dStelle = strFileName.find(strObsoletes, 0);

			if(dStelle != std::string::npos)
			{
				strFileName.erase(dStelle, strObsoletes.size());
			}
		}while (dStelle != std::string::npos);
	}
	EntferneLeerzeichen(strFileName);
	return;
}

void EntferneLeerzeichen(std::string& strFileName)
{
	size_t dStelle;
	do
	{
		dStelle = strFileName.find("  ", 0);

		if(dStelle != std::string::npos)
		{
			strFileName.erase(dStelle, (size_t)1);
		}
	}while (dStelle != std::string::npos);
	return;
}


int itoindex(int i, char s[], int n)
{
	if (i == 0) {
		s[0] = '\0';
		return 1;
	}
	
	int stelle, k = 0;
	do {
		stelle = i % 26;
		if (stelle == 0) {
			stelle = 26;
			i = i / 26 - 1;
		}
		else {
			i /= 26;
		}
		s[k++] = stelle - 1 + 'a';
		if (k == n && i != 0) {
			s[k] = '\0';
			return 1;
		}
	}while (i != 0);
	s[k] = '\0';
	swap(s);
	return 0;
}

void swap(char s[])
{
	int i, n = 0, c;
	while (s[n] != '\0') n++;
	for (i = 0; i < n / 2; i++) {
		c = s[i];
		s[i] = s[n - i - 1];
		s[n - i - 1] = c;
	}
	return;
}

int indextoi(char s[])
{
	int i = 0, c, retVal = 0;
	while ((c = s[i]) != '\0') {
		if (c >= 'a' && c <= 'z') {
			retVal = retVal * 26 + c - 'a' + 1;
		}
		i++;
	}
	return retVal;
}

void printLicense(void)
{
	std::cout<<"\n*\t*\t*\t*\t*\n";
	std::cout<<"GNU GENERAL PUBLIC LICENSE disclaimer:\n\n";
	std::cout<<"This program is free software: you can redistribute it and/or modify\n";
	std::cout<<"it under the terms of the GNU General Public License as published by\n";
	std::cout<<"the Free Software Foundation, either version 3 of the License, or\n";
	std::cout<<"(at your option) any later version.\n\n";
	std::cout<<"This program is distributed in the hope that it will be useful,\n";
	std::cout<<"but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
	std::cout<<"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
	std::cout<<"GNU General Public License for more details.\n\n";
	std::cout<<"You should have received a copy of the GNU General Public License\n";
	std::cout<<"along with this program.  If not, see <http://www.gnu.org/licenses/>\n\n";
	std::cout<<"\n*\t*\t*\t*\t*\n";

	std::cout<<"Boost Software License - Version 1.0 - August 17th, 2003\n";
	std::cout<<"\n";
	std::cout<<"Permission is hereby granted, free of charge, to any person or organization\n";
	std::cout<<"obtaining a copy of the software and accompanying documentation covered by\n";
	std::cout<<"this license (the \"Software\") to use, reproduce, display, distribute,\n";
	std::cout<<"execute, and transmit the Software, and to prepare derivative works of the\n";
	std::cout<<"Software, and to permit third-parties to whom the Software is furnished to\n";
	std::cout<<"do so, all subject to the following:\n";
	std::cout<<"\n";
	std::cout<<"The copyright notices in the Software and this entire statement, including\n";
	std::cout<<"the above license grant, this restriction and the following disclaimer,\n";
	std::cout<<"must be included in all copies of the Software, in whole or in part, and\n";
	std::cout<<"all derivative works of the Software, unless such copies or derivative\n";
	std::cout<<"works are solely in the form of machine-executable object code generated by\n";
	std::cout<<"a source language processor.\n";
	std::cout<<"\n";
	std::cout<<"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n";
	std::cout<<"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n";
	std::cout<<"FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT\n";
	std::cout<<"SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE\n";
	std::cout<<"FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,\n";
	std::cout<<"ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER\n";
	std::cout<<"DEALINGS IN THE SOFTWARE.\n";
	std::cout<<"\n*\t*\t*\t*\t*\n";
	
	std::cout<<"Please find the licenses in the \"lic\"-folder\n\n";
	return;
}
