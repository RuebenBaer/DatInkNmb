#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <wchar.h>
#include <boost/filesystem.hpp>
#include <vector>
#include <ctime>
#include <new> 

namespace fs = boost::filesystem;

typedef struct{
	std::string m_str;
	unsigned int m_wert;
}lstPraefix;

int MAX_LST_GROESSE = 50;
int MAX_BENUTZT = 0;

void PraefixFinden(std::string, lstPraefix*);
std::size_t PraefixEndeErmitteln(std::string dateiname, std::size_t& prfxStart);
void PraefixListeHinzufuegen(std::string, lstPraefix*);
std::string FindeDatum(void);
int ObsoletenListeEinlesen(std::vector<std::string>& strListeObsolet);
void EntferneObsoletes(std::string& strFileName, std::vector<std::string>& vObsoletenContainer);
void EntferneLeerzeichen(std::string& strFileName);
void printLicense(void);


int main(int argc, char** argv)
{
	std::printf("\n.: (c)Ru:bensoft 2023 - DatInkNmbV2 :.\n\n");
	if(argc <2)
	{
		std::cout<<"\nVerwendung: InkDatNmb.exe Datei_1 Datei_2 ...\n\n";
		system("PAUSE");
		return 1;
	}

	std::vector<std::string> vObsoletenContainer;
	ObsoletenListeEinlesen(vObsoletenContainer);

	lstPraefix lstPrFx[MAX_LST_GROESSE];
	unsigned int uiMaxLstEintrag = 0;
	for(int datNr = 1; datNr < argc; datNr++)
	{
		fs::path pfad(argv[datNr]);
		if(fs::exists(pfad))
		{
			if(fs::is_regular_file(pfad))
			{
				std::vector<fs::path> v;
				for(int i = 0; i < MAX_LST_GROESSE; i++)
					lstPrFx[i].m_str = "";

				fs::path subPfad = pfad.parent_path();

				for(auto&& x : fs::directory_iterator(subPfad))
					v.push_back(x.path());
				for(auto&& y : v)
				{
					if(is_regular_file(y))
					{
						PraefixFinden(y.filename().generic_string(), lstPrFx);
					}else
					{
						//std::cout<<y<<" ist keine Datei\n";
					}
				}
			}
		}
		else
		{
			std::cout<<"Datei "<<argv[datNr]<<"existiert nicht\n";
		}
	}

	std::string stEingabe;
	unsigned int uiGewPrFx = -1;

	if(MAX_BENUTZT)
	{
		std::cout<<"Welches Praefix soll verwendet werden:\n\n";
		std::cout<<"[0]: "<<"Neues Praefix\n\n";
		for(int i = 0; i<MAX_LST_GROESSE; i++)
		{
			if(!lstPrFx[i].m_str.empty())
			{
				std::cout<<"["<<i+1<<"]: "<<lstPrFx[i].m_str<<"\n\n";
				uiMaxLstEintrag = i;
			}else
				break;
		}
		std::cout<<"\n[Nummer\\lic]: ";
		do
		{
			std::getline(std::cin, stEingabe);
			if(stEingabe[0] == '0')
			{

				std::cout<<"\n\nNeues Praefix eingeben: ";
				std::getline(std::cin, stEingabe);
				lstPrFx[0].m_str = stEingabe;
				lstPrFx[0].m_wert = 0;
				uiGewPrFx = 0;
				break;
			}
			if((stEingabe[0] <'0')||(stEingabe[0]>'9'))
			{
				if(stEingabe.compare("lic") == 0)
				{
					printLicense();
					std::cout<<"\n[Nummer\\lic]: ";
					continue;
				}else{
				std::cout<<"Vorgang abgebrochen\n";
				system("pause");
				return 0;
				}
			}
			uiGewPrFx = stoi(stEingabe)-1;
			if(uiMaxLstEintrag < uiGewPrFx)
				std::cout<<"[Fehler] Bitte Zahl kleiner "<<uiMaxLstEintrag+1<<" eingeben: ";
		}while(uiMaxLstEintrag < uiGewPrFx);
	}else
	{
		std::cout<<"\nKein Praefix vorhanden\nBitte Praefix eingeben: ";
		std::getline(std::cin, stEingabe);
		lstPrFx[0].m_str = stEingabe;
		lstPrFx[0].m_wert = 0;
		uiGewPrFx = 0;
	}

	std::string strExt(lstPrFx[uiGewPrFx].m_str);
	std::string strDatum = FindeDatum();
	
	std::cout<<"\nDatumsangabe erstellt\n"<<std::flush;
	
	size_t indexLen = 0;
	char indexChar = ' ';
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
				if(fundStelle != 0)continue;
				
				fundStelle = strPfad.find(" ", 0) - strDatum.length();
				if(fundStelle == 0)
				{
					if(indexLen == 0)
					{
						indexChar = 'a';
						indexLen = 1;
					}
					continue;
				}
				if(indexLen > fundStelle)continue;
				indexLen = fundStelle;
				if(strPfad[strDatum.length()+indexLen-1] < 'z')
				{
					if(indexChar > strPfad[strDatum.length()+indexLen-1])continue;
					indexChar = strPfad[strDatum.length()+indexLen-1] + 1;
				}
				else
				{
					indexChar = 'a';
					indexLen++;
				}
			}
		}
	}
	
	if(indexLen != 0)
	{
		for(size_t i = 0; i < indexLen-1; i++)
		{
			strDatum += "z";
		}
		strDatum += indexChar;
	}
	strDatum += " ";
	
	for(int datNr = 1; datNr < argc; datNr++)
	{
		fs::path pfad(argv[datNr]);
		if(fs::exists(pfad))
		{
			if(fs::is_regular_file(pfad))
			{
				std::string strDir = pfad.parent_path().generic_string();
				std::string strFileName = pfad.filename().generic_string();
				EntferneObsoletes(strFileName, vObsoletenContainer);
				
				std::string neuerPfad = strDir + "/" + strDatum + strExt + " - " + strFileName;
				if(neuerPfad.length() > 255)
				{
					std::cout<<"Der generierte Pfad ist zu lang (> 256 Zeichen)\nBitte "<<neuerPfad.length() - 255<<" Zeichen entfernen, dann umbenennen neu starten\n";
					system("PAUSE");
					return 1;
				}
				EntferneLeerzeichen(neuerPfad);
				fs::rename(pfad, neuerPfad);
			}
		}
	}

	return 0;
}

std::string FindeDatum(void)
{
	char strDatum[11];
	char revDatum[11];
	do{
		std::cout<<"Zu verwendendes Datum [tt.mm.yyyy \\ h \\ g \\ -[Anzahl Tage]]:";
		std::cin>>strDatum;
		if((strDatum[0] == 'h')||(strDatum[0] == 'g')||(strDatum[0] == 'H')||(strDatum[0] == 'G')||(strDatum[0] == '-'))
		{
			time_t jetzt;
			time(&jetzt);
			if((strDatum[0] == 'g')||(strDatum[0] == 'G')) jetzt -= 86400;
			if(strDatum[0] == '-')
			{
				int tageOffset = atoi(strDatum);
				std::cout<<"tageOffset: "<<tageOffset<<"\n";
				jetzt += tageOffset * 86400;
			}
			struct tm * jetztStruct = localtime(&jetzt);
			strftime (revDatum, 11, "%Y-%m-%d", jetztStruct);
			return revDatum;
		}
		if(strlen(strDatum) != 10)
		{
			std::cout<<"\nDatum weist falsches Format auf!\n";
			continue;
		}
		if(strDatum[2] != '.')
		{
			std::cout<<"\nDatum weist falsches Format auf!\n";
			continue;
		}
		if(strDatum[5] != '.')
		{
			std::cout<<"\nDatum weist falsches Format auf!\n";
			continue;
		}
		revDatum[0] = strDatum[6];
		revDatum[1] = strDatum[7];
		revDatum[2] = strDatum[8];
		revDatum[3] = strDatum[9];
		revDatum[4] = '-';
		revDatum[5] = strDatum[3];
		revDatum[6] = strDatum[4];
		revDatum[7] = '-';
		revDatum[8] = strDatum[0];
		revDatum[9] = strDatum[1];
		revDatum[10] = '\0';
		std::cout<<"gedrehtes Datum: "<<revDatum<<'\n';
		break;
		std::cout<<"Das du:rfte eigentlich nie zu sehen sein (Schleife in Datumssuche)\n";
	}while(1);
	return revDatum;
}

void PraefixFinden(std::string dateiname, lstPraefix* lstPrFx)
{
	std::string strAn("An-");
	std::string strVon("Von-");
	std::size_t fundStelle;

	fundStelle = (dateiname).find(strAn, 0);
	if(fundStelle != std::string::npos) /*!= std::string::npos fuer das Auffinden an beliebiger Stelle*/
	{
		std::size_t pos = fundStelle + strAn.length();
		std::size_t prfxEnde = PraefixEndeErmitteln(dateiname, pos);
		PraefixListeHinzufuegen(std::string(dateiname, fundStelle, prfxEnde - fundStelle + 1), lstPrFx);
	}
	fundStelle = (dateiname).find(strVon, 0);
	if(fundStelle != std::string::npos) /*!= std::string::npos fuer das Auffinden an beliebiger Stelle*/
	{
		std::size_t pos = fundStelle + strVon.length();
		std::size_t prfxEnde = PraefixEndeErmitteln(dateiname, pos);
		PraefixListeHinzufuegen(std::string(dateiname, fundStelle, prfxEnde - fundStelle + 1), lstPrFx);
	}
	return;
}

void PraefixListeHinzufuegen(std::string str, lstPraefix* lstPrFx)
{
	for(int i = 0; i<MAX_LST_GROESSE; i++)
	{
		if(lstPrFx[i].m_str.empty())
		{
			lstPrFx[i].m_str = str;
			MAX_BENUTZT++;
			return;
		}
		if(lstPrFx[i].m_str == str)
		{
			/*nichts weiter machen*/
			return;
		}
	}
	return;
}

std::size_t PraefixEndeErmitteln(std::string dateiname, std::size_t& prfxStart)
{
	std::size_t prfxEnde = prfxStart;
	for(size_t i = prfxStart; i < dateiname.length(); i++)
	{
		if((dateiname[i] != ' ')&&(dateiname[i] != '-')&&(dateiname[i] != '_'))
		{
			prfxEnde = i;
		}else
		{
			return prfxEnde;
		}
	}
	return prfxEnde;
}

int ObsoletenListeEinlesen(std::vector<std::string>& strListeObsolet)
{
	int dGroesseListe = 0;

	std::fstream fObsoletenDatei;
	fObsoletenDatei.open("Obsoletenliste.txt", std::ifstream::in);

	if(fObsoletenDatei.good())
	{
		char c[256];
		std::string str;
		while(!fObsoletenDatei.eof())
		{
			fObsoletenDatei.getline(c, 256);
			str = c;
			strListeObsolet.push_back(str);
			dGroesseListe++;
		}
	}
	else
	{
		std::cout<<"Die Datei 'Obsoletenliste.txt' konnte nicht geöffnet werden!\n\n";
		system("PAUSE");
		return -1;
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